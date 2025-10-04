#pragma once

#include "AssetBase.h"
#include <atomic>
#include <Config/MeshAssetConfigBase.h>
#include <cstdint>
#include <d3d11.h>
#include <Defines/Mesh.h>
#include <Defines/Structs/RingBuffer.h>
#include <Defines/VertexDefinition.h>
#include <DirectXMath.h>
#include <Game/Systems/Terrain/Chunk/Chunk.h>
#include <memory>
#include <mutex>
#include <Services/Material.h>
#include <string>
#include <Util/GUID.h>
#include <Util/Text/Text.h>
#include <vector>
#include <Windows.h>
#include <wrl/client.h>
#include <DirectXMathMatrix.inl>

class DeviceManager;
class CameraManager;
class ShaderManager;
class ShaderAsset;
class TextureAsset;

// ---------------------------------------------
// MessAssetBase
// ---------------------------------------------
class MeshAssetBase : public AssetBase
{
protected:
    // -----------------------------------------
    // Datos base
    // -----------------------------------------
    std::string m_name;
    DirectX::XMMATRIX m_worldMatrix = DirectX::XMMatrixIdentity();
    std::string m_meshObj;
    std::string m_shaderAssetName;
    std::wstring m_shaderName;
    std::string m_vertexDef;
    Mesh::Type m_meshType;
    std::string m_textureType;
    std::shared_ptr<TextureAsset> m_textureAsset;
    std::vector<float> m_textureTransforms;

    // -----------------------------------------
    // Material
    // -----------------------------------------
    Material* m_material;
    Material* m_shadowMaterial;
    int m_passes;

    // -----------------------------------------
    // Managers
    // -----------------------------------------
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<CameraManager> m_cameraManager;
    std::shared_ptr <ShaderManager> m_shaderManager;
    std::shared_ptr<ShaderAsset> m_shaderAsset;


    // -----------------------------------------
    // Config
    // -----------------------------------------
    std::shared_ptr<MeshAssetConfigBase> m_meshConfig;

    // -----------------------------------------
    // Buffers
    // -----------------------------------------
    std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_vertexBuffer;
    std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_indexBuffer;

    // -----------------------------------------
    // Data
    // -----------------------------------------
    std::vector<std::vector<uint8_t>> m_vertexData;
    std::vector<std::vector<uint16_t>> m_indexData;

    std::vector<std::shared_ptr<VertexDefinition::VertexVariant>> m_vertexes;
    std::vector<uint16_t> m_indexes;
    std::vector<UINT> m_vertexCount;
    std::vector<UINT> m_indexCount;
    UINT m_vertexTypeSize = 0;

    // Threading
    std::mutex m_mutex;
    std::string uniqueID = generateUID();

    std::atomic<int> m_readIndex{ 1 };
    std::atomic<int> m_writeIndex{ 0 };

    // RingBuffer: Almacena la ubicación actual de los datos en el Ring Buffer.
    RingAllocation m_currentVertexAllocation;
    RingAllocation m_currentIndexAllocation;
public:

    MeshAssetBase() : AssetBase(),
        m_material(nullptr), m_shadowMaterial(nullptr),
        m_vertexBuffer(), m_indexBuffer(),
        m_meshConfig(nullptr), m_vertexes(), m_indexes(), m_vertexCount(0), m_indexCount(0), m_vertexTypeSize(0), m_meshType(Mesh::Type::None)
    {
    }

    MeshAssetBase(const MeshAssetBase* other) :
        m_material(other->m_material), m_shadowMaterial(other->m_shadowMaterial),
        m_vertexBuffer(other->m_vertexBuffer), m_indexBuffer(other->m_indexBuffer),
        m_meshConfig(other->m_meshConfig), m_vertexes(other->m_vertexes), m_indexes(other->m_indexes),
        m_vertexCount(other->m_vertexCount), m_indexCount(other->m_indexCount), m_vertexTypeSize(other->m_vertexTypeSize), m_meshType(other->m_meshType), m_name(other->m_name), m_worldMatrix(other->m_worldMatrix), m_shaderName(other->m_shaderName)
    {
    }
    ~MeshAssetBase() = default;

