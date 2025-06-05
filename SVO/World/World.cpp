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
    m_lodSettings.nearDistance = 1128.0f;
    m_lodSettings.midDistance = 1256.0f;
    m_lodSettings.farDistance = 1512.0f;
	m_mainOriginRender = { 0, 0, 0 };
    m_mainOriginUpdate = { 0, 0, 0 };
	m_depth = 6; // Profundidad inicial
}

World::~World() {
    // Limpieza automática por unique_ptr
}

HRESULT World::Init(Material* material) {
    m_material = material;
    // Aquí podrías cargar recursos adicionales si es necesario
    return S_OK;
}

// --- Miembros para double buffering del mesh ---
MarchingCubesMesh m_mainMeshUpdate;
MarchingCubesMesh m_mainMeshRender;
DirectX::XMFLOAT3 m_mainOriginUpdate;
DirectX::XMFLOAT3 m_mainOriginRender;

void World::Update(float deltaTime) {
    std::lock_guard<std::mutex> lock(worldMutex);
    m_visibleNodesUpdate.clear();
    m_mainMeshUpdate = MarchingCubesMesh(); // Limpiar el mesh de update
    m_mainOriginUpdate = {0,0,0};
    DirectX::XMFLOAT3 camPos = m_camera->GetPosition();
    AreaKey areaKey = GetAreaKeyFromPosition(camPos);
    float farPlane = m_camera->GetFarPlane();
    constexpr int MAX_AREA_RADIUS = 1;
    int radio = static_cast<int>(std::ceil(farPlane / AREA_SIZE));
    radio = std::clamp(radio, 1, MAX_AREA_RADIUS);
    VoxelData voxelData;
    MarchingCubes marchingCubes;
    bool isMainOriginSet = false;
    for (int dx = -radio; dx <= radio; ++dx) {
        for (int dy = -radio; dy <= radio; ++dy) {
            for (int dz = -radio; dz <= radio; ++dz) {
                AreaKey key{areaKey.x + dx, areaKey.y + dy, areaKey.z + dz};
                SVO_Node* root = GetOrCreateArea(key);
                if (root && root->IsOccupied()) {
                    DirectX::XMFLOAT3 areaOrigin = { static_cast<float>(key.x) * AREA_SIZE, static_cast<float>(key.y) * AREA_SIZE, static_cast<float>(key.z) * AREA_SIZE };
                    LODProcessor::ProcessLOD(root, areaOrigin, camPos, AREA_SIZE, m_lodSettings, m_visibleNodesUpdate);
                }
            }
        }
    }

    /*for (int x = 80; x < 120; x++) {
        for (int y = 80; y < 120; y++) {
            for (int z = 80; z < 120; z++) {
            }
        }
    }*/

    // --- Detección de bordes LOD y marcado de transiciones + generación de mesh ---
    for (auto& info : m_visibleNodesUpdate) {
        SVO_Node* node = info.node;
        // char buf[128];
        // sprintf_s(buf, "VISIBLE: origin=%.1f %.1f %.1f size=%.1f\n", info.origin.x, info.origin.y, info.origin.z, info.size);
        // OutputDebugStringA(buf);
        node->ClearLODTransitions();
        for (int face = 0; face < 6; ++face) {
            DirectX::XMFLOAT3 neighborOffset = {0,0,0};
            switch(face) {
                case 0: neighborOffset.x = -1; break;
                case 1: neighborOffset.x =  1; break;
                case 2: neighborOffset.y = -1; break;
                case 3: neighborOffset.y =  1; break;
                case 4: neighborOffset.z = -1; break;
                case 5: neighborOffset.z =  1; break;
            }
            DirectX::XMFLOAT3 neighborOrigin = info.origin;
            neighborOrigin.x += neighborOffset.x * info.size;
            neighborOrigin.y += neighborOffset.y * info.size;
            neighborOrigin.z += neighborOffset.z * info.size;
            AreaKey neighborKey = GetAreaKeyFromPosition(neighborOrigin);
            SVO_Node* neighborRoot = GetArea(neighborKey);
            if (!neighborRoot || !neighborRoot->IsOccupied()) {
                node->SetLODTransition(face, false);
                continue;
            }
            float neighborSize = AREA_SIZE;
            DirectX::XMFLOAT3 nOrigin = { static_cast<float>(neighborKey.x) * AREA_SIZE, static_cast<float>(neighborKey.y) * AREA_SIZE, static_cast<float>(neighborKey.z) * AREA_SIZE };
            while (!neighborRoot->IsLeaf() && neighborSize > info.size) {
                float childSize = neighborSize / 2.0f;
                int childIdx = 0;
                if (neighborOrigin.x >= nOrigin.x + childSize) childIdx |= 1;
                if (neighborOrigin.y >= nOrigin.y + childSize) childIdx |= 2;
                if (neighborOrigin.z >= nOrigin.z + childSize) childIdx |= 4;
                if (!neighborRoot->HasChild(childIdx)) break;
                neighborRoot = neighborRoot->GetChild(childIdx);
                nOrigin.x += (childIdx & 1) ? childSize : 0;
                nOrigin.y += (childIdx & 2) ? childSize : 0;
                nOrigin.z += (childIdx & 4) ? childSize : 0;
                neighborSize = childSize;
            }
            bool transition = (std::abs(neighborSize - info.size) > 1e-3f);
            node->SetLODTransition(face, transition);
        }
        // --- Generar mesh para cada nodo visible ---
        const DirectX::XMFLOAT3& origin = info.origin;
        if (!isMainOriginSet) {
            m_mainOriginUpdate = origin;
            isMainOriginSet = true;
        }
        const float size = info.size;
        std::array<bool, 6> lodTransitions{};
        for (int face = 0; face < 6; ++face) lodTransitions[face] = node->GetLODTransition(face);
        MarchingCubesMesh mesh = marchingCubes.GenerateMesh(node, origin, size, &voxelData, lodTransitions, this);
        if (!mesh.vertices.empty() && !mesh.indices.empty()) {
            m_mainMeshUpdate.addMesh(mesh);
        }
    }
    // --- Comparar nodos visibles ---
    //if (m_visibleNodesUpdate != m_visibleNodesRender) {
    //    hasNewVisibleNodes = true; // Solo marcar si hay cambios
    //}
    hasNewVisibleNodes = true;
}

