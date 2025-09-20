#pragma once
#include <d3d11.h>
#include <Defines/Matrix/MatrixDefinition.h>
#include <Defines/Matrix/MatrixDefinitionBase.h>
#include <Defines/ShaderSampler.h>
#include <Defines/Types/Sampler.h>
#include <DirectXMathMatrix.inl>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <wrl/client.h>

class FrameStateService;
class Material;

inline constexpr std::string_view MATRIX_PARAM_BASE = "BaseParams";
inline constexpr std::string_view MATRIX_PARAM_LIGHT = "LightParams";
inline constexpr std::string_view MATRIX_PARAM_MATERIAL = "MaterialParams";
inline constexpr std::string_view MATRIX_PARAM_WATER = "WaterParams";


enum class PipelineOperationType {
    Unknown = -1, // Para manejar casos no definidos
    Device_Init_BledingState,
    Device_Init_ConstantsBuffers,
    Device_Init_CreateBackBuffer,
    Device_Init_CreateRenderTargetView,
    Device_Init_CreateDeviceAndSwapChain,
    Device_Init_RasterizedState,
    Device_Init_Samplers,
    Device_Init_SetLayout,
    Device_Init_SetSencilState,
    Device_Init_SetSencilView,
    Device_Init_SetRenderTargetView,
    Device_Unbind_RenderTargetView,
    Device_ResetRenderTargetView,
    Device_Init_SetVertexShader,
    Device_Init_Viewport,
    Device_draw,
    Device_drawIndexedShared,
    Device_drawIndexed,
    Device_drawInstanced,
    Device_ClearRenderTargetView,
    Device_ClearDepthStencilView,
    Device_ClearStencilView,
    Device_ClearDepthView,
    Device_EnabledBledingState,
    Device_DisabledBledingState,
    Device_PresentSwapChain,
    Device_SetConstantsBufferState,
    Device_UpdateConstantsBufferResource,
    Device_ResetConstantsBuffers,
    Device_SetDepthStencilState,
    Device_SetRasterizedState,
    Device_SetViewport,
    Device_ResetContextState,
    Mesh_Init_InputLayout,
    Mesh_Init_MatrixBuffer,
    Mesh_Init_SamplerState,
    Mesh_Render_BindConstantsBuffers,
    Mesh_Render_BindShader,
    Mesh_Render_RestoreState,
    Mesh_Render_SaveState,
    Mesh_Render_SetIndexBuffer,
    Mesh_Render_ResetIndexBuffer,
    Mesh_Render_SetInputLayout,
    Mesh_Render_SetPrimitiveToplogy,
    Mesh_Render_SetPixelShader,
    Mesh_Render_SetSampler,
    Mesh_Render_Reset_Sampler,
    Mesh_Render_SetTexture,
    Mesh_Render_Reset_Textures,
    Mesh_Render_SetVertexBuffer,
    Mesh_Render_ResetVertexBuffer,
    Mesh_Render_SetVertexShader,
    Mesh_Render_ResetVertexShader,
    Count // Para iterar o saber el número de operaciones
};

// Enum para tipos de buffers de matrices, usando flags binarios
//enum class PipelineMatrixBufferType : unsigned int {
//    None                = 0,
//    WorldMatrix         = 1 << 0, // 1
//    ViewMatrix          = 1 << 1, // 2
//    ProjectionMatrix    = 1 << 2, // 4
//    LightViewProjMatrix = 1 << 3, // 8
//    CameraPosition      = 1 << 4, // 16
//    LightDirection      = 1 << 5, // 32
//    LightColor          = 1 << 6, // 64
//    MaterialAlbedo      = 1 << 7, // 128
//    MaterialRoughness   = 1 << 8, // 256
//    MaterialMetallic    = 1 << 9, // 512
//    MaterialF0          = 1 << 10, // 1024
//    MaterialAO          = 1 << 11, // 2048
//    // Puedes añadir más flags según sea necesario
//};

struct PipelineOperBase
{
    virtual PipelineOperBase* clone() { return this; };
    virtual std::string GetName() { return name; }
    std::string name; // Nombre de la operación
};