    // -----------------------------------------
    // Interface AssetBase
    // -----------------------------------------
    HRESULT Init() override;
    void Shutdown();

    // -----------------------------------------
    // Inicialización
    // -----------------------------------------
    HRESULT InitBuffers();
    HRESULT InitManagers();
    HRESULT InitConfig();
    HRESULT InitTexture();
    HRESULT InitShadows();

    // -----------------------------------------
    // Generación de Buffers del mesh
    // -----------------------------------------
    virtual void GenerateMesh() {};
    virtual void GenerateMesh(std::vector<Chunk*> chunks, int indexBuffer) {};
    void ClearMeshData();
    virtual bool IsGenerating() const { return false; }

    // -----------------------------------------
    // Getters
    // -----------------------------------------
    std::string GetName() const;
    virtual int GetRenderPassesValue() const { return m_passes; }
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer(int index);
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer(int index);
    void SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, int index);
    void SetIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer, int index);
    UINT GetVertexCount(int index) const { return m_vertexCount[index]; }
    UINT GetIndexCount(int index) const { return m_indexCount[index]; }
    virtual UINT GetVertexTypeSize() const { return m_vertexTypeSize; }
    virtual std::vector<uint8_t> GetVertexData(int index) const { return {}; }
    virtual std::vector<uint16_t> GetIndexData(int index) const { return {}; }
    Mesh::Type GetMeshType() const { return m_meshType; }
    DirectX::XMMATRIX GetWorldMatrix() const { return m_worldMatrix; }
    Material* GetMaterial() const { return m_material; }
    Material* GetShadowMaterial() const { return m_shadowMaterial; }
    std::shared_ptr<TextureAsset> GetTextureAsset() const { return m_textureAsset; }
    std::string GetTextureType() const { return m_textureType; }
    std::vector<float> GetTextureTransforms() const { return m_textureTransforms; }
    std::string GetShaderAssetName() const { return m_shaderAssetName; }
    const std::string GetShaderName() const {
        if (m_shaderName.size() > 0) {
            return WstringToString(m_shaderName);
        }
        if (!m_meshConfig) {
            return "";
        }
        return m_meshConfig->shader;
    }
    std::string GetVertexDef() const { return m_vertexDef; }
    virtual Mesh::DrawType GetDrawType() const;
    D3D11_PRIMITIVE_TOPOLOGY GetPrimitiveTopology() const;

    // -----------------------------------------
    // Configuración
    // -----------------------------------------
    void SetConfig(std::shared_ptr<ConfigBase> config) override {
        m_meshConfig = std::dynamic_pointer_cast<MeshAssetConfigBase>(config);
    }
    std::shared_ptr<MeshAssetConfigBase> GetConfig() const { return m_meshConfig; }

    // Threading: Doble buffering de los datos del mesh para evitar bloqueos
    // Itentando substituirla por Ring Buffer PARA QUE QUEDE DEPRECATED el doble buffering
    std::unique_lock<std::mutex> LockBuffers();

    void SwapBuffer();

    int GetReadIndex() const { return m_readIndex.load(); }
    int GetWriteIndex() const { return m_writeIndex.load(); }

    // Ring buffer - Técnica para evitar bloqueos en la actualización de buffers
    void SetVertexAllocation(const RingAllocation& alloc) { m_currentVertexAllocation = alloc; }
    const RingAllocation& GetVertexAllocation() const { return m_currentVertexAllocation; }
    void SetIndexAllocation(const RingAllocation& alloc) { m_currentIndexAllocation = alloc; }
    const RingAllocation& GetIndexAllocation() const { return m_currentIndexAllocation; }
    ID3D11Buffer* GetVertexRingBuffer() const;
    ID3D11Buffer* GetIndexRingBuffer() const;
};
