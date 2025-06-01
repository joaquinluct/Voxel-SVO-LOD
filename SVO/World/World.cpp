#include "World.h"
#include "../SVOFile/SVO_FileManager.h"
#include <cmath>
#include <functional>
#include <algorithm>
#include "../Voxel/VoxelData.h"
#include "../MarchingCubes/MarchingCubes.h"
#include "../Voxel/VoxelMesh.h"

World::World(DeviceManager* deviceManager, Camera* camera)
    : m_deviceManager(deviceManager), m_camera(camera), m_material(nullptr) {
    // Configuración básica de LOD
    m_lodSettings.nearDistance = 128.0f;
    m_lodSettings.midDistance = 256.0f;
    m_lodSettings.farDistance = 512.0f;
}

World::~World() {
    // Limpieza automática por unique_ptr
}

HRESULT World::Init(Material* material) {
    m_material = material;
    // Aquí podrías cargar recursos adicionales si es necesario
    return S_OK;
}

void World::Update(float deltaTime) {
    UpdateVisibleNodes();
}

void World::Render(ID3D11DeviceContext* context) {
    VoxelData voxelData; // Instancia temporal, reemplaza por tu sistema real
    MarchingCubes marchingCubes;
    MarchingCubesMesh mainMesh; // Aquí se almacenará el mesh principal
    DirectX::XMFLOAT3 mainOrigin;
	bool isMainOriginSet = false;
    for (const auto& info : m_visibleNodes) {
        SVO_Node* node = info.node;
        const DirectX::XMFLOAT3& origin = info.origin;
        if (!isMainOriginSet) {
		    mainOrigin = origin; // Asignar el origen del primer nodo visible
		    isMainOriginSet = true; // Marcar que se ha establecido el origen principal
		}
        float size = info.size;
        std::array<bool, 6> lodTransitions{};
        for (int face = 0; face < 6; ++face) lodTransitions[face] = node->GetLODTransition(face);
        MarchingCubesMesh mesh = marchingCubes.GenerateMesh(node, origin, size, &voxelData, lodTransitions, this);
        // drawMesh(context, mesh, ...);
        // Replace the line causing the error with the following:
        /*if (!node->IsLeaf()) {
            int a = 0;
        }
        else {
            int b = 0;
        }*/
        if (mesh.vertices.empty() || mesh.indices.empty()) continue; // No hay datos para renderizar        
		mainMesh.addMesh(mesh); // Acumular el mesh principal
    }

    // Eliminar vértices duplicados en el mesh principal
    //mainMesh.removeDuplicateVertices();

    // Renderizar el mesh principal
    std::unique_ptr<VoxelMesh> voxelMesh = std::make_unique<VoxelMesh>(m_deviceManager, m_material);
    voxelMesh->Init(m_deviceManager->GetDevice(), mainMesh);
    // Calcular la WorldMatrix para este nodo    
    XMMATRIX worldMatrix = XMMatrixTranslation(mainOrigin.x, mainOrigin.y, mainOrigin.z);
    // XMMATRIX worldMatrix = XMMatrixIdentity();
    voxelMesh->Render(context, worldMatrix, m_camera->GetViewMatrix(), m_camera->GetProjectionMatrix());
}

AreaKey World::GetAreaKeyFromPosition(const DirectX::XMFLOAT3& pos) const {
    return AreaKey{
        static_cast<int>(std::floor(pos.x / AREA_SIZE)),
        static_cast<int>(std::floor(pos.y / AREA_SIZE)),
        static_cast<int>(std::floor(pos.z / AREA_SIZE))
    };
}

SVO_Node* World::GetArea(const AreaKey& key) {
    auto it = m_areas.find(key);
    if (it != m_areas.end()) return it->second.get();
    return nullptr; // No existe el área
}

SVO_Node* World::GetOrCreateArea(const AreaKey& key) {
    auto it = m_areas.find(key);
    if (it != m_areas.end()) return it->second.get();
    // Intentar cargar desde disco
    // SVO_Node* loaded = SVO_FileManager::LoadSVO(filename);
    // if (loaded) { m_areas[key] = std::unique_ptr<SVO_Node>(loaded); return loaded; }
    // Si no existe, generar SVO procedural: Y < 0 lleno, Y >= 0 vacío
    auto node = std::make_unique<SVO_Node>(false);

    // Recursivo: subdivide hasta hoja, marca ocupación según Y
    std::function<void(SVO_Node*, float, float, float, float, int)> fillNode;
    fillNode = [&](SVO_Node* n, float ox, float oy, float oz, float size, int depth) {
        if (size <= 1.0f || depth > 6) { // Hoja
            // Marca como ocupada si la hoja intersecta el cubo [5,10]^3
            if (ox < 10.0f && ox + size > 5.0f &&
                oy < 10.0f && oy + size > 5.0f &&
                oz < 10.0f && oz + size > 5.0f) {
                n->SetOccupied(true);
                n->SetIsLeaf(true);
                return;
            }
            n->SetIsLeaf(true);
            return;
        }
        n->Subdivide();
        bool anyChildOccupied = false;
        float childSize = size / 2.0f;
        for (int i = 0; i < 8; ++i) {
            float cx = ox + ((i & 1) ? childSize : 0);
            float cy = oy + ((i & 2) ? childSize : 0);
            float cz = oz + ((i & 4) ? childSize : 0);
            SVO_Node* child = n->GetChild(i);
            fillNode(child, cx, cy, cz, childSize, depth + 1);
            if (child->IsOccupied()) anyChildOccupied = true;
        }
        n->SetAreAllChildrenOccupied(n->AreAllChildrenOccupied());
        n->SetOccupied(anyChildOccupied);
    };
    fillNode(node.get(), key.x * AREA_SIZE, key.y * AREA_SIZE, key.z * AREA_SIZE, AREA_SIZE, 0);
    SVO_Node* ptr = node.get();
    m_areas[key] = std::move(node);
    return ptr;
}