struct PipelineMaterialBufferData : public PipelineOperBase {
    PipelineOperBase* clone() {
        return new PipelineMaterialBufferData(*this);
    };
    PipelineMaterialBufferData()
        : worldMatrix(DirectX::XMMatrixIdentity()),
        viewMatrix(DirectX::XMMatrixIdentity()),
        projectionMatrix(DirectX::XMMatrixIdentity()),
        lightViewProjectionMatrix(DirectX::XMMatrixIdentity()),
        cameraPosition(0.0f, 0.0f, 0.0f),
        lightDirection(0.0f, -1.0f, 0.0f), // Dirección por defecto de la luz (hacia abajo)
        lightColor(1.0f, 1.0f, 1.0f, 1.0f), // Color por defecto de la luz (blanco)
        materialAlbedo(0.8f, 0.8f, 0.8f, 0.8f), // Color base del material (si no hay textura)
        materialRoughness(0.5f),           // Rugosidad del material (0.0=liso, 1.0=rugoso)
        materialMetallic(0.0f),            // Metalicidad del material (0.0=dieléctrico, 1.0=metal)
        materialF0(0.04f, 0.04f, 0.04f),    // Reflectividad especular para dieléctricos (generalmente 0.04)
        materialAO(0.3f),                 // Oclusión ambiental (1.0=sin oclusión)
        paddingCamera(.0f),
        paddingLight1(.0f),
        paddingMaterial1(.0f),
        paddingMaterial2(.0f)
    {
    }

    unsigned int oper = 0;

    DirectX::XMMATRIX worldMatrix;
    DirectX::XMMATRIX viewMatrix;
    DirectX::XMMATRIX projectionMatrix;

    // Datos para CameraData
    DirectX::XMFLOAT3 cameraPosition;
    float paddingCamera; // Relleno para alinear a 16 bytes (después de XMFLOAT3)

    // Datos para DirectionalLight
    DirectX::XMFLOAT3 lightDirection;
    float paddingLight1; // Relleno
    DirectX::XMFLOAT4 lightColor; // El color de la luz direccional

    // Datos para MaterialData (si se usan como constantes en lugar de texturas)
    DirectX::XMFLOAT4 materialAlbedo; // Albedo (color base) del material
    float materialRoughness;      // Rugosidad del material
    float materialMetallic;       // Metalicidad del material
    DirectX::XMFLOAT3 materialF0;         // F0 para dieléctricos (o se calcula para metales)
    float materialAO = 0.66f;             // Oclusión ambiental
    float paddingMaterial1;       // Relleno para asegurar alineación final a 16 bytes si es necesario
    float paddingMaterial2;

    // Datos para matrices de sombras
    DirectX::XMMATRIX lightViewProjectionMatrix;
};

struct PipelinePresentSwapChain : public PipelineOperBase {
    PipelineOperBase* clone() {
        return new PipelinePresentSwapChain(*this);
    };
    UINT syncInterval = 1;
    UINT flags = 0;
    Microsoft::WRL::ComPtr<IDXGISwapChain> data;
    DXGI_SWAP_CHAIN_DESC desc;
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
};

struct PipelineTextureData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        return new PipelineTextureData(*this);
    };
    UINT startSlot = 0;
    UINT numTextures = 1;
    //std::map<std::string, ID3D11ShaderResourceView*> data;
    std::vector<ID3D11ShaderResourceView*> data;
};

struct PipelineSamplerSateData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        return new PipelineSamplerSateData(*this);
    };
    UINT startSlot = 0;
    UINT numSamplers = 1;
    SamplerStates data;
    std::vector<ShaderSampler::SamplerDefinition> desc;
};

struct PipelineRenderTargetViewData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        PipelineRenderTargetViewData* copy = new PipelineRenderTargetViewData(*this);
        copy->data.Attach(data.Get());
        return copy;
    };
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> data;
    DirectX::XMFLOAT4 clearColor;
};

struct PipelineSetRenderTargetsData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        return new PipelineSetRenderTargetsData(*this);
    };
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> stencilView;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> targetView;
    bool isColorPass;
};

struct PipelineSetVertexBufferData : public PipelineOperBase
{

