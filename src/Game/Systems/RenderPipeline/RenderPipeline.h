#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h> // Para ComPtr
#include <vector>
#include <IService.h>

// Forward declarations de las clases de etapa
namespace RenderPipeline
{
    class InputAssemblyStage;
    class VertexShaderStage;
    class PixelShaderStage;
    class RasterizerStage;
    class OutputMergerStage;

    // Estructura simplificada para datos globales/por pase (ej. matrices)
    struct GlobalPassConstants
    {
        DirectX::XMMATRIX LightViewProjection;
        DirectX::XMMATRIX CameraViewProjection;
        DirectX::XMMATRIX WorldMatrix; // Se actualizaría por objeto/instancia
        // Otros datos como dirección de luz, color, tiempo, etc.
    };

    // Recursos D3D que el pipeline podría necesitar o gestionar
    struct PipelineResources
    {
        // Usamos ComPtr para la gestión automática de memoria de COM
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> ShadowRS;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> MainRS;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> ShadowDSS;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> MainDSS;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> DepthVS;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> DepthIL; // InputLayout para el VS de profundidad
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShadowMapSRV;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> ShadowMapSampler;

        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> MainRTV;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> MainDSV;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> ShadowMapDSV; // DSV dedicado para el mapa de sombras

        // Constructor/Destructor: ComPtr se encarga de la liberación
        PipelineResources() = default;
        ~PipelineResources() = default;
    };
}

namespace RenderPipeline // Declaración de la clase principal RenderPipeline
{
    class Renderer: public IService
    {
    public:
        Renderer() {};
        Renderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11Device> device);
        ~Renderer();

		// Herencias de IService
        HRESULT Init() override { return S_OK; };
        void Render() override {};
		void Update(float deltaTime) override {};
		void Shutdown() override {};

        const std::string& GetServiceName() const override {
            static const std::string name = "Renderer";
            return name;
        }
        static const std::string& GetStaticServiceName()
        {
            static const std::string name = "Renderer";
            return name;
        }


        // --- Pase de Sombras ---
        void BeginShadowPass(const DirectX::XMMATRIX& lightViewProjection);
        void DrawObjectForShadow(Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer, UINT indexCount, const DirectX::XMMATRIX& worldMatrix);
        void DrawInstancedObjectsForShadow(Microsoft::WRL::ComPtr<ID3D11Buffer> baseVertexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> baseIndexBuffer, UINT indexCountPerInstance, const std::vector<DirectX::XMMATRIX>& instanceWorldMatrices);
        void EndShadowPass();

        // --- Pase Principal ---
        void BeginMainPass(const DirectX::XMMATRIX& cameraViewProjection);
        void DrawObject(Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer, UINT indexCount,
            Microsoft::WRL::ComPtr<ID3D11VertexShader> vs, Microsoft::WRL::ComPtr<ID3D11PixelShader> ps,
            Microsoft::WRL::ComPtr<ID3D11InputLayout> il, const DirectX::XMMATRIX& worldMatrix,
            Microsoft::WRL::ComPtr<ID3D11Buffer> materialCB, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> albedoSRV);
        void DrawInstancedObjects(Microsoft::WRL::ComPtr<ID3D11Buffer> baseVertexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> baseIndexBuffer, UINT indexCountPerInstance,
            Microsoft::WRL::ComPtr<ID3D11VertexShader> vs, Microsoft::WRL::ComPtr<ID3D11PixelShader> ps,
            Microsoft::WRL::ComPtr<ID3D11InputLayout> il, const std::vector<DirectX::XMMATRIX>& instanceWorldMatrices,
            Microsoft::WRL::ComPtr<ID3D11Buffer> materialCB, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> albedoSRV);
        void EndMainPass();

    private:
        // Las instancias de ComPtr son propietarias del contexto/dispositivo si se crearon aquí.
        // Si se pasan desde fuera (como en un renderer principal), se deben gestionar cuidadosamente
        // para que no se liberen prematuramente. Aquí los tratamos como referencias fuertes.
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
        Microsoft::WRL::ComPtr<ID3D11Device> m_device;

        // Instancias de las clases de etapa
        InputAssemblyStage* m_iaStage;
        VertexShaderStage* m_vsStage;
        PixelShaderStage* m_psStage;
        RasterizerStage* m_rsStage;
        OutputMergerStage* m_omStage;

        // Recursos D3D manejados por el Renderer
        PipelineResources m_resources;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_globalPassCB; // Constant Buffer para GlobalPassConstants
    };
}