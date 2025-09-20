#include "ConcreteOperations.h"
#include <cstring>
#include <d3d11.h>

// Implementación de las clases existentes

ClearOperation::ClearOperation(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, const float clearColor[4])
    : m_rtv(rtv), m_dsv(dsv) {
    std::memcpy(m_clearColor, clearColor, sizeof(m_clearColor));
}

void ClearOperation::Execute(ID3D11DeviceContext* context) {
    if (m_rtv) {
        context->ClearRenderTargetView(m_rtv.Get(), m_clearColor);
    }
    if (m_dsv) {
        context->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    }
}

// ... (Resto de las implementaciones de las clases existentes)

SetRenderTargetOperation::SetRenderTargetOperation(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv)
    : m_rtv(rtv), m_dsv(dsv) {
}

void SetRenderTargetOperation::Execute(ID3D11DeviceContext* context) {
    context->OMSetRenderTargets(1, m_rtv.GetAddressOf(), m_dsv.Get());
}

SetViewportOperation::SetViewportOperation(const D3D11_VIEWPORT& viewport)
    : m_viewport(viewport) {
}

void SetViewportOperation::Execute(ID3D11DeviceContext* context) {
    context->RSSetViewports(1, &m_viewport);
}

PresentOperation::PresentOperation(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags)
    : m_swapChain(swapChain), m_syncInterval(syncInterval), m_flags(flags) {
}

void PresentOperation::Execute(ID3D11DeviceContext* context) {
    // Nota: El Present debe ser llamado en el contexto inmediato, no en uno diferido.
    // Esta operación se ejecutará solo en el hilo principal.
    m_swapChain->Present(m_syncInterval, m_flags);
}

SetInputLayoutOperation::SetInputLayoutOperation(ID3D11InputLayout* layout)
    : m_layout(layout) {
}

void SetInputLayoutOperation::Execute(ID3D11DeviceContext* context) {
    context->IASetInputLayout(m_layout.Get());
}

SetPrimitiveTopologyOperation::SetPrimitiveTopologyOperation(D3D_PRIMITIVE_TOPOLOGY topology)
    : m_topology(topology) {
}

void SetPrimitiveTopologyOperation::Execute(ID3D11DeviceContext* context) {
    context->IASetPrimitiveTopology(m_topology);
}

SetVertexBufferOperation::SetVertexBufferOperation(ID3D11Buffer* buffer, UINT stride, UINT offset)
    : m_buffer(buffer), m_stride(stride), m_offset(offset) {
}

void SetVertexBufferOperation::Execute(ID3D11DeviceContext* context) {
    ID3D11Buffer* pBuffer = m_buffer.Get();
    context->IASetVertexBuffers(0, 1, &pBuffer, &m_stride, &m_offset);
}

SetIndexBufferOperation::SetIndexBufferOperation(ID3D11Buffer* buffer, DXGI_FORMAT format, UINT offset)
    : m_buffer(buffer), m_format(format), m_offset(offset) {
}

void SetIndexBufferOperation::Execute(ID3D11DeviceContext* context) {
    context->IASetIndexBuffer(m_buffer.Get(), m_format, m_offset);
}

SetVertexShaderOperation::SetVertexShaderOperation(ID3D11VertexShader* shader)
    : m_shader(shader) {
}

void SetVertexShaderOperation::Execute(ID3D11DeviceContext* context) {
    context->VSSetShader(m_shader.Get(), nullptr, 0);
}

SetPixelShaderOperation::SetPixelShaderOperation(ID3D11PixelShader* shader)
    : m_shader(shader) {
}

void SetPixelShaderOperation::Execute(ID3D11DeviceContext* context) {
    context->PSSetShader(m_shader.Get(), nullptr, 0);
}

SetTextureOperation::SetTextureOperation(const std::vector<ID3D11ShaderResourceView*>& srvs, UINT startSlot)
    : m_startSlot(startSlot) {
    for (auto& srv : srvs) {
        m_srvs.push_back(srv);
    }
}

void SetTextureOperation::Execute(ID3D11DeviceContext* context) {
    context->PSSetShaderResources(m_startSlot, m_srvs.size(), m_srvs.data()->GetAddressOf());
}

SetSamplerOperation::SetSamplerOperation(const std::vector<ID3D11SamplerState*>& samplers, UINT startSlot)
    : m_startSlot(startSlot) {
    for (auto& sampler : samplers) {
        m_samplers.push_back(sampler);
    }
}

void SetSamplerOperation::Execute(ID3D11DeviceContext* context) {
    context->PSSetSamplers(m_startSlot, m_samplers.size(), m_samplers.data()->GetAddressOf());
}

UpdateConstantsBufferOperation::UpdateConstantsBufferOperation(ID3D11Buffer* buffer, const std::vector<char>& data)
    : m_buffer(buffer), m_data(data) {
}

