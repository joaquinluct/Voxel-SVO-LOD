#include "AggregatedGeometryEngine.h"
#include <Windows.h>
#include <sstream>

void AggregatedGeometryEngine::Init(TerrainConfig* config) {
    m_config = config;
    OutputDebugStringA("[AggregatedGeometryEngine] Init called\n");
}

void AggregatedGeometryEngine::Draw() {
    OutputDebugStringA("[AggregatedGeometryEngine] Draw called\n");
}

void AggregatedGeometryEngine::GenerateMesh(
    std::vector<VertexDefinition::TextureMapVertex>& globalVertexPool,
    std::unordered_map<size_t, UINT>& vertexLookup,
    std::vector<UINT>& localIndices,
    const TerrainChunk::ChunkID& chunkId,
    int zone) {

    // Para geometry aggregated, generamos una malla más densa que tessellation
    // porque no tenemos hardware tessellation disponible

        // Calcular posición del chunk en el mundo
    float chunkSize = 32.0f; // Tamaño default del chunk
    // TODO: Obtener chunk_size de la configuración correcta cuando esté disponible

    // Resolución de la malla (número de subdivisiones por lado)
    int resolution = 16; // Default: 16x16 grid
    if (m_config && m_config->geometry_resolution > 0) {
        resolution = m_config->geometry_resolution;
    }

    float worldX = chunkId.x * chunkSize;
    float worldZ = chunkId.z * chunkSize;
    float stepSize = chunkSize / static_cast<float>(resolution);

    // Generar vértices en grid
    UINT baseIndex = static_cast<UINT>(globalVertexPool.size());
    int vertexCount = (resolution + 1) * (resolution + 1);

    for (int z = 0; z <= resolution; ++z) {
        for (int x = 0; x <= resolution; ++x) {
            VertexDefinition::TextureMapVertex vertex;
            vertex.position[0] = worldX + (x * stepSize);
            vertex.position[1] = 0.0f; // Altura plana por ahora
            vertex.position[2] = worldZ + (z * stepSize);

            vertex.normal[0] = 0.0f;
            vertex.normal[1] = 1.0f;
            vertex.normal[2] = 0.0f;

            vertex.texCoord[0] = static_cast<float>(x) / static_cast<float>(resolution);
            vertex.texCoord[1] = static_cast<float>(z) / static_cast<float>(resolution);

            // Inicializar tangent y debugColor con valores por defecto
            vertex.tangent[0] = 1.0f; vertex.tangent[1] = 0.0f; vertex.tangent[2] = 0.0f;
            vertex.debugColor[0] = 1.0f; vertex.debugColor[1] = 1.0f; vertex.debugColor[2] = 1.0f; vertex.debugColor[3] = 1.0f;

            globalVertexPool.push_back(vertex);
        }
    }

    // Generar índices para formar triángulos
    for (int z = 0; z < resolution; ++z) {
        for (int x = 0; x < resolution; ++x) {
            // Calcular índices de los 4 vértices del quad actual
            UINT bottomLeft = baseIndex + (z * (resolution + 1)) + x;
            UINT bottomRight = bottomLeft + 1;
            UINT topLeft = baseIndex + ((z + 1) * (resolution + 1)) + x;
            UINT topRight = topLeft + 1;

            // Primer triángulo (bottomLeft, topLeft, bottomRight)
            localIndices.push_back(bottomLeft);
            localIndices.push_back(topLeft);
            localIndices.push_back(bottomRight);

            // Segundo triángulo (bottomRight, topLeft, topRight)
            localIndices.push_back(bottomRight);
            localIndices.push_back(topLeft);
            localIndices.push_back(topRight);
        }
    }

    std::ostringstream ss;
    ss << "[AggregatedGeometryEngine] Generated mesh for chunk (" << chunkId.x << ", " << chunkId.z
        << ") with " << vertexCount << " vertices and " << (resolution * resolution * 6) << " indices (resolution=" << resolution << ")\n";
    OutputDebugStringA(ss.str().c_str());
}

std::string AggregatedGeometryEngine::GetMeshAssetName() const {
    // Para aggregated geometry, usamos un asset de malla estándar
    return "TerrainAggregatedMesh";
}

void AggregatedGeometryEngine::RenderChunks(const std::vector<TerrainChunk::ChunkID>& chunkIds, const DirectX::XMFLOAT3& cameraPos) {
    if (chunkIds.empty()) {
        return;
    }

    // Para AggregatedGeometryEngine, cada chunk es un drawcall separado
    // porque no usa tessellation sino mallas densas pre-generadas

    OutputDebugStringA(("[AggregatedGeometryEngine] Rendering " +
        std::to_string(chunkIds.size()) + " chunks with aggregated meshes\n").c_str());

    float chunkSize = 32.0f; // TODO: Obtener de configuración

    for (const auto& chunkId : chunkIds) {
        // Calcular distancia para LOD
        float worldX = chunkId.x * chunkSize;
        float worldZ = chunkId.z * chunkSize;
        float chunkCenterX = worldX + chunkSize * 0.5f;
        float chunkCenterZ = worldZ + chunkSize * 0.5f;

        float dx = cameraPos.x - chunkCenterX;
        float dz = cameraPos.z - chunkCenterZ;
        float distance = std::sqrt(dx * dx + dz * dz);

        // Determinar LOD basado en distancia
        int lod = 0;
        if (distance > 256.0f) lod = 3;
        else if (distance > 128.0f) lod = 2;
        else if (distance > 64.0f) lod = 1;

        // TODO: Obtener el mesh del chunk desde el chunk factory
        // TODO: Configurar matrices de transformación
        // TODO: Ejecutar el drawcall específico para este chunk y LOD

        // Por ahora, solo logging para verificar el flujo
        OutputDebugStringA(("[AggregatedGeometryEngine] Chunk (" +
            std::to_string(chunkId.x) + "," + std::to_string(chunkId.z) +
            ") at distance " + std::to_string(distance) + " with LOD " + std::to_string(lod) + "\n").c_str());
    }
}

// Global instance required by Terrain.cpp
AggregatedGeometryEngine s_aggEngine;