void World::UpdateVisibleNodes() {
    m_visibleNodes.clear();
    DirectX::XMFLOAT3 camPos = m_camera->GetPosition();
    AreaKey areaKey = GetAreaKeyFromPosition(camPos);
    float farPlane = m_camera->GetFarPlane();
    constexpr int MAX_AREA_RADIUS = 3;
    int radio = static_cast<int>(std::ceil(farPlane / AREA_SIZE));
    radio = std::clamp(radio, 1, MAX_AREA_RADIUS);
    for (int dx = -radio; dx <= radio; ++dx) {
        for (int dy = -radio; dy <= radio; ++dy) {
            for (int dz = -radio; dz <= radio; ++dz) {
                AreaKey key{areaKey.x + dx, areaKey.y + dy, areaKey.z + dz};
                SVO_Node* root = GetOrCreateArea(key);
                if (root && root->IsOccupied()) {
                    DirectX::XMFLOAT3 areaOrigin = { static_cast<float>(key.x) * AREA_SIZE, static_cast<float>(key.y) * AREA_SIZE, static_cast<float>(key.z) * AREA_SIZE };
                    LODProcessor::ProcessLOD(root, areaOrigin, camPos, AREA_SIZE, m_lodSettings, m_visibleNodes);
                }
            }
        }
    }
    // --- Detección de bordes LOD y marcado de transiciones ---
    //for (auto& info : m_visibleNodes) {
    //    SVO_Node* node = info.node;
    //    node->ClearLODTransitions();
    //    for (int face = 0; face < 6; ++face) {
    //        DirectX::XMFLOAT3 neighborOffset = {0,0,0};
    //        switch(face) {
    //            case 0: neighborOffset.x = -1; break;
    //            case 1: neighborOffset.x =  1; break;
    //            case 2: neighborOffset.y = -1; break;
    //            case 3: neighborOffset.y =  1; break;
    //            case 4: neighborOffset.z = -1; break;
    //            case 5: neighborOffset.z =  1; break;
    //        }
    //        // Calcular el área y origen del vecino
    //        DirectX::XMFLOAT3 neighborOrigin = info.origin;
    //        neighborOrigin.x += neighborOffset.x * info.size;
    //        neighborOrigin.y += neighborOffset.y * info.size;
    //        neighborOrigin.z += neighborOffset.z * info.size;
    //        AreaKey neighborKey = GetAreaKeyFromPosition(neighborOrigin);
    //        SVO_Node* neighborRoot = GetArea(neighborKey);
    //        if (!neighborRoot || !neighborRoot->IsOccupied()) {
    //            node->SetLODTransition(face, false);
    //            continue;
    //        }
    //        // Buscar el nodo vecino de igual o mayor tamaño que contenga neighborOrigin            
    //        float neighborSize = AREA_SIZE;
    //        DirectX::XMFLOAT3 nOrigin = { static_cast<float>(neighborKey.x) * AREA_SIZE, static_cast<float>(neighborKey.y) * AREA_SIZE, static_cast<float>(neighborKey.z) * AREA_SIZE };
    //        while (!neighborRoot->IsLeaf() && neighborSize > info.size) {
    //            float childSize = neighborSize / 2.0f;
    //            int childIdx = 0;
    //            if (neighborOrigin.x >= nOrigin.x + childSize) childIdx |= 1;
    //            if (neighborOrigin.y >= nOrigin.y + childSize) childIdx |= 2;
    //            if (neighborOrigin.z >= nOrigin.z + childSize) childIdx |= 4;
    //            if (!neighborRoot->HasChild(childIdx)) break;
    //            neighborRoot = neighborRoot->GetChild(childIdx);
    //            nOrigin.x += (childIdx & 1) ? childSize : 0;
    //            nOrigin.y += (childIdx & 2) ? childSize : 0;
    //            nOrigin.z += (childIdx & 4) ? childSize : 0;
    //            neighborSize = childSize;
    //        }
    //        // Si el tamaño del vecino es diferente, marcar transición
    //        bool transition = (std::abs(neighborSize - info.size) > 1e-3f);
    //        node->SetLODTransition(face, transition);
    //    }

    //}
}
