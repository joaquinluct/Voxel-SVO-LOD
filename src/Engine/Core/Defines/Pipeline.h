#pragma once
#include <d3d11.h>
#include <string>
#include <map>
#include <vector>
#include <variant>
#include <memory>
#include <wrl/client.h>
//#include <Services/Material.h>
#include <Defines/Pipeline.h>
#include <Defines/Matrix/MatrixDefinition.h>
#include <Defines/Matrix/MatrixDefinitionBase.h>
#include <Defines/ShaderSampler.h>

class Material;

using SamplerStates = std::map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>>;

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
    Device_Init_SetPixelShader,
    Device_Init_SetRenderTargetView,
    Device_ResetRenderTargetView,
    Device_Init_SetVertexShader,
    Device_Init_Viewport,
    Device_draw,
    Device_drawIndexed,
    Device_ClearRenderTargetView,
    Device_ClearDepthStencilView,
    Device_EnabledBledingState,
    Device_DisabledBledingState,
    Device_PresentSwapChain,
    Device_SetConstantsBufferState,
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
    Mesh_Render_SetInputLayout,
    Mesh_Render_SetPrimitiveToplogy,
    Mesh_Render_SetPixelShader,
    Mesh_Render_SetSampler,
    Mesh_Render_Reset_Sampler,
    Mesh_Render_SetTexture,
    Mesh_Render_Reset_Textures,
    Mesh_Render_SetVertexBuffer,
    Mesh_Render_SetVertexShader,
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
	std::string name; // Nombre de la operación
};

struct PipelineMaterialBufferData : public PipelineOperBase {
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


struct PipelinePresentSwapChain: public PipelineOperBase {
    UINT syncInterval = 1;
    UINT flags = 0;
    Microsoft::WRL::ComPtr<IDXGISwapChain> data;
    DXGI_SWAP_CHAIN_DESC desc;
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
};

struct PipelineTextureData : public PipelineOperBase
{
    UINT startSlot = 0;
    UINT numTextures = 1;
    //std::map<std::string, ID3D11ShaderResourceView*> data;
    std::vector<ID3D11ShaderResourceView*> data;
};

struct PipelineSamplerSateData : public PipelineOperBase
{
    UINT startSlot = 0;
    UINT numSamplers = 1;
    SamplerStates data;
    std::vector<ShaderSampler::SamplerDefinition> desc;
};

struct PipelineRenderTargetViewData : public PipelineOperBase
{
    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> data;
    DirectX::XMFLOAT4 clearColor;
};

struct PipelineSetRenderTargetsData: public PipelineOperBase
{
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> stencilView;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> targetView;
    bool isColorPass;
};

struct PipelineSetVertexBufferData : public PipelineOperBase
{
    UINT numBuffers = 1;
    UINT offset = 0;
    UINT startSlot = 0;
    UINT stride = 0;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
};

struct PipelineSetIndexBufferData : public PipelineOperBase
{
    DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
    UINT offset = 0;
};

struct PipelinePrimitiveTopologyData : public PipelineOperBase
{
    D3D_PRIMITIVE_TOPOLOGY data;
};

struct PipelineDrawIndexedData : public PipelineOperBase
{
    D3D11_INPUT_ELEMENT_DESC data;
    UINT numIndexes;
};

struct PipelineDrawData : public PipelineOperBase
{
    D3D11_INPUT_ELEMENT_DESC data;
    UINT vertexCount;
};

struct PipelineLayoutData : public PipelineOperBase
{
    D3D11_INPUT_ELEMENT_DESC* desc;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> data;
    UINT numElements;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
};

struct PipelineVertexShaderData : public PipelineOperBase
{
    Microsoft::WRL::ComPtr<ID3D11VertexShader> data;
    ID3DBlob* byteCode = nullptr; // Código de bytes del shader
};

struct PipelinePixelShaderData : public PipelineOperBase
{
    Microsoft::WRL::ComPtr<ID3D11PixelShader> data;
};

struct PipelineDepthStencilData : public PipelineOperBase
{
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
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> state;
    D3D11_DEPTH_STENCIL_DESC desc;
};

struct PipelineViewPortData : public PipelineOperBase
{
    D3D11_VIEWPORT desc;
};

struct PipelineRasteriezeData : public PipelineOperBase
{
    D3D11_RASTERIZER_DESC desc;
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> state;
};

struct PipelineBledingData : public PipelineOperBase
{
    D3D11_BLEND_DESC desc;
    D3D11_RASTERIZER_DESC* data;
    Microsoft::WRL::ComPtr<ID3D11BlendState> state;
};

struct PipelineBackBufferData : public PipelineOperBase
{
    Microsoft::WRL::ComPtr<ID3D11Texture2D> data;
};

struct PipelineMatrixBufferData : public PipelineOperBase
{
    // Entrada:
    Material* material;
    MatrixDefinitionBase::MatrixParams data;
    std::map<int, std::pair<std::string, MatrixDefinition::AnyMatrixBuffer>> matrices;

