#include "VoxelDebug.h"
#include <unordered_map>

VoxelDebug::VoxelDebug(Material* material) : m_material(material) {}

VoxelDebug::~VoxelDebug() {
    Release();
}

HRESULT VoxelDebug::Init(ID3D11Device* device) {
    // No necesitamos inicializar nada aquí, ya que UIBox se inicializa cuando se necesita.
    return S_OK;
}

void VoxelDebug::Render(ID3D11DeviceContext* context, DeviceManager* deviceManager, WorldMatrixManager* worldMatrixManager, World* world) {
    std::unordered_map<ChunkKey, Chunk*>& chunks = world->GetChunkRenderer()->GetChunkManager()->GetChunks();

    // Define las dimensiones de un chunk una vez
    const float CHUNK_DIM_X = static_cast<float>(Chunk::CHUNK_SIZE_X);
    const float CHUNK_DIM_Y = static_cast<float>(Chunk::CHUNK_SIZE_Y);
    const float CHUNK_DIM_Z = static_cast<float>(Chunk::CHUNK_SIZE_Z);

    // El color de las cajas de debug
    DirectX::XMFLOAT4 debugBoxColor = { 1.0f, 0.0f, 1.0f, 1.0f }; // Magenta, por ejemplo

    // Contenedor para almacenar vértices únicos
    std::unordered_map<ChunkKey, UIBox*> uniqueDebugBoxes;

    for (const auto& chunkPair : chunks) {
        const ChunkKey& chunkKey = chunkPair.first;
        Chunk* chunk = chunkPair.second;

        // Calcular el origen (esquina inferior, frontal, izquierda) del chunk en el espacio mundial
        DirectX::XMFLOAT3 chunkOrigin = {
            static_cast<float>(chunkKey.x * CHUNK_DIM_X),
            static_cast<float>(chunkKey.y * CHUNK_DIM_Y),
            static_cast<float>(chunkKey.z * CHUNK_DIM_Z)
        };

        if (chunk->IsEmpty()) {
            continue;
            //debugBoxColor = { 0.0f, 1.0f, 0.0f, 1.0f }; // Verde para chunks vacíos
        }

        if (!chunk->IsEmpty()) {
            debugBoxColor = { 1.0f, 0.0f, 1.0f, 1.0f }; // Magenta, por ejemplo
        }

        // Obtener o crear el UIBox para este chunk
        UIBox* debugBox = GetOrCreateDebugBox(chunkKey, deviceManager->GetDevice(), chunkOrigin, CHUNK_DIM_X, CHUNK_DIM_Y, CHUNK_DIM_Z, debugBoxColor);
        if (!debugBox) continue; // Si falla la creación, saltar este chunk

        // Almacenar el UIBox en el contenedor de vértices únicos
        uniqueDebugBoxes[chunkKey] = debugBox;

        // Renderizar la caja de debug
        debugBox->Render(context);
    }
}

void VoxelDebug::Release() {
    for (auto& boxPair : m_debugBoxes) {
        boxPair.second->Release();
        delete boxPair.second;
    }
    m_debugBoxes.clear();
}



UIBox* VoxelDebug::GetOrCreateDebugBox(const ChunkKey& key, ID3D11Device* device,
    const DirectX::XMFLOAT3& origin, float sizeX, float sizeY, float sizeZ,
    const DirectX::XMFLOAT4& color) {
    auto it = m_debugBoxes.find(key);
    if (it != m_debugBoxes.end()) {
        return it->second;
    }

    // Crear un nuevo UIBox con los parámetros del chunk
    UIBox* newBox = new UIBox(m_material, origin, sizeX, sizeY, sizeZ, color);
    if (FAILED(newBox->Init(device))) {
        delete newBox;
        return nullptr; // Manejar el error de inicialización
    }
    m_debugBoxes[key] = newBox;
    return newBox;
}
XMFLOAT3 VoxelDebug::GetBox0Origin() { 
    if (m_debugBoxes.empty()) {
        return { -999.0f, -999.0f, -999.0f }; // Retornar un origen por defecto si no hay cajas
	}
    return m_debugBoxes.begin()->second->GetOrigin(); 
}