    void SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> buffer, UINT stride) {
        std::lock_guard<std::mutex> lock(m_mutex); // Bloquea el mutex durante la asignación
        vertexBuffer = buffer;
        vertexBufferRaw = buffer.Get();
        this->stride = stride;
    }
    ID3D11Buffer* GetVertexBuffer() {
        std::lock_guard<std::mutex> lock(m_mutex); // Bloquea el mutex durante la lectura
        return vertexBufferRaw;
    }

    UINT numBuffers = 1;
    UINT offset = 0;
    UINT startSlot = 0;
    UINT stride = 0;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    ID3D11Buffer* vertexBufferRaw;
    std::mutex m_mutex;

    PipelineOperBase* clone() {
        return this;
    };
};

struct PipelineSetIndexBufferData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        return new PipelineSetIndexBufferData(*this);
    };
    DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    UINT offset = 0;
};

struct PipelinePrimitiveTopologyData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        return new PipelinePrimitiveTopologyData(*this);
    };
    D3D_PRIMITIVE_TOPOLOGY data;
};

struct PipelineDrawIndexedData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        return new PipelineDrawIndexedData(*this);
    };
    D3D11_INPUT_ELEMENT_DESC data;
    UINT numIndexes;
    UINT indexOffset;
    UINT vertexOffset;
};

struct PipelineDrawData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        return new PipelineDrawData(*this);
    };
    D3D11_INPUT_ELEMENT_DESC data;
    UINT vertexCount;
};

struct PipelineDrawInstancedData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        return new PipelineDrawInstancedData(*this);
    };
    D3D11_INPUT_ELEMENT_DESC data;
    UINT instanceCount;
    UINT vertexCountPerInstance;
    UINT indexCountPerInstance;
};

struct PipelineLayoutData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        return new PipelineLayoutData(*this);
    };
    D3D11_INPUT_ELEMENT_DESC* desc;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> data;
    UINT numElements;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
};

struct PipelineVertexShaderData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        return new PipelineVertexShaderData(*this);
    };
    Microsoft::WRL::ComPtr<ID3D11VertexShader> data;
    ID3DBlob* byteCode = nullptr; // Código de bytes del shader
};

struct PipelinePixelShaderData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        return new PipelinePixelShaderData(*this);
    };
    Microsoft::WRL::ComPtr<ID3D11PixelShader> data;
};

struct PipelineDepthStencilData : public PipelineOperBase
{
    PipelineDepthStencilData Clone() {
        PipelineDepthStencilData copy = *this;
        if (hasViewDesc) {
            copy.viewDesc = viewDesc;
        }
        if (shaderViewDesc.Format != DXGI_FORMAT_UNKNOWN) {
            copy.shaderViewDesc = shaderViewDesc;
        }
        if (viewTextureData) {
            viewTextureData->AddRef(); // Incrementar el recuento de referencias
            copy.viewTextureData = viewTextureData;
        }
        if (stencilViewData) {
            copy.stencilViewData = stencilViewData;
        }
        if (shaderViewData) {
            copy.shaderViewData = shaderViewData;
        }
        return copy;
    }
    Microsoft::WRL::ComPtr <ID3D11DepthStencilView> stencilViewData;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderViewData;
    ID3D11Texture2D* viewTextureData = nullptr;
    D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;
    D3D11_SHADER_RESOURCE_VIEW_DESC shaderViewDesc;
    D3D11_TEXTURE2D_DESC desc;
    UINT clearFlags;
    FLOAT depth;
    UINT8 stencil;
    bool hasViewDesc = false;
};

struct PipelineSetencilStateData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        return new PipelineSetencilStateData(*this);
    };
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> state;
    D3D11_DEPTH_STENCIL_DESC desc;
};

struct PipelineViewPortData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        return new PipelineViewPortData(*this);
    };
    D3D11_VIEWPORT desc;
};

struct PipelineRasteriezeData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        return new PipelineRasteriezeData(*this);
    };
    D3D11_RASTERIZER_DESC desc;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> state;
};

struct PipelineBledingData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        return new PipelineBledingData(*this);
    };
    D3D11_BLEND_DESC desc;
    D3D11_RASTERIZER_DESC* data;
    Microsoft::WRL::ComPtr<ID3D11BlendState> state;
};

