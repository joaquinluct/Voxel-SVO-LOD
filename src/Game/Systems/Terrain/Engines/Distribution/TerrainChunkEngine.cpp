#include "TerrainChunkEngine.h"
#include <cmath>
#include <DirectXMath.h>
#include <Engine/Core/Config/Game/TerrainEngines/TerrainChunkEngineConfig.h>
#include <exception>
#include <Game/System/TerrainConfig.h>
#include <Game/Systems/Terrain/Engines/Geometry/AggregatedGeometryEngine.h>
#include <Game/Systems/Terrain/Engines/Geometry/TessellationGeometryEngine.h>
#include <Game/Systems/Terrain/Factory/DefaultChunkFactory.h>
#include <iCamera.h>
#include <IChunk.h>
#include <Locators/ConfigLocator/ConfigLocator.h>
#include <Locators/Registers/REGISTER_TERRAIN_ENGINE_MACRO.h>
#include <Windows.h>

using namespace DirectX;

// Registrar el engine para que pueda ser resuelto por nombre
REGISTER_TERRAIN_ENGINE_TYPE(TerrainChunkEngine, "TerrainChunkEngine")

TerrainChunkEngine::TerrainChunkEngine() {
    OutputDebugStringA("[TerrainChunkEngine] Constructor called\n");
}

TerrainChunkEngine::~TerrainChunkEngine() {
    OutputDebugStringA("[TerrainChunkEngine] Destructor called\n");

    // Limpiar chunks activos
    m_activeChunks.clear();
    m_chunksToDelete.clear();
}

void TerrainChunkEngine::Init(TerrainConfig* config) {
    if (!config) {
        OutputDebugStringA("[TerrainChunkEngine] ERROR: No TerrainConfig provided\n");
        return;
    }

    m_terrainConfig = config;
    OutputDebugStringA("[TerrainChunkEngine] Initializing with TerrainConfig\n");

    // Cargar configuración específica del engine desde YAML
    try {
        m_engineConfig = ConfigLocator::GetConfig<TerrainChunkEngineConfig>().get();
        if (m_engineConfig) {
            OutputDebugStringA("[TerrainChunkEngine] TerrainChunkEngineConfig loaded successfully\n");
            InitializeFromConfig();
        }
        else {
            OutputDebugStringA("[TerrainChunkEngine] WARNING: TerrainChunkEngineConfig not found, using defaults\n");
        }
    }
    catch (const std::exception&) {
        OutputDebugStringA("[TerrainChunkEngine] WARNING: Failed to load TerrainChunkEngineConfig, using defaults\n");
    }

    // Inicializar chunk factory basado en configuración del terreno
    std::string chunkType = "DefaultChunk"; // Default fallback
    if (m_terrainConfig && !m_terrainConfig->terrain_engine.empty()) {
        // Extraer tipo de chunk de la configuración si está disponible
        chunkType = "DefaultChunk"; // Por ahora usar default, se puede extender en el futuro
    }
    LoadChunkFactory(chunkType);

    // Inicializar geometry engine basado en configuración del terreno  
    std::string geometryEngine = "AggregatedGeometryEngine"; // Default fallback
    if (m_terrainConfig && !m_terrainConfig->geometry_engine.empty()) {
        geometryEngine = m_terrainConfig->geometry_engine;
    }
    LoadGeometryEngine(geometryEngine);

    OutputDebugStringA("[TerrainChunkEngine] Initialization completed\n");
}

void TerrainChunkEngine::InitializeFromConfig() {
    if (!m_engineConfig) return;

    // Extraer parámetros del config YAML generado
    m_chunkSize = m_engineConfig->chunk_size;
    m_chunkRadius = m_engineConfig->chunk_radius;
    m_worldWidth = m_engineConfig->world_width;
    m_worldHeight = m_engineConfig->world_height;
    m_terrainHeightMult = m_engineConfig->terrain_height_mult;
    m_oceanLevel = m_engineConfig->ocean_level;
    m_seed = m_engineConfig->seed;

    char debugStr[512];
    sprintf_s(debugStr, "[TerrainChunkEngine] Config loaded - ChunkSize: %.1f, Radius: %d, Seed: %u\n",
        m_chunkSize, m_chunkRadius, m_seed);
    OutputDebugStringA(debugStr);
}

