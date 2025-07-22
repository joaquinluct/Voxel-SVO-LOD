// MeshAsset.h
#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <memory>
#include <IDefine/IVertex.h>
#include <Assets/Base/AssetBase.h>
#include <ConfigBase.h>
#include <Config/MeshAssetConfigBase.h>
#include <Defines/VertexDefinition.h>
#include <DeviceManager.h>
#include <CameraManager.h>
#include <ShaderManager.h>
#include <Services/Material.h>

class TextureAsset;

class MeshAsset : public AssetBase {
private:
    UINT m_vertexCount = 0;
    UINT m_indexCount = 0;

    UINT m_vertexTypeSize = 0;

    std::shared_ptr<MeshAssetConfigBase> m_meshConfig;
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<CameraManager> m_cameraManager;
    std::shared_ptr <ShaderManager> m_shaderManager;
    std::shared_ptr<TextureAsset> m_textureAsset;

    ID3D11Buffer* m_vertexBuffer;
    ID3D11Buffer* m_indexBuffer;

    Material* m_material;

    // Evitar copias
    /*MeshAsset(const MeshAsset&) = delete;
    MeshAsset& operator=(const MeshAsset&) = delete;*/
public:
    MeshAsset();
    ~MeshAsset();
    // IAsset overrides
    void Load() override {};
    void Unload() override {};
    HRESULT Init() override;
    HRESULT InitManagers();
    HRESULT InitTexture();
    HRESULT InitMesh();
    void Render() override;
    void Update(float deltaTime) override {};
    void Shutdown() override;
    const std::string& GetAssetName() override {
        static const std::string name = "MeshAsset";
        return name;
    }
    static const std::string& GetStaticAssetName()
    {
        static const std::string name = "MeshAsset";
        return name;
    }
        
    HRESULT CreateVertexBuffer(std::shared_ptr<ID3D11Device> pDevice, const std::vector<std::shared_ptr<VertexDefinition::VertexVariant>> vertex);

    HRESULT CreateIndexBuffer(std::shared_ptr<ID3D11Device> pDevice, const std::vector<uint16_t>& indexes) {
        // Crear Vertex Buffer
        D3D11_BUFFER_DESC ibDesc = {};
        ibDesc.Usage = D3D11_USAGE_DEFAULT;
        ibDesc.ByteWidth = sizeof(WORD) * static_cast<uint16_t>(indexes.size());
        ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibDesc.CPUAccessFlags = 0;
        ibDesc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA ibInitData = {};
        ibInitData.pSysMem = indexes.data();
        HRESULT hr = pDevice->CreateBuffer(&ibDesc, &ibInitData, &m_indexBuffer);
        if (FAILED(hr)) {
            OutputDebugStringA(("ERROR: Failed to create index buffer for mesh '" + m_meshConfig->name + "'. HRESULT: " + std::to_string(hr) + "\n").c_str());
            // Liberar vertex buffer si el index buffer falla
            //m_vertexBuffer.Reset();
            m_vertexBuffer->Release();
            return hr;
        }
        m_indexCount = static_cast<UINT>(indexes.size());
        return hr;
    }

    
    HRESULT InitD3D11ResourcesVertex(std::shared_ptr <ID3D11Device> pDevice, std::vector<std::shared_ptr<VertexDefinition::VertexVariant>> vertex, std::vector<uint16_t> indexes)  {

		HRESULT hr = CreateVertexBuffer(pDevice, vertex);

        if (FAILED(hr)) {
            OutputDebugStringA(("ERROR: Failed to create vertex buffer for mesh '" + m_meshConfig->name + "'. HRESULT: " + std::to_string(hr) + "\n").c_str());
            return hr;
		}

		hr = CreateIndexBuffer(pDevice, indexes);

        return hr;
    }

    void SetConfig(std::shared_ptr<ConfigBase> config) {
        m_meshConfig = std::dynamic_pointer_cast<MeshAssetConfigBase>(config);
    }
};