void World::Render(ID3D11DeviceContext* context) {
    // Usar try_lock para no bloquear el render si el update está en progreso
    if (hasNewVisibleNodes) {
        if (worldMutex.try_lock()) {
            m_visibleNodesRender.swap(m_visibleNodesUpdate);
            m_mainMeshRender = std::move(m_mainMeshUpdate);
            m_mainOriginRender = m_mainOriginUpdate;
            hasNewVisibleNodes = false;
            worldMutex.unlock();
        }
        // Si no se puede hacer swap, sigue renderizando el último buffer válido
    }
    if (m_mainMeshRender.vertices.empty() || m_mainMeshRender.indices.empty()) {
        return;
    }
    std::unique_ptr<VoxelMesh> voxelMesh = std::make_unique<VoxelMesh>(m_deviceManager, m_material);
    voxelMesh->Init(m_deviceManager->GetDevice(), m_mainMeshRender);
    // Cambiar a matriz de mundo identidad para evitar doble translación
    XMMATRIX worldMatrix = XMMatrixIdentity();
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
        if (size <= 1.0f || depth > m_depth) { // Hoja          
            XMFLOAT3 pos{ ox,oy,oz };
            /*float boxInit = 10.0f;
            float boxEnd = 100.0f;*/
            
            // Un nodo se considera 'ocupado' si su volumen [ox, ox+size) (y_y, z_z):
            // 1. Intersecta el volumen del material [boxInit, boxEnd).
            // O
            // 2. Está adyacente al volumen del material, en un rango que una celda de Marching Cubes
            //    (cuyo origen sea este nodo) podría generar la superficie.
            //    Esto lo logramos "expandiendo" ligeramente el bounding box del material.
            /*bool node_volume_inside_material =
                (ox >= boxInit && ox <= boxEnd &&
                    oy >= boxInit && oy <= boxEnd &&
                    oz >= boxInit && oz <= boxEnd);

            bool node_volume_intersects_material =
                (ox-size >= boxInit && ox+size <= boxInit &&
                    oy >= boxInit && oy <= boxEnd &&
                    oz >= boxInit && oz <= boxEnd);

            n->SetOccupied(node_volume_intersects_material);*/

            // const float density = CubeSDF(pos, XMFLOAT3{ 20.0f,20.0f,20.0f }, XMFLOAT3{ 100.0f,100.0f,100.0f });
            const float density = SphereSDF(pos, XMFLOAT3{ 120.0f,120.0f,120.0f }, 80.0f);

            n->SetOccupied(density < 0.99999f);
            n->SetDensity(density > 1.0f ? 1.0f : density);
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
        //n->SetAreAllChildrenOccupied(n->AreAllChildrenOccupied());
        n->SetOccupied(anyChildOccupied);
        };
    fillNode(node.get(), key.x * AREA_SIZE, key.y * AREA_SIZE, key.z * AREA_SIZE, AREA_SIZE, 0);
    SVO_Node* ptr = node.get();
    m_areas[key] = std::move(node);
    return ptr;
}


