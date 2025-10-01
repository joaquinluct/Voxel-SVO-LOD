// TerrainAsset.cpp
#include "TerrainAsset.h"
#include <AssetLocator/AssetLocator.h>
#include <Assets/Base/MeshAsset.h>
#include <Assets/Base/MeshAssetBase.h>
#include <Assets/Base/TextureAsset.h>
#include <Core/Defines/Contants/Flags.h>
#include <cstdint>
#include <d3d11.h>
#include <Defines/VertexDefinition.h>
#include <ManagerLocator/ManagerLocator.h>
#include <MeshAssetConfigBase.h>
#include <REGISTER_ASSET_MACRO.h>
#include <TerrainAssetConfigBase.h>
#include <Util/Text/Text.h>
#include <vector>
#include <windows.h>
#include <wrl/client.h>

REGISTER_ASSET_TYPE(TerrainAsset, "TerrainAsset")

constexpr UINT MaxVertexCount = 100000;  // Ajusta según tu terreno
constexpr UINT MaxIndexCount = 50000;

TerrainAsset::TerrainAsset()
    : m_terrainConfig(nullptr), m_vertexTypeSize(0), m_allocator(MaxVertexCount, MaxIndexCount)
{
}
TerrainAsset::TerrainAsset(const TerrainAsset* other)
    : MeshAssetBase(other), m_terrainConfig(other->m_terrainConfig),
    m_textureTransforms(other->m_textureTransforms), m_vertexTypeSize(other->m_vertexTypeSize),
    m_textureAsset(other->m_textureAsset)
{
    // Copiar otros miembros según sea necesario
}

TerrainAsset::~TerrainAsset() {
    Shutdown();
}

void TerrainAsset::SetConfig(std::shared_ptr<ConfigBase> config) {
    m_terrainConfig = std::dynamic_pointer_cast<TerrainAssetConfigBase>(config);
    m_meshConfig = std::dynamic_pointer_cast<MeshAssetConfigBase>(config);
}

Mesh::DrawType TerrainAsset::GetDrawType() const {
    return Mesh::DrawType::DrawIndexed;
}

HRESULT TerrainAsset::Init() {
    if (!m_terrainConfig) {
        return S_OK;
    }
    m_meshConfig = std::make_shared<MeshAssetConfigBase>();
    m_meshConfig->name = m_terrainConfig->name;
    //SetConfig(m_terrainConfig);
    MeshAssetBase::Init();

    m_shaderName = StringToWstring(m_terrainConfig->shader);
    m_shaderAssetName = m_terrainConfig->shader;

    m_deviceManager = ManagerLocator::GetDeviceManager();

    // Inicializar el material
    InitMaterial();
    InitShadowMaterial();

    m_allocator = ChunkBufferAllocator(MaxVertexCount, MaxIndexCount);

    //m_meshAsset = AssetLocator::GetMeshAsset("TerrainMesh");

    // NOTA: Los búferes dinámicos se crean en el TerrainPass,
    // que es quien conoce el tamaño máximo del terreno.
    return S_OK;
}

void TerrainAsset::InitMaterial() {
    m_textureTransforms = m_terrainConfig->texture_transforms;
    m_material = new Material();
    if (!m_material) {
        OutputDebugStringA("TerrainAsset::InitMaterial - ERROR: Failed to create Material.\n");
        return;
    }
    m_material->SetShaderName(m_shaderName);
    m_material->Init();

    std::string textureAssetName = m_terrainConfig->texture;

    if (!textureAssetName.empty() && textureAssetName != "none") {
        m_textureAsset = AssetLocator::GetTextureAsset(textureAssetName);
        m_textureAsset->SetTextureView(m_material);
        m_material->SetTextureType(m_textureAsset->GetTextureType());
        if (m_textureTransforms.size() == 4) {
            XMFLOAT4 textureTransforms = XMFLOAT4(m_textureTransforms[0], m_textureTransforms[1], m_textureTransforms[2], m_textureTransforms[3]);
            m_material->SetTextureTranforms(textureTransforms);
        }
    }
}

