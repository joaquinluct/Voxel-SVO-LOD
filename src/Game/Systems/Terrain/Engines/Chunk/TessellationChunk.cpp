#include "TessellationChunk.h"
#include <algorithm>
#include <cstdlib>
#include <Defines/TerrainChunk.h>
#include <Util/RayTracing/RayTracing.h>

TessellationChunk::TessellationChunk(TerrainChunk::ChunkID id, float chunkSize)
: m_id(id), m_chunkSize(chunkSize) { // m_boundingBox() removed
}

HRESULT TessellationChunk::Init() {
    // Inicializar el Bounding Box basado en ID y Size
    // ... (Tu lógica de Init de Chunk) ...
    return S_OK;
}

void TessellationChunk::UpdateLOD(float distance, int maxLODs) {
    // 1. Definir los límites del factor de teselación.
    const float MAX_FACTOR = 64.0f; // Máxima subdivisión (cerca)
    const float MIN_FACTOR = 4.0f;  // Mínima subdivisión (lejos)

    // Asumimos que la máxima distancia de render es 'maxLODs' * 'm_chunkSize'
    float maxRenderDistance = m_chunkSize * maxLODs;

    // 2. Normalizar la distancia
    float normalizedDistance = (std::min)(1.0f, distance / maxRenderDistance);

    // 3. Mapeo: 0.0 (cerca) -> MAX_FACTOR, 1.0 (lejos) -> MIN_FACTOR
    float newLodFactor = MIN_FACTOR + (MAX_FACTOR - MIN_FACTOR) * (1.0f - normalizedDistance);

    // 4. Chequear si el cambio es significativo para marcar como dirty
    if (std::abs(newLodFactor - m_currentLodFactor) > 0.5f) {
        m_currentLodFactor = newLodFactor;
        m_dirty = true; // El Instancing Buffer necesita ser actualizado
    }
    else {
        m_dirty = false;
    }
}

TessellationInstanceData TessellationChunk::GetInstanceData() const {
    TessellationInstanceData data{};

    // 1. Matriz World: Traslada el patch base a la posición del chunk
    DirectX::XMFLOAT3 center = GetCenter();
    DirectX::XMMATRIX world = DirectX::XMMatrixTranslation(center.x, 0.0f, center.z);
    DirectX::XMStoreFloat4x4(&data.WorldMatrix, world);

    // 2. Factor de LOD
    data.LodFactor = m_currentLodFactor;

    return data;
}

// Implementaciones de IChunk faltantes
const Util::BoundingBox& TessellationChunk::GetBoundingBox() const {
    // TODO: Implement proper bounding box calculation based on chunk data
    return m_boundingBox;
}

bool TessellationChunk::IsInFrustum(const std::vector<CameraDefinition::FrustumPlane>& frustumPlanes) const {
    // TODO: Implement proper frustum culling using bounding box
    // For now, always return true (visible)
    return true;
}

float TessellationChunk::GetDistanceToCamera(std::shared_ptr<ICamera> camera) const {
    // TODO: Calculate distance to camera properly using chunk center and camera position
    // For now, return a default distance
    return 100.0f;
}