void TerrainChunkEngine::LoadChunkFactory(const std::string& chunkType) {
    OutputDebugStringA(("[TerrainChunkEngine] Loading chunk factory: " + chunkType + "\n").c_str());

    // TODO: Usar TerrainEngineLocator para resolver la factory por nombre cuando esté implementado
    // Por ahora usar factory por defecto
    m_chunkFactory = std::make_shared<DefaultChunkFactory>();

    if (m_chunkFactory) {
        OutputDebugStringA("[TerrainChunkEngine] Chunk factory loaded successfully\n");
    }
    else {
        OutputDebugStringA("[TerrainChunkEngine] ERROR: Failed to load chunk factory\n");
    }
}

void TerrainChunkEngine::LoadGeometryEngine(const std::string& geometryEngine) {
    OutputDebugStringA(("[TerrainChunkEngine] Loading geometry engine: " + geometryEngine + "\n").c_str());

    // TODO: Usar TerrainEngineLocator para resolver el engine por nombre cuando esté implementado
    // Por ahora usar referencias directas a las instancias globales
    extern TessellationGeometryEngine s_tessEngine;
    extern AggregatedGeometryEngine s_aggEngine;

    if (geometryEngine == "TessellationGeometryEngine") {
        m_geometryEngine = &s_tessEngine;
        m_geometryEngine->Init(m_terrainConfig);
        OutputDebugStringA("[TerrainChunkEngine] TessellationGeometryEngine assigned\n");
    }
    else if (geometryEngine == "AggregatedGeometryEngine") {
        m_geometryEngine = &s_aggEngine;
        m_geometryEngine->Init(m_terrainConfig);
        OutputDebugStringA("[TerrainChunkEngine] AggregatedGeometryEngine assigned\n");
    }
    else {
        // Fallback a AggregatedGeometryEngine
        m_geometryEngine = &s_aggEngine;
        m_geometryEngine->Init(m_terrainConfig);
        OutputDebugStringA("[TerrainChunkEngine] WARNING: Unknown geometry engine, using AggregatedGeometryEngine as fallback\n");
    }
}

std::vector<MeshAssetBase*> TerrainChunkEngine::GetVisibleMeshes(std::vector<CameraDefinition::FrustumPlane>& frustumPlanes, int zone) {
    std::vector<MeshAssetBase*> visibleMeshes;

    std::vector<TerrainChunk::ChunkID> visibleChunkIds = GetVisibleChunkIds(frustumPlanes, zone);

    // TODO: Convertir chunks visibles a mesh assets cuando la interfaz IChunk lo soporte
    // Por ahora retornar vacío, el renderizado se maneja directamente en Render()

    return visibleMeshes;
}

void TerrainChunkEngine::UpdateTerrain(const DirectX::XMFLOAT3& worldPosition) {
    // Verificar si la posición ha cambiado lo suficiente para requerir actualización
    float dx = worldPosition.x - m_lastPlayerPosition.x;
    float dz = worldPosition.z - m_lastPlayerPosition.z;
    float distanceMoved = std::sqrt(dx * dx + dz * dz);

    // Solo actualizar si se ha movido más de medio chunk
    if (distanceMoved > m_chunkSize * 0.5f) {
        m_lastPlayerPosition = worldPosition;
        m_isDirty = true;

        char debugStr[256];
        sprintf_s(debugStr, "[TerrainChunkEngine] Player moved %.1f units, updating terrain\n", distanceMoved);
        OutputDebugStringA(debugStr);
    }

    // Actualizar chunks si es necesario
    if (m_isDirty && !m_isUpdating) {
        m_isUpdating = true;

        LoadChunksInRadius(worldPosition);
        UnloadChunksOutsideRadius(worldPosition);
        CleanupDeletedChunks();

        m_isDirty = false;
        m_isUpdating = false;

        char debugStr[256];
        sprintf_s(debugStr, "[TerrainChunkEngine] Update completed. Active chunks: %zu\n", m_activeChunks.size());
        OutputDebugStringA(debugStr);
    }
}