void TerrainAsset::InitShadowMaterial() {
    if (!m_terrainConfig->cast_shadows) {
        return;
    }
    m_shadowMaterial = new Material();
    m_shadowMaterial->SetShaderName(StringToWstring(m_terrainConfig->shader_shadows));
    m_shadowMaterial->Init();
}

bool TerrainAsset::ShouldCompact() const {
    float fragmentation = m_allocator.FragmentationRatio();
    return fragmentation > 0.3f; // Umbral configurable
}

Chunk* TerrainAsset::FindChunkByRegion(const ChunkBufferRegion& region) {
    size_t key = HashRegion(region);
    auto it = m_regionToChunkMap.find(key);
    return it != m_regionToChunkMap.end() ? it->second : nullptr;
}

void TerrainAsset::GenerateMesh(std::vector<Chunk*> chunks, int indexBuffer) {
    if (chunks.empty() || m_isGeneratingMesh) return;
    m_isGeneratingMesh = true;

    // A. Verificar el tamaño del vértice (Asume que es correcto)
    m_vertexTypeSize = sizeof(VertexDefinition::TextureMapVertex);

    auto& dirtyChunks = chunks;

    // 2. Calcular el tamaño total
    size_t totalVertexCount = 0;
    size_t totalIndexCount = 0;
    for (Chunk* chunk : dirtyChunks) {
        totalVertexCount += chunk->GetVertexCount();
        totalIndexCount += chunk->GetIndexCount();
    }

    if (totalIndexCount == 0 || totalVertexCount == 0) {
        m_isGeneratingMesh = false;
        return;
    }

    Microsoft::WRL::ComPtr<ID3D11Buffer> vBuffer = m_vertexBuffer[indexBuffer];
    Microsoft::WRL::ComPtr<ID3D11Buffer> iBuffer = m_indexBuffer[indexBuffer];

    // 3. Crear o redimensionar los buffers dinámicos solo si es necesario (Solución 2)
    // NOTA: Debes pasar 'indexBuffer' a esta función.
    CreateDynamicBuffers(totalVertexCount, totalIndexCount, indexBuffer, vBuffer, iBuffer);

    m_vertexBuffer[indexBuffer] = vBuffer;
    m_indexBuffer[indexBuffer] = iBuffer;

    // --- SOLUCIÓN 1: Pre-asignación y llenado eficiente ---

    std::vector<uint8_t>& vertexData = m_vertexData[indexBuffer];
    std::vector<uint16_t>& indexData = m_indexData[indexBuffer];

    // 5. Redimensionar los vectores temporales UNA SOLA VEZ
    // Esto garantiza el tamaño exacto y evita reasignaciones
    size_t vertexDataBytes = totalVertexCount * m_vertexTypeSize;
    vertexData.resize(vertexDataBytes); // ¡Solo resize!
    indexData.resize(totalIndexCount);

    // Punteros de escritura raw
    uint8_t* currentVertexPtr = vertexData.data();
    uint16_t* currentIndexPtr = indexData.data();
    size_t currentVertexOffset = 0;

    // 6. Llenar los buffers temporales con memcpy
    for (Chunk* chunk : dirtyChunks) {
        const auto& vertices = chunk->GetChunkVertices();
        const auto& indexes = chunk->GetIndexes();

        // Copiar los vértices (usando memcpy y avance de puntero)
        for (const auto& vertex : vertices) {
            // Copia m_vertexTypeSize bytes desde GetRawData() a currentVertexPtr
            memcpy(currentVertexPtr, vertex->GetRawData(), m_vertexTypeSize);
            currentVertexPtr += m_vertexTypeSize; // Avanza el puntero
        }

        // Copiar y ajustar los índices
        for (uint16_t index : indexes) {
            *currentIndexPtr = index + static_cast<uint16_t>(currentVertexOffset);
            currentIndexPtr++;
        }

        // Mover el offset de vértices para el próximo chunk
        currentVertexOffset += vertices.size();
    }
    // ----------------------------------------------------

    // 8. Actualizar los contadores totales
    this->m_vertexCount[indexBuffer] = static_cast<UINT>(totalVertexCount);
    this->m_indexCount[indexBuffer] = static_cast<UINT>(totalIndexCount);

    // 9. Marcar los chunks como procesados
    for (Chunk* chunk : dirtyChunks) {
        chunk->SetFlag(FLAG_CHUNK_WITH_VERTEX_BUFFER, true);
    }

    m_isGeneratingMesh = false;
}

