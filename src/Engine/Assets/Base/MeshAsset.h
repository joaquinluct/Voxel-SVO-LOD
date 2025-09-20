// MeshAsset.h
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <string>
#include <vector>
#include <variant>
#include <memory>
#include <IDefine/IVertex.h>
#include <Assets/Base/MeshAssetBase.h>
#include <ConfigBase.h>
#include <Config/MeshAssetConfigBase.h>
#include <Defines/Pass.h>
#include <Defines/VertexDefinition.h>
#include <Defines/Mesh.h>
#include <DeviceManager.h>
#include <CameraManager.h>
#include <ShaderManager.h>
#include <Services/Material.h>
#include <UI/UIText.h>
#include <Defines/AreaMesh.h>

class TextureAsset;
class ShaderAsset;

class MeshAsset : public MeshAssetBase {
private:
    UIText* m_uiText;

    // Evitar copias
    /*MeshAsset(const MeshAsset&) = delete;
    MeshAsset& operator=(const MeshAsset&) = delete;*/
public:
    MeshAsset();
	MeshAsset(const MeshAsset& other) : m_uiText(other.m_uiText) {}
    MeshAsset(const MeshAsset* other) : m_uiText(other->m_uiText) {}    
    ~MeshAsset();
    // -----------------------------------
    // IAsset overrides
	// -----------------------------------
    //std::shared_ptr<AssetBase> Clone() const override { return std::make_shared<MeshAsset>(*this); }
    std::shared_ptr<MeshAssetBase> CloneAsMesh() const { return std::make_shared<MeshAsset>(*this); }
    void Load() override {};
    void Unload() override {};
    HRESULT Init() override;    
    void Render() override;
    void Update(float deltaTime) override {};
    void UpdateTextMesh(UIText* uiText, std::string text);
    void Shutdown() override;

	// -----------------------------------
	// Inicialización
	// -----------------------------------
    HRESULT InitMesh();

	// Generación de malla
    HRESULT GenerateMesh(std::vector<std::shared_ptr<VertexDefinition::VertexVariant>>& outVertices, std::vector<uint16_t>& outIndices) {};

	// -----------------------------------
	// Getters y Setters
	// -----------------------------------
    std::string GetName() const {
        if (m_name.empty() && m_meshConfig) {
            return m_meshConfig->name;
		}
        return m_name;
	}

    std::vector<RenderPassType> GetRenderPasses() const {
		return RenderPasses::GetRenderPasses(m_meshConfig->render_passes);
    }
    bool IsRenderPassEnabled(RenderPassType pass) const {
        return RenderPasses::IsRenderPassEnabled(m_meshConfig->render_passes, pass);
	}

    void SetPosition(const DirectX::XMFLOAT3& position) {
        m_worldMatrix = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	}
    void SetPosition(float x, float y, float z) {
        m_worldMatrix = DirectX::XMMatrixTranslation(x, y, z);
    }
    void SetRotation(const DirectX::XMFLOAT3& rotation) {
        m_worldMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	}
    void SetRotation(float pitch, float yaw, float roll) {
        m_worldMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);
	}
    void SetScale(const DirectX::XMFLOAT3& scale) {
        m_worldMatrix = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    }
    void SetScale(float x, float y, float z) {
        m_worldMatrix = DirectX::XMMatrixScaling(x, y, z);
    }
    void SetWorldMatrix(const DirectX::XMMATRIX& worldMatrix) {
        m_worldMatrix = worldMatrix;
	}

    const std::string& GetAssetName() override {
        static const std::string name = "MeshAsset";
        return name;
    }
    static const std::string& GetStaticAssetName()
    {
        static const std::string name = "MeshAsset";
        return name;
    }

    XMMATRIX GetWorldMatrix() const {
        return m_worldMatrix;
    }

    Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() const{
		return m_vertexBuffer;
	}    
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer() const {
        return m_indexBuffer;
    }
    UINT GetVertexCount() const {
        return m_vertexCount;
	}    
    UINT GetIndexCount() const {
        return m_indexCount;
    }
    UINT GetVertexTypeSize() const {
        return m_vertexTypeSize;
	}
    const std::string GetShaderName() const {
        if (!m_meshConfig) {
            return "";
        }
        return m_meshConfig->shader;
    }

    bool CastShadows() const {
        if (!m_meshConfig) {
            return false;
        }
        return m_meshConfig->cast_shadows;
	}

    bool ReceiveShadows() const {
        if (!m_meshConfig) {
            return false;
        }
        return m_meshConfig->receive_shadows;
    }

    void SetMaterial(Material* material) {
        m_material = material;
	}
    void SetShadowMaterial(Material* shadowMaterial) {
        m_shadowMaterial = shadowMaterial;
    }
    const Material* GetMaterial() const { return m_material; };
    Material* GetShadowMaterial() { return m_shadowMaterial; };

    XMFLOAT4 GetTextureTransforms() {
        if (!m_material) {
            return XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f);
		}
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
        
    HRESULT CreateVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, const std::vector<std::shared_ptr<VertexDefinition::VertexVariant>> vertex);

    HRESULT CreateIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, const std::vector<uint16_t>& indexes) {
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

    
    HRESULT InitD3D11ResourcesVertex(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, std::vector<std::shared_ptr<VertexDefinition::VertexVariant>> vertex, std::vector<uint16_t> indexes)  {

		HRESULT hr = CreateVertexBuffer(pDevice, vertex);

            if (FAILED(hr)) {
            OutputDebugStringA(("ERROR: Failed to create vertex buffer for mesh '" + m_meshConfig->name + "'. HRESULT: " + std::to_string(hr) + "\n").c_str());
            return hr;
		}

        if (Mesh::DrawType::Draw == static_cast<Mesh::DrawType>(m_meshConfig->drawType)) {
            return hr;
		}

		hr = CreateIndexBuffer(pDevice, indexes);

        return hr;
    }
};