void TerrainChunkEngine::Generate(ICamera* camera) {
    if (!camera || !m_geometryEngine) {
        return;
    }

    std::vector<CameraDefinition::FrustumPlane> frustumPlanes;
    camera->ExtractFrustumPlanes(frustumPlanes);

    // ? CORRECCIÓN: Trabajar directamente con m_activeChunks
    int chunksGenerated = 0;
    
    for (const auto& [id, chunk] : m_activeChunks) {
        if (!chunk) continue;

        // Frustum culling
        if (!IsChunkInFrustum(id, frustumPlanes)) continue;

        // TODO: Verificar si el chunk necesita regenerar geometría
        // TODO: Llamar al geometry engine para generar la malla del chunk
        chunksGenerated++;
    }

    if (chunksGenerated > 0) {
        char debugStr[256];
        sprintf_s(debugStr, "[TerrainChunkEngine] Generated geometry for %d chunks\n", chunksGenerated);
        OutputDebugStringA(debugStr);
    }
}

void TerrainChunkEngine::Render(ICamera* camera) {
    if (!camera || !m_geometryEngine) {
        return;
    }

    std::vector<CameraDefinition::FrustumPlane> frustumPlanes;
    camera->ExtractFrustumPlanes(frustumPlanes);

    // ? CORRECCIÓN: Trabajar directamente con m_activeChunks en lugar de duplicar
    // Recopilar IDs de chunks visibles directamente
    std::vector<TerrainChunk::ChunkID> chunkIds;

    for (const auto& [id, chunk] : m_activeChunks) {
        if (!chunk) continue;

        // Frustum culling
        if (!IsChunkInFrustum(id, frustumPlanes)) continue;

        // TODO: Verificar zona (para ring buffering)
        // TODO: Verificar si el chunk tiene buffers listos

        chunkIds.push_back(id);
    }

    if (chunkIds.empty()) {
        return;
    }

    DirectX::XMFLOAT3 cameraPos = camera->GetPosition();

    // Delegar renderizado al geometry engine específico
    if (auto tessEngine = dynamic_cast<TessellationGeometryEngine*>(m_geometryEngine)) {
        tessEngine->RenderChunks(chunkIds, cameraPos);
    }
    else if (auto aggEngine = dynamic_cast<AggregatedGeometryEngine*>(m_geometryEngine)) {
        aggEngine->RenderChunks(chunkIds, cameraPos);
    }
    else {
        // Fallback al método Draw() base
        m_geometryEngine->Draw();
    }

    char debugStr[256];
    sprintf_s(debugStr, "[TerrainChunkEngine] Rendered %zu chunks\n", chunkIds.size());
    OutputDebugStringA(debugStr);
}

void TerrainChunkEngine::LoadChunksInRadius(const DirectX::XMFLOAT3& centerPosition) {
    TerrainChunk::ChunkID centerChunk = WorldPositionToChunkID(centerPosition);

    int chunksLoaded = 0;
    for (int x = centerChunk.x - m_chunkRadius; x <= centerChunk.x + m_chunkRadius; ++x) {
        for (int z = centerChunk.z - m_chunkRadius; z <= centerChunk.z + m_chunkRadius; ++z) {
            TerrainChunk::ChunkID chunkId = { x, 0, z };

            // Verificar si el chunk ya está cargado
            if (m_activeChunks.count(chunkId) == 0) {
                if (SUCCEEDED(LoadSingleChunk(chunkId))) {
                    chunksLoaded++;
                }
            }
        }
    }

    if (chunksLoaded > 0) {
        char debugStr[256];
        sprintf_s(debugStr, "[TerrainChunkEngine] Loaded %d new chunks\n", chunksLoaded);
        OutputDebugStringA(debugStr);
    }
}

void TerrainChunkEngine::UnloadChunksOutsideRadius(const DirectX::XMFLOAT3& centerPosition) {
    std::vector<TerrainChunk::ChunkID> chunksToUnload;

    for (const auto& [id, chunk] : m_activeChunks) {
        float distance = CalculateDistanceToChunk(id, centerPosition);
        float maxDistance = m_chunkRadius * m_chunkSize;

        if (distance > maxDistance) {
            chunksToUnload.push_back(id);
        }
    }

    // Mover chunks a la cola de eliminación
    for (const TerrainChunk::ChunkID& id : chunksToUnload) {
        auto it = m_activeChunks.find(id);
        if (it != m_activeChunks.end()) {
            m_chunksToDelete.push_back(std::move(it->second));
            m_activeChunks.erase(it);
        }
    }

    if (!chunksToUnload.empty()) {
        char debugStr[256];
        sprintf_s(debugStr, "[TerrainChunkEngine] Queued %zu chunks for unloading\n", chunksToUnload.size());
        OutputDebugStringA(debugStr);
    }
}