//void TerrainAsset::GenerateMesh(std::vector<Chunk*> chunks, int indexBuffer) {
//    if (chunks.empty() || m_isGeneratingMesh) return;
//    m_isGeneratingMesh = true;
//
//    m_vertexTypeSize = sizeof(VertexDefinition::TextureMapVertex);
//
//    auto& dirtyChunks = chunks;
//
//    // 2. Calcular el tamaño total necesario para los buffers
//    size_t totalVertexCount = 0;
//    size_t totalIndexCount = 0;
//    for (Chunk* chunk : dirtyChunks) {
//        totalVertexCount += chunk->GetVertexCount();
//        totalIndexCount += chunk->GetIndexCount();
//    }
//
//    if (totalIndexCount == 0 || totalVertexCount == 0) {
//        m_isGeneratingMesh = false;
//        return; // Nada que actualizar
//    }
//
//    Microsoft::WRL::ComPtr<ID3D11Buffer> vBuffer = m_vertexBuffer[indexBuffer];
//    Microsoft::WRL::ComPtr<ID3D11Buffer> iBuffer = m_indexBuffer[indexBuffer];
//
//    // 3. Crear o redimensionar los buffers dinámicos si la capacidad es insuficiente
//    CreateDynamicBuffers(totalVertexCount, totalIndexCount, vBuffer, iBuffer);
//
//    m_vertexBuffer[indexBuffer] = vBuffer;
//    m_indexBuffer[indexBuffer] = iBuffer;
//
//    // 5. Redimensionar los vectores temporales una sola vez
//    //m_tempVertexData.resize(totalVertexCount * m_vertexTypeSize);
//    std::vector<uint8_t>& vertexData = m_vertexData[indexBuffer];
//    std::vector<uint16_t>& indexData = m_indexData[indexBuffer];
//
//    vertexData.clear();
//    indexData.resize(totalIndexCount);
//
//    uint16_t* currentIndexPtr = indexData.data();
//    size_t currentVertexOffset = 0;
//
//    // 6. Llenar los buffers temporales de forma segura con memcpy
//    for (Chunk* chunk : dirtyChunks) {
//        const auto& vertices = chunk->GetChunkVertices();
//        const auto& indexes = chunk->GetIndexes();
//        size_t reservedSize = vertexData.size() + (vertices.size() * m_vertexTypeSize);
//        vertexData.reserve(reservedSize);
//        // Copiar los vértices
//        for (const auto& vertex : vertices) {
//            const void* source_data = vertex->GetRawData();
//            vertexData.insert(vertexData.end(), (const uint8_t*)source_data, (const uint8_t*)source_data + m_vertexTypeSize);
//        }
//
//        // Copiar y ajustar los índices
//        for (uint16_t index : indexes) {
//            *currentIndexPtr = index + static_cast<uint16_t>(currentVertexOffset);
//            currentIndexPtr++;
//        }
//
//        // Mover los punteros de escritura para el próximo chunk
//        currentVertexOffset += vertices.size();
//    }
//
//    // 8. Actualizar los contadores totales
//    this->m_vertexCount[indexBuffer] = static_cast<UINT>(totalVertexCount);
//    this->m_indexCount[indexBuffer] = static_cast<UINT>(totalIndexCount);
//
//
//    // 9. Marcar los chunks como procesados
//    for (Chunk* chunk : dirtyChunks) {
//        chunk->SetFlag(FLAG_CHUNK_WITH_VERTEX_BUFFER, true);
//    }
//
//    m_isGeneratingMesh = false;
//}

