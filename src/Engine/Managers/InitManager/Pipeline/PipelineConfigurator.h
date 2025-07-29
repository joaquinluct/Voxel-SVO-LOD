#pragma once
#include <DirectXMath.h>
#include <dxgi.h>
#include <memory>
#include <wrl/client.h>
#include <Defines/Pipeline.h> // Incluimos tus definiciones

class PipelineConfigurator {
private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;

    // Aquí podrías almacenar referencias a otros managers para operaciones específicas,
    // como un ShaderManager, MeshManager, etc.
public:
    // Constructor que recibe los punteros base de DirectX.
    PipelineConfigurator(
        Microsoft::WRL::ComPtr<ID3D11Device> device,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
        Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain);

    // Método para ejecutar una operación de inicialización.
    HRESULT ExecuteInitOperation(PipelineOperation& operation);
};