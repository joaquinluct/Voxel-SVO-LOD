#include "AggregatedMeshChunk.h"
#include <algorithm>
#include <Windows.h>
#include <memory>
#include <Util/RayTracing/RayTracing.h>
#include <ICamera.h>

AggregatedMeshChunk::AggregatedMeshChunk(TerrainChunk::ChunkID id, float chunkSize)
    : m_id(id), m_chunkSize(chunkSize), m_geometryEngine(nullptr) {
    
    // Inicializar bounding box básico basado en chunk position y size
    float halfSize = chunkSize * 0.5f;
    DirectX::XMFLOAT3 chunkCenter = {
        static_cast<float>(id.x * chunkSize) + halfSize,
        0.0f, // TODO: Calcular altura apropiada
        static_cast<float>(id.z * chunkSize) + halfSize
    };
    
    m_boundingBox.min = DirectX::XMFLOAT3{chunkCenter.x - halfSize, chunkCenter.y - halfSize, chunkCenter.z - halfSize};
    m_boundingBox.max = DirectX::XMFLOAT3{chunkCenter.x + halfSize, chunkCenter.y + halfSize, chunkCenter.z + halfSize};
}

HRESULT AggregatedMeshChunk::Init() {
    // ... (Tu lógica de Init de Chunk) ...
    return S_OK;
}

void AggregatedMeshChunk::UpdateLOD(float distance, int maxLODs) {
    // 1. Definir los umbrales de distancia (configuración del TerrainConfig)
    // Esto debería venir de los 'geometry_lod_thresholds' de tu YAML.
    // Usaremos un ejemplo de umbral fijo:

    int newLOD = 0;
    if (distance > 500.0f) newLOD = 1;
    if (distance > 1000.0f) newLOD = 2;
    if (distance > 2000.0f) newLOD = 3;

    newLOD = (std::min)(newLOD, maxLODs);

    // 2. Comprobar si se requiere reconstrucción
    if (newLOD != m_currentLOD) {
        m_currentLOD = newLOD;
        m_dirty = true; // Se requiere que el Budget System lo regenere
    }
    else {
        m_dirty = false;
    }
}

void AggregatedMeshChunk::GenerateMesh(class TerrainGeometryFactory* factory) {
    // El motor de geometría DEBE estar inyectado y el chunk DEBE estar dirty.
    if (!m_geometryEngine || !m_dirty) return;

    m_meshData = std::make_unique<TerrainChunkMeshData>();

    // **DELEGACIÓN CRUCIAL:** Se llama al motor de geometría para que 
    // genere la data del chunk en el LOD actual y lo guarde en m_meshData.

    /*
    m_geometryEngine->GenerateChunkMeshData(
        m_meshData->vertices,
        m_meshData->indices,
        m_id,
        m_chunkSize,
        m_currentLOD
    );
    */

    // Lógica posterior a la generación (cálculo de normales, etc.)
    // ...

    m_dirty = false;
}

// Implementaciones de IChunk faltantes
DirectX::XMFLOAT3 AggregatedMeshChunk::GetCenter() const {
    // Calculate center based on chunk position and size
    float halfSize = m_chunkSize * 0.5f;
    return DirectX::XMFLOAT3{
        static_cast<float>(m_id.x * m_chunkSize) + halfSize,
        0.0f, // TODO: Calcular altura apropiada basada en terrain data
        static_cast<float>(m_id.z * m_chunkSize) + halfSize
    };
}

const Util::BoundingBox& AggregatedMeshChunk::GetBoundingBox() const {
    // TODO: Implement proper bounding box calculation based on chunk data
    // Por ahora retornar bounding box básico basado en chunk size
    return m_boundingBox;
}

bool AggregatedMeshChunk::IsInFrustum(const std::vector<CameraDefinition::FrustumPlane>& frustumPlanes) const {
    // TODO: Implement proper frustum culling
    // For now, always return true (visible)
    return true;
}

float AggregatedMeshChunk::GetDistanceToCamera(std::shared_ptr<ICamera> camera) const {
    if (!camera) {
        return 9999.0f; // Muy lejos si no hay cámara
    }
    
    DirectX::XMFLOAT3 chunkCenter = GetCenter();
    DirectX::XMFLOAT3 cameraPos = camera->GetPosition();
    
    // Calcular distancia euclidiana
    float dx = chunkCenter.x - cameraPos.x;
    float dy = chunkCenter.y - cameraPos.y;
    float dz = chunkCenter.z - cameraPos.z;
    
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}
