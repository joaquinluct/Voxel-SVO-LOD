#include "World.h"
#include "FrustumCullingHelper.h"
#include "../SVOFile/SVO_FileManager.h"
#include <cmath>
#include <functional>
#include <algorithm>
#include "../Voxel/VoxelData.h"
#include "../MarchingCubes/MarchingCubes.h"
#include "../Voxel/VoxelMesh.h"

World::World(DeviceManager* deviceManager, FirstPersonCamera* camera)
    : m_deviceManager(deviceManager), m_camera(camera), m_material(nullptr), m_terrainGenerator(nullptr), m_lastPosition{} {
    // Configuración básica de LOD
    m_lodSettings.nearDistance = 2000.0f;
    m_lodSettings.midDistance = 3000.0f;
    m_lodSettings.farDistance = 4000.0f;
	m_mainOriginRender = { 0, 0, 0 };
    m_mainOriginUpdate = { 0, 0, 0 };
	m_depth = 1; // Profundidad inicial
    m_terrainGenerator = new TerrainGeneratorPerling1();
}

World::~World() {
    // Limpieza automática por unique_ptr
}

HRESULT World::Init(Material* material) {
    m_material = material;
    // Aquí podrías cargar recursos adicionales si es necesario    
    //HRESULT hr = m_terrainGenerator->Init("AwAAAIA/");
    HRESULT hr = m_terrainGenerator->Init("EAAAAAA/AwAAAIA/AAAAgD8=");
    XMFLOAT3 pos{ 100.0f,100.0f,100.0f };
    XMFLOAT3 posEnd{pos.x+100.0f, pos.y + 100.0f, pos.z + 100.0f };
    m_terrainGenerator->PrepareNoiseOutput(pos, posEnd);
    return hr;
}

void World::Release() {
    // Liberar recursos si es necesario
    SafeDelete(m_terrainGenerator);
	m_material = nullptr; // Dejar que el material se libere automáticamente
}

// --- Miembros para double buffering del mesh ---
MarchingCubesMesh m_mainMeshUpdate;
MarchingCubesMesh m_mainMeshRender;
DirectX::XMFLOAT3 m_mainOriginUpdate;
DirectX::XMFLOAT3 m_mainOriginRender;

