#pragma once

#include "StageBase.h"
#include <d3d11.h>
#include <wrl/client.h> // Para ComPtr

namespace RenderPipeline
{
    class VertexShaderStage : StageBase
    {
    public:
        VertexShaderStage(std::shared_ptr<DeviceManager>deviceManager);
        ~VertexShaderStage(); // Necesario si la etapa gestiona recursos D3D internos no compartidos

        void SetShader(Microsoft::WRL::ComPtr<ID3D11VertexShader> pVertexShader);
        void SetConstantBuffers(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppConstantBuffers);

    private:
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    };
}
