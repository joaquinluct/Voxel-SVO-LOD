#include "RenderPipeline.h"
#include <iostream>
#include "Pipeline/InputAssemblyStage.h"
#include "Pipeline/VertexShaderStage.h"
#include "Pipeline/PixelShaderStage.h"
#include "Pipeline/RasterizerStage.h"
#include "Pipeline/OutputMergerStage.h"
#include <REGISTER_SERVICE_MACRO.h>


// Include para mocks si no se usa un entorno D3D real para compilar/ejecutar este ejemplo
// #include "D3DGraphicsMocks.h" // Si se usa el mock ID3D11DeviceContextMock

namespace RenderPipeline
{
    REGISTER_SERVICE_TYPE(Renderer, "Renderer")
    // Helper function to create a simple Constant Buffer (DUMMY for this example)
    Microsoft::WRL::ComPtr<ID3D11Buffer> CreateConstantBuffer(Microsoft::WRL::ComPtr<ID3D11Device> device, size_t byteSize)
    {
        Microsoft::WRL::ComPtr<ID3D11Buffer> cb;
        if (!device) {
            std::cout << "[D3D] Device not provided, returning null ComPtr for CB.\n";
            return nullptr;
        }

        // --- REAL D3D CODE EXAMPLE (commented out) ---
        // D3D11_BUFFER_DESC cbDesc = {};
        // cbDesc.ByteWidth = static_cast<UINT>(byteSize);
        // cbDesc.Usage = D3D11_USAGE_DYNAMIC;
        // cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        // cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        // cbDesc.MiscFlags = 0;
        // cbDesc.StructureByteStride = 0;
        // HRESULT hr = device->CreateBuffer(&cbDesc, nullptr, cb.GetAddressOf());
        // if (FAILED(hr)) {
        //     std::cerr << "[D3D] Failed to create constant buffer! HRESULT: " << hr << "\n";
        //     return nullptr;
        // }
        // ------------------------------------------------

        std::cout << "[D3D] Created dummy (null) ComPtr for Constant Buffer.\n";
        return cb; // Will be nullptr for this mock
    }

    // Constructor del RenderPipeline (Renderer)
    Renderer::Renderer(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<ID3D11Device> device)
        : m_context(context),
        m_device(device),
        m_iaStage(new InputAssemblyStage(context)),
        m_vsStage(new VertexShaderStage(context)),
        m_psStage(new PixelShaderStage(context)),
        m_rsStage(new RasterizerStage(context)),
        m_omStage(new OutputMergerStage(context))
    {
        m_globalPassCB = CreateConstantBuffer(m_device, sizeof(GlobalPassConstants));

        // En un escenario real, aquí crearías tus recursos D3D iniciales (shaders, estados, etc.)
        // Usando m_device->Create* y asignando a m_resources.ComPtr.
        // Ejemplo para un Recurso D3D real (comentado):
        // HRESULT hr = m_device->CreateRasterizerState(&desc, m_resources.MainRS.GetAddressOf());
        // if (FAILED(hr)) { /* handle error */ }

        std::cout << "[RenderPipeline::Renderer] Initialized pipeline stages and created dummy global CB.\n";
    }

    Renderer::~Renderer()
    {
        // Los objetos ComPtr (m_context, m_device, m_globalPassCB, m_resources)
        // se liberan automáticamente. Solo necesitamos eliminar las instancias de clases de etapa.
        delete m_iaStage;
        delete m_vsStage;
        delete m_psStage;
        delete m_rsStage;
        delete m_omStage;

        std::cout << "[RenderPipeline::Renderer] Destroyed pipeline stages.\n";
    }