void World::Update(float deltaTime) {

    DirectX::XMFLOAT3 camPos = m_camera->GetPosition();
 //   if (DirectXUtils::AreEqual(m_lastPosition, camPos)) {
 //       return; // No ha cambiado la posición de la cámara, no hay nada que actualizar
	//}
    m_lastPosition = camPos;
    std::lock_guard<std::mutex> lock(worldMutex);
    m_visibleNodesUpdate.clear();
    m_mainMeshUpdate = MarchingCubesMesh(); // Limpiar el mesh de update
    m_mainOriginUpdate = {0,0,0};
    AreaKey areaKey = GetAreaKeyFromPosition(camPos);
    float farPlane = m_camera->GetFarPlane();
    constexpr int MAX_AREA_RADIUS = 30;
    int radio = static_cast<int>(std::ceil(farPlane / AREA_SIZE));
    radio = std::clamp(radio, 1, MAX_AREA_RADIUS);
    VoxelData voxelData;
    MarchingCubes marchingCubes;
    bool isMainOriginSet = false;
    
    // Extraer los planos del frustum para culling
    /*XMFLOAT4 frustumPlanes[6];
    m_camera->ExtractFrustumPlanes(frustumPlanes);*/

	/*const XMFLOAT3 pos{ -radio * AREA_SIZE_F, -radio * AREA_SIZE_F, -radio * AREA_SIZE_F };
    const XMFLOAT3 size{ (radio + 1) * AREA_SIZE_F * 2.0f, (radio + 1) * AREA_SIZE_F * 2.0f, (radio+1)*AREA_SIZE_F * 2.0f };*/
    //m_terrainGenerator->PrepareNoiseOutput(pos, size);
    
    for (int dx = -radio; dx <= radio; ++dx) {
        for (int dy = -radio; dy <= radio; ++dy) {
            for (int dz = -radio; dz <= radio; ++dz) {
                AreaKey key{areaKey.x + dx, areaKey.y + dy, areaKey.z + dz};
                
                // Calcular los límites del área para Frustum Culling
                DirectX::XMFLOAT3 areaOrigin = { 
                    static_cast<float>(key.x) * AREA_SIZE, 
                    static_cast<float>(key.y) * AREA_SIZE, 
                    static_cast<float>(key.z) * AREA_SIZE 
                };

                // Comprobar si el área está dentro del frustum antes de procesarla
                //DirectX::XMFLOAT3 areaMax = { 
                //    areaOrigin.x + AREA_SIZE,
                //    areaOrigin.y + AREA_SIZE,
                //    areaOrigin.z + AREA_SIZE
                //};                
                //if (!FrustumCullingHelper::IsAABBInFrustum(areaOrigin, areaMax, frustumPlanes)) {
                //    continue; // Área fuera del frustum, saltar al siguiente área
                //}
                
                // El área está dentro del frustum, procesarla normalmente
                SVO_Node* root = GetOrCreateArea(key);
                if (root && root->IsOccupied()) {
                    int level = 0;
                    LODProcessor::ProcessLOD(root, areaOrigin, camPos, AREA_SIZE, m_lodSettings, m_visibleNodesUpdate, level);
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
        MarchingCubesMesh* nodeMesh = info.m_marchingCubesMesh;
        // char buf[128];
        // sprintf_s(buf, "VISIBLE: origin=%.1f %.1f %.1f size=%.1f\n", info.origin.x, info.origin.y, info.origin.z, info.size);
        // OutputDebugStringA(buf);
        /*node->ClearLODTransitions();
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
        }*/
        // --- Generar mesh para cada nodo visible ---
        const DirectX::XMFLOAT3& origin = info.origin;
        if (!isMainOriginSet) {
            m_mainOriginUpdate = origin;
            isMainOriginSet = true;
        }
        const float size = info.size;
        std::array<bool, 6> lodTransitions{};
        //for (int face = 0; face < 6; ++face) lodTransitions[face] = node->GetLODTransition(face);
		//AreaKey key = GetAreaKeyFromPosition(origin);
		//MarchingCubesMesh* meshSaved = GetMesh(key);
  //      if (meshSaved) {
  //          // Si ya existe un mesh para este área, usarlo directamente
  //          m_mainMeshUpdate.addMesh(*meshSaved);
  //          continue; // No generar de nuevo
		//}
        if (nodeMesh != nullptr) {
            // Si el nodo ya tiene un mesh generado, usarlo directamente
            MarchingCubesMesh* existingMesh = nodeMesh;
            if (!existingMesh->vertices.empty() && !existingMesh->indices.empty()) {
                m_mainMeshUpdate.addMesh(*existingMesh);
                continue; // No generar de nuevo
            }
        }
        else {
            MarchingCubesMesh mesh = marchingCubes.GenerateMesh(node, origin, size, &voxelData, lodTransitions, this);
            if (!mesh.vertices.empty() && !mesh.indices.empty()) {
                m_mainMeshUpdate.addMesh(mesh);
                info.m_marchingCubesMesh = &mesh; // Guardar mesh generado en el nodo
                //m_meshes[key] = std::make_unique<MarchingCubesMesh>(std::move(mesh)); // Guardar mesh generado
            }
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

MarchingCubesMesh* World::GetMesh(const AreaKey& key) {
    auto it = m_meshes.find(key);
    if (it != m_meshes.end()) return it->second.get();
    return nullptr; // No existe el mesh
}

float World::GetDensityFromTerrainNoise(XMFLOAT3 cornerPos)
{
    // Obtener la altura del terreno en esta esquina
    float rawHeight = m_terrainGenerator->GetRawHeight(cornerPos.x, cornerPos.z);

    // Escalar y desplazar el valor de ruido para que esté en el rango [-10, 100]
    float terrainHeight = (rawHeight * 0.5f + 0.5f) * 1510.0f - 110.0f; // Mapear [-1, 1] a [-10, 100]

    // Calcular la densidad en unidades del mundo
    float rawDensity = terrainHeight - cornerPos.y;

    // Determinar un rango razonable para la diferencia de altura
    // Este valor depende de la escala de tu terreno y de la variabilidad del ruido.
    // Ajusta este valor según sea necesario.
    constexpr float MAX_HEIGHT_DIFFERENCE = 150.0f;

    // Normalizar la densidad al rango [-1.0, 1.0]
    float normalizedDensity = rawDensity / MAX_HEIGHT_DIFFERENCE;

    // Clampear la densidad normalizada al rango [-1.0, 1.0]
    normalizedDensity = std::clamp(normalizedDensity, -1.0f, 1.0f);

    return normalizedDensity;
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
            float cube[8]{};
			bool allInside = true;
			bool allOutside = true;
            for (int i = 0; i < 8; ++i) {
                XMFLOAT3 cornerPos = DirectXUtils::Add(pos, DirectXUtils::Multiply(MarchingCubesTables::cornerOffsetsX[i], size));

				float normalizedDensity = GetDensityFromTerrainNoise(cornerPos);

                cube[i] = normalizedDensity;

                if (normalizedDensity < (MarchingCubes::ISO_LEVEL - 1.2f)) {
                    allOutside = false; // Al menos uno está "dentro"
                }
                if (normalizedDensity > (MarchingCubes::ISO_LEVEL + 1.2f)) {
                    allInside = false; // Al menos uno está "fuera"
                }
            }

            NeightborsDensitiesNormal nDensity(pos);

            nDensity.xMax = GetDensityFromTerrainNoise(nDensity.posXMax);
			nDensity.yMax = GetDensityFromTerrainNoise(nDensity.posYMax);
            nDensity.zMax = GetDensityFromTerrainNoise(nDensity.posZMax);
            nDensity.xMin = GetDensityFromTerrainNoise(nDensity.posXMin);
            nDensity.yMin = GetDensityFromTerrainNoise(nDensity.posYMin);
            nDensity.zMin = GetDensityFromTerrainNoise(nDensity.posZMin);

            XMFLOAT3 normal = nDensity.GetNormal();

            n->SetNormal(normal);
            n->SetCubeDensity(cube);
            n->SetIsLeaf(true);
            n->SetDensity(cube[0]);
            n->SetOccupied(cube[0] < (MarchingCubes::ISO_LEVEL+1.2f));
            // Si todos están dentro o todos están fuera, marcar el nodo como no ocupado
            /*if (allInside || allOutside) {
                n->SetOccupied(false);
            }
            else {
                n->SetOccupied(true);
            }*/

            return;
        }
        n->Subdivide();
        bool anyChildOccupied = false;
        float childSize = size / 2.0f;
        float density = 1.0f;
        for (int i = 0; i < 8; ++i) {
            float cx = ox + ((i & 1) ? childSize : 0);
            float cy = oy + ((i & 2) ? childSize : 0);
            float cz = oz + ((i & 4) ? childSize : 0);
            SVO_Node* child = n->GetChild(i);
            fillNode(child, cx, cy, cz, childSize, depth + 1);
            density += n->GetDensity();
            if (child->IsOccupied()) anyChildOccupied = true;
        }
        //n->SetAreAllChildrenOccupied(n->AreAllChildrenOccupied());
        n->SetDensity(std::clamp(density/8.0f, 0.0f, 1.0f));
        n->SetOccupied(anyChildOccupied);
        };
    /*XMFLOAT3 pos = AreaKey::GetPositionFromAreaKey(key);
    XMFLOAT3 posEnd{pos.x+AREA_SIZE_F, pos.y + AREA_SIZE_F, pos.z };
    m_terrainGenerator->PrepareNoiseOutput(pos, posEnd, AREA_SIZE_F);*/
	float x = static_cast<float>(key.x) * AREA_SIZE_F;
	float y = static_cast<float>(key.y) * AREA_SIZE_F;
	float z = static_cast<float>(key.z) * AREA_SIZE_F;
    fillNode(node.get(), x, y, z, AREA_SIZE, 0);
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
    
    // Extraer los planos del frustum para culling
    XMFLOAT4 frustumPlanes[6];
    m_camera->ExtractFrustumPlanes(frustumPlanes);
    
    for (int dx = -radio; dx <= radio; ++dx) {
        for (int dz = -radio; dz <= radio; ++dz) {
            for (int dy = -radio; dy <= radio; ++dy) {
                AreaKey key{ areaKey.x + dx, areaKey.y + dy, areaKey.z + dz };
                
                // Calcular los límites del área para Frustum Culling
                DirectX::XMFLOAT3 areaOrigin = { 
                    static_cast<float>(key.x) * AREA_SIZE, 
                    static_cast<float>(key.y) * AREA_SIZE, 
                    static_cast<float>(key.z) * AREA_SIZE 
                };
                DirectX::XMFLOAT3 areaMax = { 
                    areaOrigin.x + AREA_SIZE,
                    areaOrigin.y + AREA_SIZE,
                    areaOrigin.z + AREA_SIZE
                };
                
                // Comprobar si el área está dentro del frustum antes de procesarla
                if (!FrustumCullingHelper::IsAABBInFrustum(areaOrigin, areaMax, frustumPlanes)) {
                    continue; // Área fuera del frustum, saltar al siguiente área
                }
                
                // El área está dentro del frustum, procesarla normalmente
                SVO_Node* root = GetOrCreateArea(key);
                if (root && root->IsOccupied()) {
                    int level = 0;
                    LODProcessor::ProcessLOD(root, areaOrigin, camPos, AREA_SIZE, m_lodSettings, m_visibleNodesUpdate, level);
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

//float World::GetVoxelDensity(const DirectX::XMFLOAT3& worldPos, const SVO_Node* nodeRef, const float nodeSize)
// 
// {
//    // 1. Obtener la clave del área (chunk) y el nodo raíz SVO para esa área.
//    AreaKey areaKey = GetAreaKeyFromPosition(worldPos);
//    SVO_Node* currentSVO_Node = GetOrCreateArea(areaKey); // Llama al helper que carga/crea
//
//    // Si el nodo raíz del SVO del chunk es nulo o no está ocupado, el chunk está vacío.
//    if (!currentSVO_Node) {
//        return 1.0f; // Vacío
//    }
//
//    DirectX::XMFLOAT3 currentSVO_Origin = worldPos;
//    float currentSVO_Size = AREA_SIZE_F;
//
//    // 2. Recorrer el SVO para encontrar el nodo más específico para 'worldPos'.
//    //    El recorrido se detendrá si:
//    //    - Se llega a una hoja del SVO.
//    //    - El tamaño del nodo actual es menor o igual al `targetNodeSize` solicitado.
//    //    - El punto cae en una región sin hijo o el hijo no está ocupado (poda).
//
//    while (currentSVO_Node && !currentSVO_Node->IsLeaf() && currentSVO_Size > 1.0f) {
//        // Calcular el índice del hijo donde debe estar worldPos
//        float half = currentSVO_Size / 2.0f;
//        int childIndex = 0;
//        if (worldPos.x >= currentSVO_Origin.x + half) childIndex |= 1;
//        if (worldPos.y >= currentSVO_Origin.y + half) childIndex |= 2;
//        if (worldPos.z >= currentSVO_Origin.z + half) childIndex |= 4;
//
//        // Condición de parada para el recorrido del SVO:
//        // A) Si el nodo actual no tiene el hijo que buscamos.
//        // B) Si el hijo existe pero no está ocupado (poda del SVO).
//        // C) Si el tamaño del siguiente nivel de subdivisión (half) ya es más pequeño
//        //    que el 'targetNodeSize' solicitado por Marching Cubes.
//        //    Usamos una pequeña tolerancia para comparaciones de floats.
//        if (!currentSVO_Node->HasChild(childIndex) ||
//            //!currentSVO_Node->GetChild(childIndex)->IsOccupied() ||
//            half < nodeSize - 0.001f) // Condición para detenerse en el LOD solicitado
//        {
//            break;
//        }
//
//        // Moverse al hijo
//        currentSVO_Origin.x += (childIndex & 1) ? half : 0.0f;
//        currentSVO_Origin.y += (childIndex & 2) ? half : 0.0f;
//        currentSVO_Origin.z += (childIndex & 4) ? half : 0.0f;
//        currentSVO_Node = currentSVO_Node->GetChild(childIndex);
//        currentSVO_Size = half;
//    }
//
//    if (!currentSVO_Node) {
//        return 1.0f; // Vacío
//    }
//    
//    return currentSVO_Node->GetDensity();
//}

float World::GetVoxelDensity(const DirectX::XMFLOAT3& worldPos, const SVO_Node* nodeRef, const float targetNodeSize) {

    // 1. Obtener la clave del área (chunk) y el nodo raíz SVO para esa área.
    AreaKey areaKey = GetAreaKeyFromPosition(worldPos);
    SVO_Node* currentSVO_Node = GetOrCreateArea(areaKey); // Llama al helper que carga/crea

    // Si el nodo raíz del SVO del chunk es nulo o no está ocupado, el chunk está vacío.
    if (!currentSVO_Node) {
        return 1.0f; // Vacío
    }

    DirectX::XMFLOAT3 currentSVO_Origin = areaKey.GetPosition();
    float currentSVO_Size = AREA_SIZE_F;

    while (currentSVO_Node && !currentSVO_Node->IsLeaf() && currentSVO_Size > 1.0f) { // Usar constante
        float half = currentSVO_Size / 2.0f;
        int childIndex = 0;
        if (worldPos.x >= currentSVO_Origin.x + half) childIndex |= 1;
        if (worldPos.y >= currentSVO_Origin.y + half) childIndex |= 2;
        if (worldPos.z >= currentSVO_Origin.z + half) childIndex |= 4;

        if (!currentSVO_Node->HasChild(childIndex) ||
            (currentSVO_Node->HasChild(childIndex) && !currentSVO_Node->GetChild(childIndex)->IsOccupied()) ||
            half < targetNodeSize - 0.001f)
        {
            if (currentSVO_Node->HasChild(childIndex)) {
                SVO_Node* node = currentSVO_Node->GetChild(childIndex);
                if (node->GetDensity() < 0.999999f) {
                    return 1.0f;
                }
            }
            return 1.0f;
        }

        currentSVO_Origin.x += (childIndex & 1) ? half : 0.0f;
        currentSVO_Origin.y += (childIndex & 2) ? half : 0.0f;
        currentSVO_Origin.z += (childIndex & 4) ? half : 0.0f;
        currentSVO_Node = currentSVO_Node->GetChild(childIndex);
        currentSVO_Size = half;
    }

    // Si llegamos aqu�, currentSVO_Node es una hoja o hemos alcanzado el nivel de detalle m�ximo.
    // Si el nodo es una HOJA y tiene precalculadas las 8 densidades, INTERPOLA.
    // Si no es una hoja (ej. nos detuvimos por targetNodeSize), o si la hoja solo tiene 1 densidad,
    // o si el nodo no tiene las 8 densidades (p. ej. es un nodo interno podado), usa la funci�n global.
    if (currentSVO_Node && currentSVO_Node->IsLeaf() && currentSVO_Node->HasPrecalculatedCornerDensities()) { 
        return MarchingCubesUtil::TrilinearInterpolate(worldPos, currentSVO_Origin, currentSVO_Size, currentSVO_Node->GetCube());
    }
    // Fallback si currentSVO_Node es nulo (no deber�a ocurrir con el primer 'if' si es correcto)
    return 1.0f;
    
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

float World::SphereSDF(const DirectX::XMFLOAT3& point, const float nodeSize, const DirectX::XMFLOAT3& center, const float radio) {
    float dx = point.x - center.x;
    float dy = point.y - center.y;
    float dz = point.z - center.z;    
    float distance = std::sqrt(dx * dx + dy * dy + dz * dz);
    const float sdf = distance - radio;
    // El SDF final es la suma de estas distancias    
    const float smoothnessRadius = 2.0f;

    float density = std::clamp(0.5f + (sdf / smoothnessRadius), 0.0f, 1.0f);

    /*if (density > 0.0000001f && density < 0.9999999f) {
        density = density;
    }
    else if (density < 0.000001f) {
        density = density;
    }*/

    return density;
}