HRESULT TerrainChunkEngine::LoadSingleChunk(const TerrainChunk::ChunkID& id) {
    if (!m_chunkFactory) {
        return E_FAIL;
    }

    // Crear chunk usando la factory
    auto chunk = m_chunkFactory->CreateChunk(id, m_chunkSize);
    if (!chunk) {
        return E_FAIL;
    }

    // TODO: Configurar chunk con procedural engine y otros parámetros
    // TODO: Inicializar el chunk

    // Añadir a chunks activos
    m_activeChunks[id] = std::move(chunk);

    return S_OK;
}

std::vector<TerrainChunk::ChunkID> TerrainChunkEngine::GetVisibleChunkIds(const std::vector<CameraDefinition::FrustumPlane>& frustumPlanes, int zone) {
    std::vector<TerrainChunk::ChunkID> visibleChunkIds;

    for (const auto& [id, chunk] : m_activeChunks) {
        if (!chunk) continue;

        // Frustum culling
        if (!IsChunkInFrustum(id, frustumPlanes)) continue;

        // TODO: Verificar zona (para ring buffering)
        // TODO: Verificar si el chunk tiene buffers listos

        visibleChunkIds.push_back(id);
    }

    return visibleChunkIds;
}

bool TerrainChunkEngine::IsChunkInFrustum(const TerrainChunk::ChunkID& id, const std::vector<CameraDefinition::FrustumPlane>& frustumPlanes) const {
    // Calcular bounding box del chunk
    DirectX::XMFLOAT3 chunkPos = GetChunkWorldPosition(id);
    DirectX::XMFLOAT3 chunkMin = { chunkPos.x, m_oceanLevel, chunkPos.z };
    DirectX::XMFLOAT3 chunkMax = { chunkPos.x + m_chunkSize, m_terrainHeightMult * 100.0f, chunkPos.z + m_chunkSize };

    // Test contra cada plano del frustum
    for (const auto& plane : frustumPlanes) {
        // TODO: Implementar test de intersección AABB vs plano
        // Por ahora, asumir que todos los chunks están visibles
    }

    return true; // Temporal - asumir todos visibles
}

float TerrainChunkEngine::CalculateDistanceToChunk(const TerrainChunk::ChunkID& id, const DirectX::XMFLOAT3& position) const {
    DirectX::XMFLOAT3 chunkCenter = GetChunkWorldPosition(id);
    chunkCenter.x += m_chunkSize * 0.5f;
    chunkCenter.z += m_chunkSize * 0.5f;

    float dx = position.x - chunkCenter.x;
    float dz = position.z - chunkCenter.z;

    return std::sqrt(dx * dx + dz * dz);
}

DirectX::XMFLOAT3 TerrainChunkEngine::GetChunkWorldPosition(const TerrainChunk::ChunkID& id) const {
    return {
        static_cast<float>(id.x) * m_chunkSize,
        0.0f,
        static_cast<float>(id.z) * m_chunkSize
    };
}

TerrainChunk::ChunkID TerrainChunkEngine::WorldPositionToChunkID(const DirectX::XMFLOAT3& worldPos) const {
    return {
        static_cast<int>(std::floor(worldPos.x / m_chunkSize)),
        0,
        static_cast<int>(std::floor(worldPos.z / m_chunkSize))
    };
}

void TerrainChunkEngine::CleanupDeletedChunks() {
    if (!m_chunksToDelete.empty()) {
        char debugStr[256];
        sprintf_s(debugStr, "[TerrainChunkEngine] Cleaning up %zu deleted chunks\n", m_chunksToDelete.size());
        OutputDebugStringA(debugStr);

        m_chunksToDelete.clear();
    }
}
