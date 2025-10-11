#pragma once

#include "MeshAssetBase.h"
#include <Assets/Base/TextureAsset.h>
#include <Config/MeshAssetConfigBase.h>
#include <cstdint>
#include <d3d11.h>
#include <Defines/Mesh.h>
#include <Defines/VertexDefinition.h>
#include <DirectXMath.h>
// #include <Game/Systems/Terrain/Chunk/Chunk.h> // No longer needed - UIAsset doesn't use Chunk
#include <memory>
#include <mutex>
#include <Services/Material.h>
#include <string>
#include <Util/GUID.h>
#include <vector>
#include <wrl/client.h>

class DeviceManager;
class CameraManager;
class ShaderManager;
class ShaderAsset;
class TextureAsset;

// ---------------------------------------------
// MessAssetBase
// ---------------------------------------------
class UIAsset : public MeshAssetBase
{
protected:
    // -----------------------------------------
    // Datos base
    // -----------------------------------------
    std::string m_name;
    DirectX::XMMATRIX m_worldMatrix = DirectX::XMMatrixIdentity();
    std::string m_meshObj;
    std::string m_shaderAssetName;
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
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

    // -----------------------------------------
    // Data
    // -----------------------------------------
    std::vector<std::shared_ptr<VertexDefinition::VertexVariant>> m_vertexes;
    std::vector<uint16_t> m_indexes;
    UINT m_vertexCount = 0;
    UINT m_indexCount = 0;
    UINT m_vertexTypeSize = 0;

    // Threading
    std::mutex m_mutex;
    std::string uniqueID = generateUID();
public:

    UIAsset() : MeshAssetBase(),
        m_material(nullptr), m_shadowMaterial(nullptr),
        m_vertexBuffer(nullptr), m_indexBuffer(nullptr),
        m_meshConfig(nullptr), m_vertexes(), m_indexes(), m_vertexCount(0), m_indexCount(0), m_vertexTypeSize(0), m_meshType(Mesh::Type::None)
    {
    }

    UIAsset(const UIAsset* other) :
        m_material(other->m_material), m_shadowMaterial(other->m_shadowMaterial),
        m_vertexBuffer(other->m_vertexBuffer), m_indexBuffer(other->m_indexBuffer),
        m_meshConfig(other->m_meshConfig), m_vertexes(other->m_vertexes), m_indexes(other->m_indexes),
        m_vertexCount(other->m_vertexCount), m_indexCount(other->m_indexCount), m_vertexTypeSize(other->m_vertexTypeSize), m_meshType(other->m_meshType), m_name(other->m_name), m_worldMatrix(other->m_worldMatrix)
    {
    }
    ~UIAsset() = default;

    // -----------------------------------------
    // Interface AssetBase
    // -----------------------------------------
    HRESULT Init() override;
    void Shutdown();

    // -----------------------------------------
    // Inicialización
    // -----------------------------------------
    HRESULT InitManagers();
    HRESULT InitConfig();
    HRESULT InitTexture();
    HRESULT InitShadows();

    // -----------------------------------------
    // Generación de Buffers del mesh
    // -----------------------------------------
    virtual void GenerateMesh() {};
    virtual void GenerateMesh(std::vector<IChunk*> chunks, int indexBuffer) {};
    void ClearMeshData();
    virtual bool IsGenerating() const { return false; }

    // -----------------------------------------
    // Getters
    // -----------------------------------------
    std::string GetName() const;
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer();
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer();
    void SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer);
    void SetIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer);
    UINT GetVertexCount() const { return m_vertexCount; }
    UINT GetIndexCount() const { return m_indexCount; }
    virtual UINT GetVertexTypeSize() const { return m_vertexTypeSize; }
    Mesh::Type GetMeshType() const { return m_meshType; }
    DirectX::XMMATRIX GetWorldMatrix() const { return m_worldMatrix; }
    Material* GetMaterial() const { return m_material; }
    Material* GetShadowMaterial() const { return m_shadowMaterial; }
    std::shared_ptr<TextureAsset> GetTextureAsset() const { return m_textureAsset; }
    std::string GetTextureType() const { return m_textureType; }
    std::vector<float> GetTextureTransforms() const { return m_textureTransforms; }
    std::string GetShaderAssetName() const { return m_shaderAssetName; }
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

    // Threading
    std::unique_lock<std::mutex> LockBuffers();
};