void UpdateConstantsBufferOperation::Execute(ID3D11DeviceContext* context) {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    context->Map(m_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    std::memcpy(mappedResource.pData, m_data.data(), m_data.size());
    context->Unmap(m_buffer.Get(), 0);
}

BindConstantsBuffersOperation::BindConstantsBuffersOperation(const std::vector<ID3D11Buffer*>& buffers, UINT startSlot)
    : m_startSlot(startSlot) {
    for (auto& buffer : buffers) {
        m_buffers.push_back(buffer);
    }
}

void BindConstantsBuffersOperation::Execute(ID3D11DeviceContext* context) {
    std::vector<ID3D11Buffer*> pBuffers;
    for (const auto& buffer : m_buffers) {
        pBuffers.push_back(buffer.Get());
    }
    context->VSSetConstantBuffers(m_startSlot, pBuffers.size(), pBuffers.data());
    context->PSSetConstantBuffers(m_startSlot, pBuffers.size(), pBuffers.data());
}

DrawOperation::DrawOperation(UINT vertexCount, UINT startVertexLocation)
    : m_vertexCount(vertexCount), m_startVertexLocation(startVertexLocation) {
}

void DrawOperation::Execute(ID3D11DeviceContext* context) {
    context->Draw(m_vertexCount, m_startVertexLocation);
}

DrawIndexedOperation::DrawIndexedOperation(UINT indexCount, UINT startIndexLocation, UINT baseVertexLocation)
    : m_indexCount(indexCount), m_startIndexLocation(startIndexLocation), m_baseVertexLocation(baseVertexLocation) {
}

void DrawIndexedOperation::Execute(ID3D11DeviceContext* context) {
    context->DrawIndexed(m_indexCount, m_startIndexLocation, m_baseVertexLocation);
}


// --- Implementaciones de las nuevas operaciones ---

CreateRasterizerStateOperation::CreateRasterizerStateOperation(ID3D11Device* device, const D3D11_RASTERIZER_DESC& desc)
    : m_device(device), m_desc(desc) {
}

void CreateRasterizerStateOperation::Execute(ID3D11DeviceContext* context) {
    if (m_device) {
        HRESULT hr = m_device->CreateRasterizerState(&m_desc, m_state.GetAddressOf());
        if (FAILED(hr)) {
            // Manejar error
        }
    }
}

CreateBlendStateOperation::CreateBlendStateOperation(ID3D11Device* device, const D3D11_BLEND_DESC& desc)
    : m_device(device), m_desc(desc) {
}

void CreateBlendStateOperation::Execute(ID3D11DeviceContext* context) {
    if (m_device) {
        HRESULT hr = m_device->CreateBlendState(&m_desc, m_state.GetAddressOf());
        if (FAILED(hr)) {
            // Manejar error
        }
    }
}

CreateDepthStencilStateOperation::CreateDepthStencilStateOperation(ID3D11Device* device, const D3D11_DEPTH_STENCIL_DESC& desc)
    : m_device(device), m_desc(desc) {
}

void CreateDepthStencilStateOperation::Execute(ID3D11DeviceContext* context) {
    if (m_device) {
        HRESULT hr = m_device->CreateDepthStencilState(&m_desc, m_state.GetAddressOf());
        if (FAILED(hr)) {
            // Manejar error
        }
    }
}

CreateSamplerStateOperation::CreateSamplerStateOperation(ID3D11Device* device, const D3D11_SAMPLER_DESC& desc)
    : m_device(device), m_desc(desc) {
}

void CreateSamplerStateOperation::Execute(ID3D11DeviceContext* context) {
    if (m_device) {
        HRESULT hr = m_device->CreateSamplerState(&m_desc, m_state.GetAddressOf());
        if (FAILED(hr)) {
            // Manejar error
        }
    }
}

CreateConstantBufferOperation::CreateConstantBufferOperation(ID3D11Device* device, UINT size, bool isDynamic)
    : m_device(device), m_size(size), m_isDynamic(isDynamic) {
}

void CreateConstantBufferOperation::Execute(ID3D11DeviceContext* context) {
    if (m_device) {
        D3D11_BUFFER_DESC bufferDesc = {};
        bufferDesc.ByteWidth = m_size; // Suponiendo que 'size' es un múltiplo de 16
        bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bufferDesc.Usage = m_isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
        bufferDesc.CPUAccessFlags = m_isDynamic ? D3D11_CPU_ACCESS_WRITE : 0;

        HRESULT hr = m_device->CreateBuffer(&bufferDesc, nullptr, m_buffer.GetAddressOf());
        if (FAILED(hr)) {
            // Manejar error
        }
    }
}

CreateDepthStencilViewOperation::CreateDepthStencilViewOperation(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& textureDesc, const D3D11_DEPTH_STENCIL_VIEW_DESC& viewDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc)
    : m_device(device), m_textureDesc(textureDesc), m_viewDesc(viewDesc), m_srvDesc(srvDesc) {
}

void CreateDepthStencilViewOperation::Execute(ID3D11DeviceContext* context) {
    if (m_device) {
        HRESULT hr = m_device->CreateTexture2D(&m_textureDesc, nullptr, m_texture.GetAddressOf());
        if (SUCCEEDED(hr)) {
            hr = m_device->CreateDepthStencilView(m_texture.Get(), &m_viewDesc, m_dsv.GetAddressOf());
            if (SUCCEEDED(hr) && m_srvDesc.Format != DXGI_FORMAT_UNKNOWN) {
                hr = m_device->CreateShaderResourceView(m_texture.Get(), &m_srvDesc, m_srv.GetAddressOf());
            }
        }
        if (FAILED(hr)) {
            // Manejar error
        }
    }
}

CreateRenderTargetsViewOperation::CreateRenderTargetsViewOperation(ID3D11Device* device, ID3D11Texture2D* backBuffer)
    : m_device(device), m_backBuffer(backBuffer) {
}

void CreateRenderTargetsViewOperation::Execute(ID3D11DeviceContext* context) {
    if (m_device && m_backBuffer) {
        HRESULT hr = m_device->CreateRenderTargetView(m_backBuffer.Get(), nullptr, m_rtv.GetAddressOf());
        if (FAILED(hr)) {
            // Manejar error
        }
    }
}
