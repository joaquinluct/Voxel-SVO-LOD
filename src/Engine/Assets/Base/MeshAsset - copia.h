// MeshAsset.h
#pragma once
#include <Config/Assets/Mesh/SkyboxCubeConfig.h>
#include <Defines/VertexDefinition.h>
#include <Assets/Base/AssetBase.h>
#include <Assets/Base/VertexAsset.h>
#include "Assets/IAssetMesh.h"
#include "Assets/IAssetMeshConfigBase.h"
#include "Assets/IAssetMeshConfig.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <wrl/client.h> // Para Microsoft::WRL::ComPtr
#include <variant>
#include <Mesh/SkyboxCube.h>
#include <DeviceManager.h>
#include <CameraManager.h>
#include <WorldMatrixManager.h>
#include <KeyboardManager.h>
#include <ShaderManager.h>
#include <Material/Material.h>
#include <Camera/FirstPersonCamera.h>
#include <Config/ITextureConfig.h>
#include <ConfigBase.h>

class MeshAsset : public AssetBase {
public:
    MeshAsset();
    ~MeshAsset() override;
    // IAsset overrides
    void Load() override {};
    void Unload() override {};
    HRESULT Init() override;
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

    //virtual std::vector<VertexDefinition::BaseVertex> GetVertex() = 0;

    void SetType(const std::string& type);
    // Métodos específicos de MeshAsset
    /*template <typename T>
    HRESULT InitD3D11Resources(ID3D11Device* pDevice);*/
    
    HRESULT InitD3D11ResourcesSimpleVertex(ID3D11Device* pDevice) {
  //      if (!pDevice) {
  //          OutputDebugStringA("ERROR: ID3D11Device is null when initializing mesh.\n");
  //          return E_INVALIDARG;
  //      }

		//m_vertextType = "SimpleVertex";

  //      std::vector<VertexDefinition::SimpleVertex> vertices;
  //      auto baseVertices = m_meshAsset->GetVertex<VertexDefinition::SimpleVertex>();
  //      vertices.reserve(baseVertices.size());
  //      for (const auto& v : baseVertices) {
  //          vertices.push_back(static_cast<const VertexDefinition::SimpleVertex&>(v));
  //      }
  //      const std::vector<WORD>& indices = m_meshAsset->GetIndex();

  //      if (vertices.empty() || indices.empty()) {
  //          OutputDebugStringA(("ERROR: No vertex or index data provided for mesh '" + m_name + "'.\n").c_str());
  //          return E_INVALIDARG;
  //      }

  //      HRESULT hr = S_OK;

  //      // Crear Vertex Buffer
  //      D3D11_BUFFER_DESC vbDesc = {};
  //      vbDesc.Usage = D3D11_USAGE_DEFAULT;
  //      vbDesc.ByteWidth = static_cast<UINT>(sizeof(VertexDefinition::SimpleVertex) * vertices.size());
  //      vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  //      vbDesc.CPUAccessFlags = 0;
  //      vbDesc.MiscFlags = 0;

  //      D3D11_SUBRESOURCE_DATA vbInitData = {};
  //      vbInitData.pSysMem = vertices.data();
  //      hr = pDevice->CreateBuffer(&vbDesc, &vbInitData, &m_vertexBuffer);
  //      if (FAILED(hr)) {
  //          OutputDebugStringA(("ERROR: Failed to create vertex buffer for mesh '" + m_name + "'. HRESULT: " + std::to_string(hr) + "\n").c_str());
  //          return hr;
  //      }
  //      m_vertexCount = static_cast<UINT>(vertices.size());

  //      // Crear Index Buffer
  //      D3D11_BUFFER_DESC ibDesc = {};
  //      ibDesc.Usage = D3D11_USAGE_DEFAULT;
  //      ibDesc.ByteWidth = sizeof(WORD) * static_cast<UINT>(indices.size());
  //      ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
  //      ibDesc.CPUAccessFlags = 0;
  //      ibDesc.MiscFlags = 0;

  //      D3D11_SUBRESOURCE_DATA ibInitData = {};
  //      ibInitData.pSysMem = indices.data();
  //      hr = pDevice->CreateBuffer(&ibDesc, &ibInitData, &m_indexBuffer);
  //      if (FAILED(hr)) {
  //          OutputDebugStringA(("ERROR: Failed to create index buffer for mesh '" + m_name + "'. HRESULT: " + std::to_string(hr) + "\n").c_str());
  //          // Liberar vertex buffer si el index buffer falla
  //          //m_vertexBuffer.Reset();
  //          m_vertexBuffer->Release();
  //          return hr;
  //      }
  //      m_indexCount = static_cast<UINT>(indices.size());

        //OutputDebugStringA(("MeshAsset '" + m_name + "' D3D11 resources initialized.\n").c_str());
        return S_OK;
    }

    // Getters para los buffers
    ID3D11Buffer* const* GetVertexBuffer() { return &m_vertexBuffer; }
    ID3D11Buffer* GetIndexBuffer() const { return m_indexBuffer.Get(); }
    UINT GetIndexCount() const { return m_indexCount; }
    UINT GetVertexCount() const { return m_vertexCount; }

	void SetTexture(const std::string& textureName);
    void SetTexture(ID3D11ShaderResourceView* textureResource);

    void SetShaderName(const std::string& shaderName) { m_shaderName = shaderName; };
    void PrepareViewMatrix(ID3D11DeviceContext* context);

    void SetConfig(std::shared_ptr<ConfigBase> config) {
        m_textureConfig = dynamic_cast<ITextureConfig*>(config.get());
    }

private:
    ITextureConfig* m_textureConfig = nullptr; // Configuración de textura
    //Axis* m_axis;
    std::shared_ptr<ShaderManager> m_shaderManager;
    std::shared_ptr<WorldMatrixManager> m_wMatrixManager;
    std::shared_ptr<CameraManager> m_cameraManager;
	std::shared_ptr<DeviceManager> m_deviceManager;

    Material* m_material;
    //std::shared_ptr<VertexAsset> m_meshAsset;

    std::shared_ptr<ICamera> m_camera;

    std::string m_vertextType = "";
    IAssetMeshConfigBase* m_config;

	std::string m_shaderName;

    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11InputLayout* m_inputLayout;

    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    std::string m_name;
    ID3D11Buffer* m_vertexBuffer;
    ComPtr<ID3D11Buffer> m_indexBuffer;
    UINT m_vertexCount;
    UINT m_indexCount;

    // Evitar copias
    /*MeshAsset(const MeshAsset&) = delete;
    MeshAsset& operator=(const MeshAsset&) = delete;*/
};