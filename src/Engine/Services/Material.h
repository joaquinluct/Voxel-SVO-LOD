#pragma once
#include <DirectXMath.h> // Para XMMATRIX
#include <d3d11.h>
#include <DirectX/DirectXTex/DirectXTex.h>
#include <wincodec.h> // Para Windows Imaging Component (WIC)
#include <wrl.h>
#include <iostream>
#include <type_traits> // Para std::is_same y std::is_same_v
#include "IMaterial.h"
#include "DeviceManager.h"
#include "ShaderManager.h"
#include "CameraManager.h"
#include "KeyboardManager.h"
#include "RenderManager/RenderManager.h"
#include <Game/Systems/Lighting.h>
#include <Game/Systems/Shadows.h>
#include <Util/Text/Text.h>
#include "Texture.h"
#include "ITextureInitializer.h"
#include <Defines/VertexDefinition.h>
#include <Defines/MatrixDefinitionBase.h>
#include <IService.h>

struct ID3D11ShaderResourceViewReleaser {
    void operator()(ID3D11ShaderResourceView* ptr) const {
        if (ptr) {
            ptr->Release(); // Llama a Release() en lugar de delete
        }
    }
};


class Material : public IMaterial, public IService {
public:
    Material();
    ~Material() override {};
    void SetTexture(ID3D11ShaderResourceView* texture, std::string textureMap) override;
    void Apply(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) override;
    HRESULT Init() override;
    HRESULT InitManagers();
    HRESULT InitPixelAndVertexShaders();
    HRESULT InitMatrixBuffer();
    HRESULT InitSampleState();
    void Render() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    

    const std::string& GetServiceName() const override {
        static const std::string name = "Material";
        return name;
    }
    static const std::string& GetStaticServiceName()
    {
        static const std::string name = "Material";
        return name;
    }

    Microsoft::WRL::ComPtr<ID3D11InputLayout> GetInputLayout() {
        return inputLayout;
    }

    Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState() {
        return m_samplerState;
    }

    void SetShaderName(const std::wstring& shaderName) {
        m_shaderName = shaderName;
	}
    
    // Método para actualizar el contenido del buffer de matrices y vincularlo
    // Ahora toma las tres matrices por separado.	
    
    Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader() const { return vertexShader; }
    Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader() const { return pixelShader; }

    UINT GetNumTextures() {
        UINT count = 0;
        count += (m_texture_albedo != nullptr); // Asumiendo que m_texture_albedo es un puntero o similar
        count += (m_texture_normal != nullptr);
        count += (m_texture_roughness != nullptr);
        count += (m_texture_metallic != nullptr);
        count += (m_texture_ao != nullptr);
        count += (m_texture != nullptr);
        return count;
    }

    std::map<std::string, ID3D11ShaderResourceView*> GetTextures() {
        std::map<std::string, ID3D11ShaderResourceView*> textures = {};
        if (m_texture_albedo) {
            textures[TEXTURE_MAP_ALBEDO.data()] = m_texture_albedo;
        }
        if (m_texture_normal) {
            textures[TEXTURE_MAP_NORMAL.data()] = m_texture_normal;
        }
        if (m_texture_roughness) {
            textures[TEXTURE_MAP_ROUGHNESS.data()] = m_texture_roughness;
        }
        if (m_texture_metallic) {
            textures[TEXTURE_MAP_METALLIC.data()] = m_texture_metallic;
        }
        if (m_texture_ao) {
            textures[TEXTURE_MAP_AO.data()] = m_texture_ao;
        }
        return textures;
    }

    void SetConstantBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, MatrixDefinitionBase::MatrixParams matrixParams, int slot = 0);

    ID3D11ShaderResourceView* LoadTextureFromFile(std::shared_ptr<ID3D11Device> device, const std::wstring& filename);

    PBRTextures textures;

    ID3D11Buffer* m_matrixBuffer; // Miembro para el buffer de matrices
private:
    std::shared_ptr<KeyboardManager> m_keyboard; 
    std::shared_ptr<DeviceManager> m_deviceManager; 
    std::shared_ptr<ShaderManager> m_shaderManager; 
    std::shared_ptr<CameraManager> m_cameraManager;
    std::shared_ptr<RenderManager> m_renderManager;
    std::shared_ptr<Lighting> m_lighting;
    std::shared_ptr<Shadows> m_shadows;
    std::wstring m_shaderName;

    ID3D11ShaderResourceView* m_texture_albedo;
    ID3D11ShaderResourceView* m_texture_normal;
    ID3D11ShaderResourceView* m_texture_roughness;
    ID3D11ShaderResourceView* m_texture_metallic;
    ID3D11ShaderResourceView* m_texture_ao;
    ID3D11ShaderResourceView* m_texture;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
    // Aquí guardamos la estrategia actual.
    // std::unique_ptr es ideal porque Material es el dueño de la estrategia.
    std::unique_ptr<ITextureInitializer> m_initializerStrategy;

	std::map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>> m_constantBuffers; // Vector para almacenar los buffers de constantes

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pLightConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pCameraConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pMaterialConstantBuffer;

	XMFLOAT3 debug_lightDirection = { 0.0f, -1.0f, 0.0f };
};