    // --- Fase de Sombras ---
    void Renderer::BeginShadowPass(const DirectX::XMMATRIX& lightViewProjection)
    {
        std::cout << "\n--- BEGIN SHADOW PASS ---\n";

        // 1. Configuración del Output Merger (OM) - ¡Lo primero lógicamente!
        m_omStage->SetRenderTargets(0, nullptr, m_resources.ShadowMapDSV.Get());
        m_omStage->ClearDepthStencilView(m_resources.ShadowMapDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        m_omStage->SetDepthStencilState(m_resources.ShadowDSS, 0);

        // 2. Configuración del Rasterizer (RS)
        D3D11_VIEWPORT shadowViewport = { 0, 0, 1024, 1024, 0.0f, 1.0f }; // Ejemplo
        m_rsStage->SetViewport(shadowViewport);
        m_rsStage->SetState(m_resources.ShadowRS); // CULL_FRONT, DepthBias

        // 3. Configuración de Shaders (VS, PS)
        m_vsStage->SetShader(m_resources.DepthVS);
        m_psStage->SetShader(nullptr); // No PS

        // 4. Configuración del Input Assembly (IA)
        m_iaStage->SetInputLayout(m_resources.DepthIL);
        m_iaStage->SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // 5. Actualizar y Bindear Constant Buffers globales del pase (LightViewProjection)
        GlobalPassConstants passData;
        passData.LightViewProjection = DirectX::XMMatrixTranspose(lightViewProjection); // Transpose for HLSL
        m_vsStage->UpdateGlobalPassConstants(m_context, m_globalPassCB, passData);

        ID3D11Buffer* const vsCBs[] = { m_globalPassCB.Get() };
        m_vsStage->SetConstantBuffers(0, 1, vsCBs);
    }

    void Renderer::DrawObjectForShadow(Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer, UINT indexCount, const DirectX::XMMATRIX& worldMatrix)
    {
        std::cout << "[Shadow Pass] Drawing object independently.\n";

        GlobalPassConstants currentData;
        // En un motor real, leerías el CB actual para no sobrescribir LightViewProjection
        // Por simplicidad, aquí creamos una nueva estructura y sobrescribimos el CB completo.
        currentData.WorldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
        m_vsStage->UpdateGlobalPassConstants(m_context, m_globalPassCB, currentData);

        // Bindear la geometría del objeto
        UINT stride = sizeof(float) * 6; // Ejemplo: PosXYZ + NormalXYZ
        UINT offset = 0;
        //ID3D11Buffer* const vbs[] = { vertexBuffer.Get() };
        m_iaStage->SetVertexBuffers(0, 1, vertexBuffer.Get(), &stride, &offset);
        m_iaStage->SetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

        // Realizar el Draw Call
        if (m_context) m_context->DrawIndexed(indexCount, 0, 0);
    }

    void Renderer::DrawInstancedObjectsForShadow(Microsoft::WRL::ComPtr<ID3D11Buffer> baseVertexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> baseIndexBuffer, UINT indexCountPerInstance, const std::vector<DirectX::XMMATRIX>& instanceWorldMatrices)
    {
        std::cout << "[Shadow Pass] Drawing instanced objects.\n";

        Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer = CreateConstantBuffer(m_device, sizeof(DirectX::XMMATRIX) * instanceWorldMatrices.size());

        // --- REAL D3D CODE EXAMPLE ---
        // if (instanceBuffer && m_context) {
        //     D3D11_MAPPED_SUBRESOURCE mapped;
        //     m_context->Map(instanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        //     memcpy(mapped.pData, instanceWorldMatrices.data(), sizeof(DirectX::XMMATRIX) * instanceWorldMatrices.size());
        //     m_context->Unmap(instanceBuffer.Get(), 0);
        // }
        // -----------------------------

        UINT meshStride = sizeof(float) * 6;
        UINT meshOffset = 0;
        UINT instanceStride = sizeof(DirectX::XMMATRIX);
        UINT instanceOffset = 0;

        /*ID3D11Buffer* const vbs[] = { baseVertexBuffer.Get(), instanceBuffer.Get() };
        UINT strides[] = { meshStride, instanceStride };
        UINT offsets[] = { meshOffset, instanceOffset };
        m_iaStage->SetVertexBuffers(0, 2, vbs, strides, offsets);
        m_iaStage->SetIndexBuffer(baseIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);*/

        if (m_context) m_context->DrawIndexedInstanced(indexCountPerInstance, (UINT)instanceWorldMatrices.size(), 0, 0, 0);
    }


    void Renderer::EndShadowPass()
    {
        std::cout << "--- END SHADOW PASS ---\n";
        m_vsStage->SetShader(nullptr);
        m_psStage->SetShader(nullptr);
        m_iaStage->SetInputLayout(nullptr);

        ID3D11Buffer* const nullCBs[] = { nullptr };
        m_vsStage->SetConstantBuffers(0, 1, nullCBs);

        ID3D11ShaderResourceView* const nullSRVs[] = { nullptr };
        m_psStage->SetShaderResources(0, 1, nullSRVs);

        D3D11_VIEWPORT mainViewport = { 0, 0, 1280, 720, 0.0f, 1.0f };
        m_rsStage->SetViewport(mainViewport);
        m_rsStage->SetState(m_resources.MainRS);
    }

    // --- Fase Principal ---
    void Renderer::BeginMainPass(const DirectX::XMMATRIX& cameraViewProjection)
    {
        std::cout << "\n--- BEGIN MAIN PASS ---\n";

        m_omStage->SetRenderTargets(1, m_resources.MainRTV.GetAddressOf(), m_resources.MainDSV.Get());
        float clearColor[] = { 0.1f, 0.2f, 0.3f, 1.0f };
        m_omStage->ClearRenderTargetView(m_resources.MainRTV, clearColor);
        m_omStage->ClearDepthStencilView(m_resources.MainDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        m_omStage->SetDepthStencilState(m_resources.MainDSS, 0);
        m_omStage->SetBlendState(nullptr, new float[4] {0, 0, 0, 0}, 0xFFFFFFFF);

        D3D11_VIEWPORT mainViewport = { 0, 0, 1280, 720, 0.0f, 1.0f };
        m_rsStage->SetViewport(mainViewport);
        m_rsStage->SetState(m_resources.MainRS);

        GlobalPassConstants passData;
        passData.CameraViewProjection = DirectX::XMMatrixTranspose(cameraViewProjection);
        m_vsStage->UpdateGlobalPassConstants(m_context, m_globalPassCB, passData);

        ID3D11Buffer* const vsCBs[] = { m_globalPassCB.Get() };
        m_vsStage->SetConstantBuffers(0, 1, vsCBs);

        ID3D11ShaderResourceView* const shadowSRVs[] = { m_resources.ShadowMapSRV.Get() };
        m_psStage->SetShaderResources(1, 1, shadowSRVs);
        ID3D11SamplerState* const shadowSamplers[] = { m_resources.ShadowMapSampler.Get() };
        m_psStage->SetSamplers(1, 1, shadowSamplers);
    }

    void Renderer::DrawObject(Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer, UINT indexCount,
        Microsoft::WRL::ComPtr<ID3D11VertexShader> vs, Microsoft::WRL::ComPtr<ID3D11PixelShader> ps,
        Microsoft::WRL::ComPtr<ID3D11InputLayout> il, const DirectX::XMMATRIX& worldMatrix,
        Microsoft::WRL::ComPtr<ID3D11Buffer> materialCB, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> albedoSRV)
    {
        std::cout << "[Main Pass] Drawing object independently.\n";

        GlobalPassConstants currentData;
        currentData.WorldMatrix = DirectX::XMMatrixTranspose(worldMatrix);
        m_vsStage->UpdateGlobalPassConstants(m_context, m_globalPassCB, currentData);

        m_vsStage->SetShader(vs);
        m_psStage->SetShader(ps);
        m_iaStage->SetInputLayout(il);

        ID3D11Buffer* const psCBs[] = { materialCB.Get() };
        m_psStage->SetConstantBuffers(0, 1, psCBs);

        ID3D11ShaderResourceView* const psSRVs[] = { albedoSRV.Get() };
        m_psStage->SetShaderResources(0, 1, psSRVs);

        UINT stride = sizeof(float) * 6;
        UINT offset = 0;
        m_iaStage->SetVertexBuffers(0, 1, vertexBuffer.Get(), &stride, &offset);
        m_iaStage->SetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

        if (m_context) m_context->DrawIndexed(indexCount, 0, 0);
    }

    void Renderer::DrawInstancedObjects(Microsoft::WRL::ComPtr<ID3D11Buffer> baseVertexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer> baseIndexBuffer, UINT indexCountPerInstance,
        Microsoft::WRL::ComPtr<ID3D11VertexShader> vs, Microsoft::WRL::ComPtr<ID3D11PixelShader> ps,
        Microsoft::WRL::ComPtr<ID3D11InputLayout> il, const std::vector<DirectX::XMMATRIX>& instanceWorldMatrices,
        Microsoft::WRL::ComPtr<ID3D11Buffer> materialCB, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> albedoSRV)
    {
        std::cout << "[Main Pass] Drawing instanced objects.\n";

        m_vsStage->SetShader(vs);
        m_psStage->SetShader(ps);
        m_iaStage->SetInputLayout(il);

        ID3D11Buffer* const psCBs[] = { materialCB.Get() };
        m_psStage->SetConstantBuffers(0, 1, psCBs);

        ID3D11ShaderResourceView* const psSRVs[] = { albedoSRV.Get() };
        m_psStage->SetShaderResources(0, 1, psSRVs);

        Microsoft::WRL::ComPtr<ID3D11Buffer> instanceBuffer = CreateConstantBuffer(m_device, sizeof(DirectX::XMMATRIX) * instanceWorldMatrices.size());
        // Map and copy instance matrices (in a real scenario)

        UINT meshStride = sizeof(float) * 6;
        UINT meshOffset = 0;
        UINT instanceStride = sizeof(DirectX::XMMATRIX);
        UINT instanceOffset = 0;

        /*ID3D11Buffer* const vbs[] = { baseVertexBuffer.Get(), instanceBuffer.Get() };
        UINT strides[] = { meshStride, instanceStride };
        UINT offsets[] = { meshOffset, instanceOffset };
        m_iaStage->SetVertexBuffers(0, 2, vbs, strides, offsets);
        m_iaStage->SetIndexBuffer(baseIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);*/

        if (m_context) m_context->DrawIndexedInstanced(indexCountPerInstance, (UINT)instanceWorldMatrices.size(), 0, 0, 0);
    }

    void Renderer::EndMainPass()
    {
        std::cout << "--- END MAIN PASS ---\n";
        m_vsStage->SetShader(nullptr);
        m_psStage->SetShader(nullptr);
        m_iaStage->SetInputLayout(nullptr);

        ID3D11Buffer* const nullCBs[] = { nullptr };
        m_vsStage->SetConstantBuffers(0, 1, nullCBs);
        m_psStage->SetConstantBuffers(0, 1, nullCBs);
        ID3D11ShaderResourceView* const nullSRVs[] = { nullptr };
        m_psStage->SetShaderResources(0, 2, nullSRVs);
        ID3D11SamplerState* const nullSamplers[] = { nullptr };
        m_psStage->SetSamplers(0, 2, nullSamplers);

        m_omStage->SetRenderTargets(0, nullptr, nullptr);
    }
}