struct PipelineBackBufferData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        return new PipelineBackBufferData(*this);
    };
    Microsoft::WRL::ComPtr<ID3D11Texture2D> data;
};

struct PipelineMatrixBufferData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        return new PipelineMatrixBufferData(*this);
    };
    // Entrada:
    Material* material;
    //MatrixDefinitionBase::MatrixParams data;
    std::map<std::string, std::shared_ptr<IMatrixParams>> params;
    std::map<int, std::pair<std::string, MatrixDefinition::AnyMatrixBuffer>> matrices;

    // Salida:
    std::map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>> constantsBuffers; // Mapa de buffers de constantes
};

struct PipelineUpdateMatrixBufferData : public PipelineOperBase
{
    PipelineOperBase* clone() {
        return new PipelineUpdateMatrixBufferData(*this);
    };
    // Entrada:
    std::shared_ptr<MatrixDefinitionBase::WaterMatrixParams> data;
    //MatrixDefinitionBase::MatrixBufferType oper = MatrixDefinitionBase::MatrixBufferType::None;
    std::map<int, std::pair<std::string, MatrixDefinition::AnyMatrixBuffer>> matrices;

    // Salida:
    Microsoft::WRL::ComPtr<ID3D11Buffer> constantsBuffer; // buffer a actualizar
};

using PipelineParameter = std::variant <
    PipelineMatrixBufferData,
    PipelineUpdateMatrixBufferData,
    PipelineMaterialBufferData,
    PipelineRasteriezeData,
    PipelineBledingData,
    PipelineDepthStencilData,
    PipelineSetencilStateData,
    PipelineViewPortData,
    PipelineVertexShaderData,
    PipelinePixelShaderData,
    PipelineLayoutData,
    PipelineDrawData,
    PipelineDrawIndexedData,
    PipelineDrawInstancedData,
    PipelinePrimitiveTopologyData,
    PipelineSetVertexBufferData,
    PipelineSetIndexBufferData,
    PipelineRenderTargetViewData,
    PipelineSamplerSateData,
    PipelineTextureData,
    PipelinePresentSwapChain,
    PipelineBackBufferData,
    PipelineSetRenderTargetsData
> ;

class PipelineOperation
{
public:
    // Función ejecutora
    using ExecuteFn = std::function<void(PipelineOperation*, FrameStateService*)>;

    void SetExecutor(ExecuteFn fn) { executor = std::move(fn); }
    void Execute(FrameStateService* frameState) {
        if (executor) executor(this, frameState);
    }
    // Constructor por defecto
    PipelineOperation() : operationType(PipelineOperationType::Unknown), priority(0), result(), parameter(), operationParameter{} {}
    PipelineOperation(PipelineOperation* other) : operationType(other->GetOperationType()), priority(other->GetPriority()), result() {}
    // Constructor con parámetros
    PipelineOperation(PipelineOperationType type, int prio = 0)
        : operationType(type), priority(prio), result() {
    }
    PipelineOperation(PipelineOperationType operationType, FrameStateService* frameState, int prio = 0) : operationType(operationType), priority(prio), result() {}
    ~PipelineOperation() {};

    // Datos de entrada y salida de la operación
    PipelineOperationType GetOperationType() const { return operationType; };
    void SetOperationType(PipelineOperationType opType) { operationType = opType; }
    void SetOpertionParameter(PipelineOperBase* param) { operationParameter = param; }
    PipelineOperBase* GetOperationParameter() { return operationParameter; }
    void SetParameter(std::optional<PipelineOperBase>& param) { parameter = param; }
    std::optional<PipelineOperBase> GetParameter() { return parameter; }
    PipelineOperBase GetParameterValue() { return parameter.value(); }
    int GetPriority() const { return priority; }
    void SetPriority(int prio) { priority = prio; }
    HRESULT GetResult() const { return result; }
    void SetResult(HRESULT hr) { result = hr; }
private:
    std::string renderPassName;
    PipelineOperationType operationType; // Tipo de operación en el pipeline
    PipelineOperBase* operationParameter;
    std::optional<PipelineOperBase> parameter; // Parámetros específicos de la operación
    int priority;
    HRESULT result;
    // Función que ejecuta la operación
    ExecuteFn executor;
};
