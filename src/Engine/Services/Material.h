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
    void Apply(ID3D11DeviceContext* context) override;
    HRESULT Init() override;
    HRESULT InitManagers();
    HRESULT InitPixelAndVertexShaders();
    HRESULT InitMatrixBuffer();
    HRESULT InitSampleState();
    void Render() override;
    void Update(float deltaTime) override {};
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

    ID3D11InputLayout* GetInputLayout() {
        return inputLayout;
    }

    void SetShaderName(const std::wstring& shaderName) {
        m_shaderName = shaderName;
	}
    
    // Método para actualizar el contenido del buffer de matrices y vincularlo
    // Ahora toma las tres matrices por separado.	
    
    ID3D11VertexShader* GetVertexShader() const { return vertexShader; }
    ID3D11PixelShader* GetPixelShader() const { return pixelShader; }

    void SetConstantBuffers(ID3D11DeviceContext* context, MatrixDefinitionBase::MatrixParams matrixParams, int slot = 0);

    ID3D11ShaderResourceView* LoadTextureFromFile(std::shared_ptr<ID3D11Device> device, const std::wstring& filename);

    PBRTextures textures;

    ID3D11Buffer* m_matrixBuffer; // Miembro para el buffer de matrices
private:
    std::shared_ptr<DeviceManager> m_deviceManager; // Usamos shared_ptr para compartir la instancia de ShaderManager
    std::shared_ptr<ShaderManager> m_shaderManager; // Usamos shared_ptr para compartir la instancia de ShaderManager
    std::shared_ptr<CameraManager> m_cameraManager; // Usamos shared_ptr para compartir la instancia de ShaderManager
	std::wstring m_shaderName;

    ID3D11ShaderResourceView* m_texture_albedo;
    ID3D11ShaderResourceView* m_texture_normal;
    ID3D11ShaderResourceView* m_texture_roughness;
    ID3D11ShaderResourceView* m_texture_metallic;
    ID3D11ShaderResourceView* m_texture_ao;
    ID3D11ShaderResourceView* m_texture;

    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* inputLayout;
    ID3D11SamplerState* m_samplerState;
    // Aquí guardamos la estrategia actual.
    // std::unique_ptr es ideal porque Material es el dueño de la estrategia.
    std::unique_ptr<ITextureInitializer> m_initializerStrategy;

	std::map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>> m_constantBuffers; // Vector para almacenar los buffers de constantes

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pLightConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pCameraConstantBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_pMaterialConstantBuffer;
};