void World::SetDepth(int depth) { 
    m_depth = depth;
	UpdateVisibleNodes(); // Actualizar nodos visibles al cambiar profundidad
	Update(0.0f); // Forzar actualización del mesh
}

void World::UpdateVisibleNodes() {
    m_visibleNodesUpdate.clear();
    DirectX::XMFLOAT3 camPos = m_camera->GetPosition();
    AreaKey areaKey = GetAreaKeyFromPosition(camPos);
    float farPlane = m_camera->GetFarPlane();
    constexpr int MAX_AREA_RADIUS = 1;
    int radio = static_cast<int>(std::ceil(farPlane / AREA_SIZE));
    radio = std::clamp(radio, 1, MAX_AREA_RADIUS);
    for (int dx = -radio; dx <= radio; ++dx) {
        for (int dy = -radio; dy <= radio; ++dy) {
            for (int dz = -radio; dz <= radio; ++dz) {
                AreaKey key{ areaKey.x + dx, areaKey.y + dy, areaKey.z + dz };
                SVO_Node* root = GetOrCreateArea(key);
                if (root && root->IsOccupied()) {
                    DirectX::XMFLOAT3 areaOrigin = { static_cast<float>(key.x) * AREA_SIZE, static_cast<float>(key.y) * AREA_SIZE, static_cast<float>(key.z) * AREA_SIZE };
                    LODProcessor::ProcessLOD(root, areaOrigin, camPos, AREA_SIZE, m_lodSettings, m_visibleNodesUpdate);
                }
            }
        }
    }
    // --- Detección de bordes LOD y marcado de transiciones ---
    for (auto& info : m_visibleNodesUpdate) {
        SVO_Node* node = info.node;
        node->ClearLODTransitions();
        for (int face = 0; face < 6; ++face) {
            DirectX::XMFLOAT3 neighborOffset = { 0,0,0 };
            switch (face) {
            case 0: neighborOffset.x = -1; break;
            case 1: neighborOffset.x = 1; break;
            case 2: neighborOffset.y = -1; break;
            case 3: neighborOffset.y = 1; break;
            case 4: neighborOffset.z = -1; break;
            case 5: neighborOffset.z = 1; break;
            }
            // Calcular el área y origen del vecino
            DirectX::XMFLOAT3 neighborOrigin = info.origin;
            neighborOrigin.x += neighborOffset.x * info.size;
            neighborOrigin.y += neighborOffset.y * info.size;
            neighborOrigin.z += neighborOffset.z * info.size;
            AreaKey neighborKey = GetAreaKeyFromPosition(neighborOrigin);
            SVO_Node* neighborRoot = GetArea(neighborKey);
            if (!neighborRoot || !neighborRoot->IsOccupied()) {
                node->SetLODTransition(face, false);
                continue;
            }
            // Buscar el nodo vecino de igual o mayor tamaño que contenga neighborOrigin            
            //float neighborSize = AREA_SIZE;
            float neighborSize = AREA_SIZE;
            DirectX::XMFLOAT3 nOrigin = { static_cast<float>(neighborKey.x) * AREA_SIZE, static_cast<float>(neighborKey.y) * AREA_SIZE, static_cast<float>(neighborKey.z) * AREA_SIZE };
            while (!neighborRoot->IsLeaf() && neighborSize > info.size) {
                float childSize = neighborSize / 2.0f;
                int childIdx = 0;
                if (neighborOrigin.x >= nOrigin.x + childSize) childIdx |= 1;
                if (neighborOrigin.y >= nOrigin.y + childSize) childIdx |= 2;
                if (neighborOrigin.z >= nOrigin.z + childSize) childIdx |= 4;
                if (!neighborRoot->HasChild(childIdx)) break;
                neighborRoot = neighborRoot->GetChild(childIdx);
                nOrigin.x += (childIdx & 1) ? childSize : 0;
                nOrigin.y += (childIdx & 2) ? childSize : 0;
                nOrigin.z += (childIdx & 4) ? childSize : 0;
                neighborSize = childSize;
            }
            // Si el tamaño del vecino es diferente, marcar transición
            bool transition = (std::abs(neighborSize - info.size) > 1e-3f);
            node->SetLODTransition(face, transition);
        }
    }
    hasNewVisibleNodes = true;
}