	// Salida:
	std::map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>> constantsBuffers; // Mapa de buffers de constantes



};

using PipelineParameter = std::variant<
	PipelineMatrixBufferData,
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
    PipelinePrimitiveTopologyData,
    PipelineSetVertexBufferData,
    PipelineSetIndexBufferData,
    PipelineRenderTargetViewData,
    PipelineSamplerSateData,
    PipelineTextureData,
    PipelinePresentSwapChain,
    PipelineBackBufferData,
    PipelineSetRenderTargetsData
>;

using PipelineData = std::variant<
    std::vector<Microsoft::WRL::ComPtr<ID3D11SamplerState>>,
    SamplerStates,
    PipelineDepthStencilData,
    Microsoft::WRL::ComPtr<ID3D11RasterizerState>,
    Microsoft::WRL::ComPtr<ID3D11BlendState>,
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView>,
    Microsoft::WRL::ComPtr<ID3D11DepthStencilState>,
    Microsoft::WRL::ComPtr<ID3D11SamplerState>,
    Microsoft::WRL::ComPtr<ID3D11Texture2D>,
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>>;

class PipelineOperation
{
private:
    PipelineOperationType operationType; // Tipo de operación en el pipeline
    PipelineParameter operationParam;
	PipelineData operationData; // Datos específicos de la operación
    int priority;
    HRESULT result;
public:
    // Constructor por defecto
    PipelineOperation() : operationType(PipelineOperationType::Unknown), operationParam(), operationData(), priority(0), result() {}
    PipelineOperation(const PipelineOperation* other) : operationType(other->GetOperationType()), priority(other->GetPriority()), operationParam(other->GetOperationParam()), operationData(other->GetOperationData()), result() {}
    // Constructor con parámetros
    PipelineOperation(PipelineOperationType type, PipelineParameter param, PipelineData operationOutData = {}, int prio = 0)
        : operationType(type), operationParam(param), operationData(operationOutData), priority(prio), result() {}
    ~PipelineOperation() {};
    PipelineOperationType GetOperationType() const { return operationType; };
    PipelineParameter GetOperationParam() const { return operationParam; };
    void SetOperationType(PipelineOperationType opType) { operationType = opType; }
    int GetPriority() const { return priority; }
	void SetPriority(int prio) { priority = prio; }
    void SetOperationData(PipelineData data) { operationData = data; }
    PipelineData GetOperationData() const {
        return operationData;
    }
	// Método para obtener un puntero al tipo de dato específico de la operación
    template<typename T>
    T GetOperationData() {
        if (operationData.index() == std::variant_npos) {
            return nullptr;
		}
        return std::get<T>(operationData);
    }
    template<typename T>
    T GetOperationParam() {
        //if (!operationParam) return nullptr;
        return std::get<T>(operationParam);
    }
    void SetResult(HRESULT hr) {
        result = hr;
    }
    HRESULT GetResult() { return result; }
};


class PipelineStore
{
private:
    std::map<std::string, PipelineData> m_pipelineStates;
public:
    PipelineStore() : m_pipelineStates() {}
    PipelineStore(const PipelineStore* other) : m_pipelineStates(other->m_pipelineStates) {  }
    ~PipelineStore() { m_pipelineStates.clear(); };

    void SetPipelineStates(std::map<std::string, PipelineData> pipelineStates) {
        m_pipelineStates = pipelineStates;
    }

    std::map<std::string, PipelineData> GetPielineStates() { return m_pipelineStates; }

    template<typename T>
    T GetState(std::string stateName) {
        T result;
        auto pair = m_pipelineStates.find(stateName);
        if (pair != m_pipelineStates.end()) {
            result = std::get<T>(pair->second);
        }
        return result;
    }
};