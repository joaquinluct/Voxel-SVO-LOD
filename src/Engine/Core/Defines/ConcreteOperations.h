#pragma once

#include <d3d11.h>
#include <dxgi.h>
#include <vector>
#include <wrl/client.h>

class FrameStateService;
class Material;

// --- 1. Enumeración de tipos de operaciones ---
// Identifica cada tipo de comando en el pipeline de renderizado.
enum class PipelineOperationType {
    // Operaciones del Dispositivo
    Device_Clear,
    Device_SetRenderTarget,
    Device_SetViewport,
    Device_Present,

    // Operaciones de Malla y Búferes
    Mesh_SetInputLayout,
    Mesh_SetPrimitiveTopology,
    Mesh_SetVertexBuffer,
    Mesh_SetIndexBuffer,
    Mesh_SetVertexShader,
    Mesh_SetPixelShader,
    Mesh_SetTexture,
    Mesh_SetSampler,

    // Operaciones de Búferes de Constantes
    Constants_UpdateBuffer,
    Constants_BindBuffers,

    // Operaciones de Dibujo
    Draw_Draw,
    Draw_DrawIndexed,
    Draw_DrawInstanced,

    // --- Nuevas Operaciones ---
    // Operaciones de Creación de Recursos
    Resource_CreateBackBuffer,
    Resource_CreateRasterizerState,
    Resource_CreateBlendState,
    Resource_CreateDepthStencilState,
    Resource_CreateDepthStencilView,
    Resource_CreateViewport,
    Resource_CreateSamplerState,
    Resource_CreateRenderTargetsView,
    Resource_CreateConstantBuffer,
};

// --- 2. Clase base abstracta para todas las operaciones ---
// Define la interfaz común para todas las operaciones del pipeline.
class PipelineOperation {
public:
    virtual ~PipelineOperation() = default;

    // El método Execute es el corazón de la operación.
    // Recibe el contexto de Direct3D para ejecutar el comando.
    virtual void Execute(ID3D11DeviceContext* context) = 0;
};

// --- 3. Declaraciones de clases de operaciones concretas ---
// Cada clase encapsula la lógica, los datos y los recursos necesarios para un comando específico.

// (Resto de las clases ya existentes... ClearOperation, SetRenderTargetOperation, etc.)

// Limpia el render target y el depth/stencil view.
class ClearOperation : public PipelineOperation {
public:
    ClearOperation(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv, const float clearColor[4]);
    void Execute(ID3D11DeviceContext* context) override;
private:
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;
    float m_clearColor[4];
};

// Vincula un render target y un depth/stencil view.
class SetRenderTargetOperation : public PipelineOperation {
public:
    SetRenderTargetOperation(ID3D11RenderTargetView* rtv, ID3D11DepthStencilView* dsv);
    void Execute(ID3D11DeviceContext* context) override;
private:
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;
};

// Establece el viewport de renderizado.
class SetViewportOperation : public PipelineOperation {
public:
    SetViewportOperation(const D3D11_VIEWPORT& viewport);
    void Execute(ID3D11DeviceContext* context) override;
private:
    D3D11_VIEWPORT m_viewport;
};

// Presenta el back buffer en el swap chain.
class PresentOperation : public PipelineOperation {
public:
    PresentOperation(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags);
    void Execute(ID3D11DeviceContext* context) override;
private:
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
    UINT m_syncInterval;
    UINT m_flags;
};

// Establece el input layout de los vértices.
class SetInputLayoutOperation : public PipelineOperation {
public:
    SetInputLayoutOperation(ID3D11InputLayout* layout);
    void Execute(ID3D11DeviceContext* context) override;
private:
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_layout;
};

// Establece la topología de la primitiva.
class SetPrimitiveTopologyOperation : public PipelineOperation {
public:
    SetPrimitiveTopologyOperation(D3D_PRIMITIVE_TOPOLOGY topology);
    void Execute(ID3D11DeviceContext* context) override;
private:
    D3D_PRIMITIVE_TOPOLOGY m_topology;
};

// Vincula un búfer de vértices al pipeline.
class SetVertexBufferOperation : public PipelineOperation {
public:
    SetVertexBufferOperation(ID3D11Buffer* buffer, UINT stride, UINT offset);
    void Execute(ID3D11DeviceContext* context) override;
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
    UINT m_stride;
    UINT m_offset;
};

// Vincula un búfer de índices al pipeline.
class SetIndexBufferOperation : public PipelineOperation {
public:
    SetIndexBufferOperation(ID3D11Buffer* buffer, DXGI_FORMAT format, UINT offset);
    void Execute(ID3D11DeviceContext* context) override;
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
    DXGI_FORMAT m_format;
    UINT m_offset;
};

// Vincula un vertex shader al pipeline.
class SetVertexShaderOperation : public PipelineOperation {
public:
    SetVertexShaderOperation(ID3D11VertexShader* shader);
    void Execute(ID3D11DeviceContext* context) override;
private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_shader;
};