float World::GetVoxelDensity(const DirectX::XMFLOAT3& worldPos, const SVO_Node* nodeRef, const float nodeSize) {
    // 1. Obtener la clave del área (chunk) y el nodo raíz SVO para esa área.
    AreaKey areaKey = GetAreaKeyFromPosition(worldPos);
    SVO_Node* currentSVO_Node = GetOrCreateArea(areaKey); // Llama al helper que carga/crea

    // Si el nodo raíz del SVO del chunk es nulo o no está ocupado, el chunk está vacío.
    if (!currentSVO_Node || !currentSVO_Node->IsOccupied()) {
        return 1.0f; // Vacío
    }

    DirectX::XMFLOAT3 currentSVO_Origin = worldPos;
    float currentSVO_Size = AREA_SIZE_F;

    // 2. Recorrer el SVO para encontrar el nodo más específico para 'worldPos'.
    //    El recorrido se detendrá si:
    //    - Se llega a una hoja del SVO.
    //    - El tamaño del nodo actual es menor o igual al `targetNodeSize` solicitado.
    //    - El punto cae en una región sin hijo o el hijo no está ocupado (poda).

    while (currentSVO_Node && !currentSVO_Node->IsLeaf() && currentSVO_Size > 1.0f) {
        // Calcular el índice del hijo donde debe estar worldPos
        float half = currentSVO_Size / 2.0f;
        int childIndex = 0;
        if (worldPos.x >= currentSVO_Origin.x + half) childIndex |= 1;
        if (worldPos.y >= currentSVO_Origin.y + half) childIndex |= 2;
        if (worldPos.z >= currentSVO_Origin.z + half) childIndex |= 4;

        // Condición de parada para el recorrido del SVO:
        // A) Si el nodo actual no tiene el hijo que buscamos.
        // B) Si el hijo existe pero no está ocupado (poda del SVO).
        // C) Si el tamaño del siguiente nivel de subdivisión (half) ya es más pequeño
        //    que el 'targetNodeSize' solicitado por Marching Cubes.
        //    Usamos una pequeña tolerancia para comparaciones de floats.
        if (!currentSVO_Node->HasChild(childIndex) ||
            !currentSVO_Node->GetChild(childIndex)->IsOccupied() ||
            half < nodeSize - 0.001f) // Condición para detenerse en el LOD solicitado
        {
            break;
        }

        // Moverse al hijo
        currentSVO_Origin.x += (childIndex & 1) ? half : 0.0f;
        currentSVO_Origin.y += (childIndex & 2) ? half : 0.0f;
        currentSVO_Origin.z += (childIndex & 4) ? half : 0.0f;
        currentSVO_Node = currentSVO_Node->GetChild(childIndex);
        currentSVO_Size = half;
    }

    // A este punto, 'currentSVO_Node' es el nodo SVO más específico encontrado para 'worldPos',
    // teniendo en cuenta el 'targetNodeSize' y la poda del SVO.

    // 3. Devolver la densidad basada en el estado de 'IsOccupied()' del nodo encontrado.
    //    Esto es la implementación "pura" que solicitaste, asumiendo que Marching Cubes
    //    funcionará con valores binarios 0.0f (sólido) y 1.0f (vacío) directamente.

    currentSVO_Node->GetDensity();
    //if (currentSVO_Node->IsOccupied()) {
    //    return 0.0f; // Ocupado (sólido)
    //}
    //else {
    //    return 1.0f; // No ocupado (vacío)
    //}
}