//void TerrainAsset::GenerateMesh(std::vector<Chunk*> chunks, int indexBuffer) {
//    if (chunks.empty() || m_isGeneratingMesh) return;
//    m_isGeneratingMesh = true;
//
//    m_vertexTypeSize = sizeof(VertexDefinition::TextureMapVertex);
//
//    /*Microsoft::WRL::ComPtr<ID3D11Buffer> vBuffer{};
//    Microsoft::WRL::ComPtr<ID3D11Buffer> iBuffer{};
//
//    vBuffer.Attach(m_vertexBuffer.Detach());
//    iBuffer.Attach(m_indexBuffer.Detach());*/
//
//    //auto lock = LockBuffers();
//
//    // 1. Obtener la lista de chunks que necesitan ser actualizados
//   /* PointerQueryRaw<Chunk> query(chunks);
//    auto dirtyChunks = query
//        .Filter([](Chunk* c) { return !c->GetFlag(FLAG_CHUNK_WITH_VERTEX_BUFFER); })
//        .ToVector();
//
//    if (dirtyChunks.empty()) return;*/
//    auto& dirtyChunks = chunks;
//
//    // 2. Calcular el tamaño total necesario para los buffers
//    size_t totalVertexCount = 0;
//    size_t totalIndexCount = 0;
//    for (Chunk* chunk : dirtyChunks) {
//        totalVertexCount += chunk->GetVertexCount();
//        totalIndexCount += chunk->GetIndexCount();
//    }
//
//    if (totalIndexCount == 0 || totalVertexCount == 0) {
//        m_isGeneratingMesh = false;
//        return; // Nada que actualizar
//    }
//
//    Microsoft::WRL::ComPtr<ID3D11Buffer> vBuffer = m_vertexBuffer[indexBuffer];
//    Microsoft::WRL::ComPtr<ID3D11Buffer> iBuffer = m_indexBuffer[indexBuffer];
//
//    // 3. Crear o redimensionar los buffers dinámicos si la capacidad es insuficiente
//    //if (totalVertexCount > m_vertexCount[indexBuffer] || totalIndexCount > m_indexCount[indexBuffer] || vBuffer == nullptr) {
//    CreateDynamicBuffers(totalVertexCount, totalIndexCount, vBuffer, iBuffer);
//    //}
//
//    m_vertexBuffer[indexBuffer] = vBuffer;
//    m_indexBuffer[indexBuffer] = iBuffer;
//
//    // 4. Compactar si se ha solicitado (lógica de gestión de memoria)
//    //if (ShouldCompact()) {
//    //    m_allocator.Compact([&](const ChunkBufferRegion& oldRegion, ChunkBufferRegion& newRegion) {
//    //        // Lógica para encontrar el chunk por la región (simulado)
//    //        // Y actualizar la región del chunk
//    //        Chunk* chunk = nullptr; // chunk = FindChunkByRegion(oldRegion);
//    //        if (chunk) {
//    //            chunk->SetRegion(newRegion);
//    //        }
//    //        });
//    //}
//
//    // 5. Redimensionar los vectores temporales una sola vez
//    //m_tempVertexData.resize(totalVertexCount * m_vertexTypeSize);
//    std::vector<uint8_t>& vertexData = m_vertexData[indexBuffer];
//    std::vector<uint16_t>& indexData = m_indexData[indexBuffer];
//
//    vertexData.clear();
//    indexData.resize(totalIndexCount);
//
//    uint16_t* currentIndexPtr = indexData.data();
//    size_t currentVertexOffset = 0;
//
//    // 6. Llenar los buffers temporales de forma segura con memcpy
//    for (Chunk* chunk : dirtyChunks) {
//        const auto& vertices = chunk->GetChunkVertices();
//        const auto& indexes = chunk->GetIndexes();
//        size_t reservedSize = vertexData.size() + (vertices.size() * m_vertexTypeSize);
//        vertexData.reserve(reservedSize);
//        // Copiar los vértices
//        //size_t vertexByteCount = vertices.size() * m_vertexTypeSize;
//        //memcpy(currentVertexPtr, vertices.data(), vertexByteCount);
//        for (const auto& vertex : vertices) {
//            const void* source_data = vertex->GetRawData();
//            vertexData.insert(vertexData.end(), (const uint8_t*)source_data, (const uint8_t*)source_data + m_vertexTypeSize);
//        }
//
//        // Copiar y ajustar los índices
//        for (uint16_t index : indexes) {
//            *currentIndexPtr = index + static_cast<uint16_t>(currentVertexOffset);
//            currentIndexPtr++;
//        }
//
//        // Mover los punteros de escritura para el próximo chunk
//        //currentVertexPtr += vertexByteCount;
//        currentVertexOffset += vertices.size();
//    }
//
//    // 7. Mapear los buffers de Direct3D y copiar los datos
//    //D3D11_MAPPED_SUBRESOURCE mappedVertices;
//    //D3D11_MAPPED_SUBRESOURCE mappedIndices;
//
//    //ID3D11Buffer* vB = vBuffer.Get();
//
//    //HRESULT hr = m_deviceManager->GetContext()->Map(vB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertices);
//    //if (FAILED(hr)) return;
//    //hr = m_deviceManager->GetContext()->Map(iBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIndices);
//    //if (FAILED(hr)) {
//    //    m_deviceManager->GetContext()->Unmap(vBuffer.Get(), 0);
//    //    return;
//    //}
//
//    //memcpy(mappedVertices.pData, m_tempVertexData.data(), m_tempVertexData.size());
//    //memcpy(mappedIndices.pData, m_tempIndexData.data(), m_tempIndexData.size() * sizeof(UINT));
//
//    //m_deviceManager->GetContext()->Unmap(vBuffer.Get(), 0);
//    //m_deviceManager->GetContext()->Unmap(iBuffer.Get(), 0);
//
//    // 8. Actualizar los contadores totales
//    this->m_vertexCount[indexBuffer] = static_cast<UINT>(totalVertexCount);
//    this->m_indexCount[indexBuffer] = static_cast<UINT>(totalIndexCount);
//
//    /*OutputDebugStringA(("Terrain Generado: VertexCount:" + ParseInt(totalVertexCount) + " IndexCount:" + ParseInt(totalIndexCount) + "\n").c_str());*/
//
//    // 9. Marcar los chunks como procesados
//    for (Chunk* chunk : dirtyChunks) {
//        chunk->SetFlag(FLAG_CHUNK_WITH_VERTEX_BUFFER, true);
//    }
//
//    /*SetVertexBuffer(vBuffer);
//    SetIndexBuffer(iBuffer);*/
//
//    m_isGeneratingMesh = false;
//}