// Vincula un pixel shader al pipeline.
class SetPixelShaderOperation : public PipelineOperation {
public:
    SetPixelShaderOperation(ID3D11PixelShader* shader);
    void Execute(ID3D11DeviceContext* context) override;
private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_shader;
};

// Vincula texturas.
class SetTextureOperation : public PipelineOperation {
public:
    SetTextureOperation(const std::vector<ID3D11ShaderResourceView*>& srvs, UINT startSlot);
    void Execute(ID3D11DeviceContext* context) override;
private:
    std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> m_srvs;
    UINT m_startSlot;
};

// Vincula samplers.
class SetSamplerOperation : public PipelineOperation {
public:
    SetSamplerOperation(const std::vector<ID3D11SamplerState*>& samplers, UINT startSlot);
    void Execute(ID3D11DeviceContext* context) override;
private:
    std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>> m_samplers;
    UINT m_startSlot;
};

// Actualiza un búfer de constantes con nuevos datos.
class UpdateConstantsBufferOperation : public PipelineOperation {
public:
    UpdateConstantsBufferOperation(ID3D11Buffer* buffer, const std::vector<char>& data);
    void Execute(ID3D11DeviceContext* context) override;
private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
    std::vector<char> m_data;
};

// Vincula búferes de constantes.
class BindConstantsBuffersOperation : public PipelineOperation {
public:
    BindConstantsBuffersOperation(const std::vector<ID3D11Buffer*>& buffers, UINT startSlot);
    void Execute(ID3D11DeviceContext* context) override;
private:
    std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> m_buffers;
    UINT m_startSlot;
};

// Dibuja la geometría sin un búfer de índices.
class DrawOperation : public PipelineOperation {
public:
    DrawOperation(UINT vertexCount, UINT startVertexLocation);
    void Execute(ID3D11DeviceContext* context) override;
private:
    UINT m_vertexCount;
    UINT m_startVertexLocation;
};

// Dibuja la geometría usando un búfer de índices.
class DrawIndexedOperation : public PipelineOperation {
public:
    DrawIndexedOperation(UINT indexCount, UINT startIndexLocation, UINT baseVertexLocation);
    void Execute(ID3D11DeviceContext* context) override;
private:
    UINT m_indexCount;
    UINT m_startIndexLocation;
    UINT m_baseVertexLocation;
};


// --- Nuevas Operaciones de Creación de Recursos y Estados ---

class CreateRasterizerStateOperation : public PipelineOperation {
public:
    CreateRasterizerStateOperation(ID3D11Device* device, const D3D11_RASTERIZER_DESC& desc);
    void Execute(ID3D11DeviceContext* context) override;
private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    D3D11_RASTERIZER_DESC m_desc;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_state;
};

class CreateBlendStateOperation : public PipelineOperation {
public:
    CreateBlendStateOperation(ID3D11Device* device, const D3D11_BLEND_DESC& desc);
    void Execute(ID3D11DeviceContext* context) override;
private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    D3D11_BLEND_DESC m_desc;
    Microsoft::WRL::ComPtr<ID3D11BlendState> m_state;
};

class CreateDepthStencilStateOperation : public PipelineOperation {
public:
    CreateDepthStencilStateOperation(ID3D11Device* device, const D3D11_DEPTH_STENCIL_DESC& desc);
    void Execute(ID3D11DeviceContext* context) override;
private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    D3D11_DEPTH_STENCIL_DESC m_desc;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_state;
};

class CreateSamplerStateOperation : public PipelineOperation {
public:
    CreateSamplerStateOperation(ID3D11Device* device, const D3D11_SAMPLER_DESC& desc);
    void Execute(ID3D11DeviceContext* context) override;
private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    D3D11_SAMPLER_DESC m_desc;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_state;
};

class CreateConstantBufferOperation : public PipelineOperation {
public:
    CreateConstantBufferOperation(ID3D11Device* device, UINT size, bool isDynamic);
    void Execute(ID3D11DeviceContext* context) override;
private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    UINT m_size;
    bool m_isDynamic;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
};

class CreateDepthStencilViewOperation : public PipelineOperation {
public:
    CreateDepthStencilViewOperation(ID3D11Device* device, const D3D11_TEXTURE2D_DESC& textureDesc, const D3D11_DEPTH_STENCIL_VIEW_DESC& viewDesc, const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDesc);
    void Execute(ID3D11DeviceContext* context) override;
private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    D3D11_TEXTURE2D_DESC m_textureDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC m_viewDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC m_srvDesc;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
};

class CreateRenderTargetsViewOperation : public PipelineOperation {
public:
    CreateRenderTargetsViewOperation(ID3D11Device* device, ID3D11Texture2D* backBuffer);
    void Execute(ID3D11DeviceContext* context) override;
private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_backBuffer;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
};
