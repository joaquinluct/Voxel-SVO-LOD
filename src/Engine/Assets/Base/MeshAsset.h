// MeshAsset.h
#pragma once
#include <Config/Asset/SkyboxCubeConfig.h>
#include <Material/includes/VertexDefinition.h>
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
#include <../RenderObjects/Axis/Axis.h>
#include <../Camera/FirstPersonCamera.h>

class MeshAsset : public IAssetMesh {
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

    void SetType(const std::string& type);
    // Métodos específicos de MeshAsset
	template <typename T>
    HRESULT InitD3D11Resources(ID3D11Device* pDevice,
        const std::vector<T>& vertices,
        const std::vector<WORD>& indices);

    // Getters para los buffers
    ID3D11Buffer* GetVertexBuffer() const { return m_vertexBuffer.Get(); }
    ID3D11Buffer* GetIndexBuffer() const { return m_indexBuffer.Get(); }
    UINT GetIndexCount() const { return m_indexCount; }
    UINT GetVertexCount() const { return m_vertexCount; }

	void SetTexture(const std::string& textureName);
    void SetShaderName(const std::string& shaderName) { m_shaderName = shaderName; };
    void PrepareViewMatrix(ID3D11DeviceContext* context);

private:
    //Axis* m_axis;
    std::shared_ptr<KeyboardManager> m_keyboardrManager;
    std::shared_ptr<ShaderManager> m_shaderManager;
    std::shared_ptr<WorldMatrixManager> m_wMatrixManager;
    std::shared_ptr<CameraManager> m_cameraManager;
	std::shared_ptr<DeviceManager> m_deviceManager;

    std::shared_ptr<ICamera> m_camera;
    Material* m_material;

    std::string m_vertextType = "";
    IAssetMeshConfigBase* m_config;

	std::string m_shaderName;

    ID3D11VertexShader* m_vertexShader = nullptr;
    ID3D11PixelShader* m_pixelShader = nullptr;
    ID3D11InputLayout* m_inputLayout;

    template <typename T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;

    std::string m_name;
    ComPtr<ID3D11Buffer> m_vertexBuffer;
    ComPtr<ID3D11Buffer> m_indexBuffer;
    UINT m_vertexCount;
    UINT m_indexCount;

    // Evitar copias
    MeshAsset(const MeshAsset&) = delete;
    MeshAsset& operator=(const MeshAsset&) = delete;
};