void TerrainAsset::CreateDynamicBuffers(
    size_t vertexCount,
    size_t indexCount,
    int indexBuffer, // Indice de 0 o 1 para el double-buffering
    Microsoft::WRL::ComPtr<ID3D11Buffer>& vertexBuffer,
    Microsoft::WRL::ComPtr<ID3D11Buffer>& indexBufferComPtr)
{

    const size_t requiredVertexBytes = vertexCount * m_vertexTypeSize;
    // Corregido: Usamos sizeof(uint16_t) ya que tus datos son uint16_t
    const size_t requiredIndexBytes = indexCount * sizeof(uint16_t);

    // --- Lógica para el Vertex Buffer ---
    // Si el búfer es nulo O la capacidad requerida excede la capacidad actual, recrear.
    if (!vertexBuffer || requiredVertexBytes > m_currentVertexCapacityBytes[indexBuffer]) {

        D3D11_BUFFER_DESC vertexBufferDesc = {};
        vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        vertexBufferDesc.ByteWidth = static_cast<UINT>(requiredVertexBytes);
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        // La ComPtr se encarga de liberar la referencia al buffer antiguo
        HRESULT hr = m_deviceManager->GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &vertexBuffer);

        if (SUCCEEDED(hr)) {
            // Actualizar la capacidad solo si la creación fue exitosa
            m_currentVertexCapacityBytes[indexBuffer] = requiredVertexBytes;
        }
        else {
            // Manejo de error: Capacidad insuficiente o fallo de GPU
        }
    }

    // --- Lógica para el Index Buffer ---
    if (!indexBufferComPtr || requiredIndexBytes > m_currentIndexCapacityBytes[indexBuffer]) {

        D3D11_BUFFER_DESC indexBufferDesc = {};
        indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        indexBufferDesc.ByteWidth = static_cast<UINT>(requiredIndexBytes); // Tamaño correcto
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        HRESULT hr = m_deviceManager->GetDevice()->CreateBuffer(&indexBufferDesc, nullptr, &indexBufferComPtr);

        if (SUCCEEDED(hr)) {
            // Actualizar la capacidad solo si la creación fue exitosa
            m_currentIndexCapacityBytes[indexBuffer] = requiredIndexBytes;
        }
        else {
            // Manejo de error
        }
    }
}