float World::CubeSDF(const DirectX::XMFLOAT3& point, const DirectX::XMFLOAT3& boxMin, const DirectX::XMFLOAT3& boxMax) {
    // Calcular el centro y el half-size del cubo
    DirectX::XMFLOAT3 center = { (boxMin.x + boxMax.x) / 2.0f, (boxMin.y + boxMax.y) / 2.0f, (boxMin.z + boxMax.z) / 2.0f };
    DirectX::XMFLOAT3 halfSize = { (boxMax.x - boxMin.x) / 2.0f, (boxMax.y - boxMin.y) / 2.0f, (boxMax.z - boxMin.z) / 2.0f };

    // Vector del punto al centro del cubo
    DirectX::XMFLOAT3 p_relative_to_center = { point.x - center.x, point.y - center.y, point.z - center.z };

    // Calcula la distancia desde el punto al bounding box en cada eje
    // Esto es el "q" en la fórmula de SDF de cubo de Inigo Quilez (min(0, dist) para el interior, max(0, dist) para el exterior)
    DirectX::XMFLOAT3 q = { std::abs(p_relative_to_center.x) - halfSize.x,
                            std::abs(p_relative_to_center.y) - halfSize.y,
                            std::abs(p_relative_to_center.z) - halfSize.z };

    // La distancia al exterior del cubo (si el punto está fuera)
    float outer_dist = std::sqrt((std::max)(0.0f, q.x) * (std::max)(0.0f, q.x) +
        (std::max)(0.0f, q.y) * (std::max)(0.0f, q.y) +
        (std::max)(0.0f, q.z) * (std::max)(0.0f, q.z));

    // La distancia al interior del cubo (si el punto está dentro, esta será negativa)
    float inner_dist = (std::min)(0.0f, (std::max)({ q.x, q.y, q.z })); // std::max con un initializer list

    // El SDF final es la suma de estas distancias
    //return outer_dist + inner_dist;

    const float sdf = outer_dist + inner_dist;
    const float smoothnessRadius = 2.0f;

    const float density = std::clamp(0.5f + (sdf / smoothnessRadius), 0.0f, 1.0f);    
    return density;
}

float World::SphereSDF(const DirectX::XMFLOAT3& point, const DirectX::XMFLOAT3& center, const float radio) {
    float dx = point.x - center.x;
    float dy = point.y - center.y;
    float dz = point.z - center.z;
    float distance = std::sqrt(dx * dx + dy * dy + dz * dz);
    const float sdf = distance - radio;
    // El SDF final es la suma de estas distancias
    //return outer_dist + inner_dist;
    const float smoothnessRadius = 2.0f;

    const float density = std::clamp(0.5f + (sdf / smoothnessRadius), 0.0f, 1.0f);
    return density;
}
