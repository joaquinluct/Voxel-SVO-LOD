#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h> // Para Microsoft::WRL::ComPtr
#include <memory> // Para shared_ptr
#include <IService.h>
//#include <ShaderManager.h>
//#include <DeviceManager.h>
#include <Util/DirectXUtils.h>
#include <Defines/Matrix/MatrixDefinition.h>
#include <Defines/Asset.h>
#include <Defines/Texture.h>

class ShaderManager;
class DeviceManager;
//class MatrixDefinition;

// Dimensiones del mapa de sombras (pueden ser configurables)
#define SHADOW_MAP_WIDTH 2048
#define SHADOW_MAP_HEIGHT 2048

// Estructura del Constant Buffer para las matrices del espacio de luz
// Debe coincidir con la definición de tu cbuffer en HLSL (ej: LightSpaceMatrices : register(b0))
// Asegúrate del padding si es necesario (matrices son ya múltiplos de 16 bytes)
struct LightSpaceMatricesData {
    DirectX::XMMATRIX worldMatrix;         // La matriz de mundo del objeto actual
    DirectX::XMMATRIX lightViewProjectionMatrix; // La matriz ViewProjection de la luz para el pase
};


class Shadows : public IService {
public:
    Shadows();
    ~Shadows() override;

    // Métodos del ciclo de vida del servicio
    HRESULT Init() override;
    void Update(float deltaTime) override;
	void Render() override {} // No hay renderizado directo, se hace en el pase de sombras
    void Shutdown() override;

    // Implementación de IService
    const std::string& GetServiceName() const override {
        static const std::string name = "Shadows";
        return name;
    }
    static const std::string& GetStaticServiceName() {
        static const std::string name = "Shadows";
        return name;
    }
    
    // Métodos específicos de Shadows
    // Calcula las matrices de la luz y las almacena en la CPU
    void UpdateLightMatrices(const DirectX::XMFLOAT3& lightDirection, const DirectX::XMFLOAT3& sceneCenter, float sceneRadius);
    
    // Actualiza solo la worldMatrix en el Constant Buffer de luz en la GPU
    void UpdateWorldMatrixInLightCB(const DirectX::XMMATRIX& worldMatrix);

    // Métodos para que otros shaders puedan acceder al mapa de sombras
    ID3D11ShaderResourceView* GetShadowMapSRV() const { return m_shadowMapSRV.Get(); }
    ID3D11SamplerState* GetShadowMapSamplerState() const { return m_shadowMapSamplerState.Get(); }

    // Accesor para la matriz ViewProjection de la luz (puede ser útil para debug o para otros cálculos)
    const DirectX::XMMATRIX& GetLightViewProjectionMatrix() const { return m_lightViewProjectionMatrix; }

private:
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<ShaderManager> m_shaderManager;

    // Recursos del mapa de sombras
    Microsoft::WRL::ComPtr<ID3D11Texture2D>          m_shadowMapTexture2D;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>   m_shadowMapDSV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowMapSRV;
    Microsoft::WRL::ComPtr<ID3D11SamplerState>       m_shadowMapSamplerState; // Sampler de comparación

    // Shaders y Input Layout para el pase de sombras
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_depthVS;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_depthPS; // Puede ser nullptr
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_depthInputLayout;

    // Constant Buffer para las matrices en el espacio de luz
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_lightSpaceMatricesCB;
    LightSpaceMatricesData               m_lightSpaceData; // Copia en CPU de los datos del CB

    // Matrices de la luz (almacenadas en CPU)
    DirectX::XMMATRIX m_lightViewMatrix;
    DirectX::XMMATRIX m_lightProjectionMatrix;
    DirectX::XMMATRIX m_lightViewProjectionMatrix; // View * Projection

    // Estado del render target original para restaurar
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_oldRTV;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_oldDSV;
    D3D11_VIEWPORT                                 m_oldViewport;
    UINT                                           m_numViewports; // Puede ser 1 normalmente, pero se guarda por si acaso

    // Opcional: Rasterizer State para el pase de sombras (ej. Cull Front)
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_shadowRasterizerState;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_oldRasterizerState; // Para guardar y restaurar
};