//void TerrainAsset::CreateDynamicBuffers(size_t vertexCount, size_t indexCount, Microsoft::WRL::ComPtr<ID3D11Buffer>& vertexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer>& indexBuffer) {
//    // Lógica para crear un búfer dinámico, simulado
//    D3D11_BUFFER_DESC vertexBufferDesc = {};
//    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//    vertexBufferDesc.ByteWidth = static_cast<UINT>(vertexCount * m_vertexTypeSize);
//    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//
//    D3D11_BUFFER_DESC indexBufferDesc = {};
//    indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//    indexBufferDesc.ByteWidth = static_cast<UINT>(indexCount * sizeof(UINT));
//    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//    indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//
//    // Aquí iría la llamada real a m_deviceManager->GetDevice()->CreateBuffer()
//    m_deviceManager->GetDevice()->CreateBuffer(&vertexBufferDesc, nullptr, &vertexBuffer);
//    m_deviceManager->GetDevice()->CreateBuffer(&indexBufferDesc, nullptr, &indexBuffer);
//}

XMFLOAT4 TerrainAsset::GetTextureTransforms() {
    XMFLOAT4 defaultTransform(1.0f, 1.0f, 0.0f, 0.0f);
    if (m_textureTransforms.empty()) {
        // Si no hay transformaciones de textura, obtenemos las del asset de textura
        defaultTransform = m_material->GetTextureTranforms();
    }
    else {
        defaultTransform = XMFLOAT4(m_textureTransforms[0], m_textureTransforms[1], m_textureTransforms[2], m_textureTransforms[3]);
    }
    return defaultTransform;
}

std::vector<uint8_t> TerrainAsset::GetVertexData(int index) const {
    return m_vertexData[index];
}
std::vector<uint16_t> TerrainAsset::GetIndexData(int index) const {
    return m_indexData[index];
}

void TerrainAsset::UnregisterChunk(Chunk* chunk) {
    size_t key = HashRegion(chunk->GetRegion());
    m_regionToChunkMap.erase(key);
}

void TerrainAsset::Shutdown() {
    for (auto& vBuffer : m_vertexBuffer) {
        if (vBuffer) { vBuffer.Reset(); }
    }
    for (auto& iBuffer : m_indexBuffer) {
        if (iBuffer) { iBuffer.Reset(); }
    }
    if (m_material) { SafeShutDown(m_material); }
    if (m_shadowMaterial) { SafeShutDown(m_shadowMaterial); }
}
