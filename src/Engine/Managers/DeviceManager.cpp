#define NOMINMAX 
#include "DeviceManager.h"
#include "ManagerBase.h"
#include "REGISTER_MANAGER_MACRO.h"
#include "Utils.h"
#include <d3d11.h>
#include <Pipeline/PipelineStateLocator.h>
#include <windows.h>

REGISTER_MANAGER_TYPE(DeviceManager, "DeviceManager")

DeviceManager::DeviceManager()
    : m_device()
{
    m_context = nullptr;
    m_swapChain = nullptr;
    m_featureLevel = D3D_FEATURE_LEVEL_11_0;
    m_alphaBlendState = nullptr;
    m_defaultBlendState = nullptr;
    m_rasterizerState = nullptr;
    m_rasterizerShadowsState = nullptr;
}

DeviceManager::~DeviceManager()
{
}

HRESULT DeviceManager::Init(EngineContext* context)
{
    ManagerBase::Init(context);

    m_swapChainMainConfig = PipelineStateLocator::GetPipelineState<SwapChainMain>();
    HRESULT hr = (m_swapChainMainConfig) ? S_OK : E_FAIL;
    if (FAILED(hr)) {
        OutputDebugStringA("Error al obtener SwapChainMainConfig.\n");
        return hr;
    }
    OutputDebugStringA("Incializando DeviceManager...\n");
    hr = CreateDeviceAndSwapChain(context->hWnd, static_cast<int>(context->width), static_cast<int>(context->height));
    if (FAILED(hr)) {
        MessageBox(*context->hWnd, L"Error al crear el dispositivo DirectX 11", L"Error", MB_OK);
        return hr;
    }
    // -------CÓDIGO COMENTADO ANTES DEL CAMBIO DE RENDERING------------------
       // Modo Rasterizado (ColourPass)
       /*hr = InitRasterizedState();
       if (FAILED(hr)) {
           MessageBox(*hwnd, L"Error al inciailizar el modo Raterizado", L"Error", MB_OK);
           return hr;
       }*/

       // Modo Rasterizado para el pase de las sombras (ShadowPass)
       /*hr = InitRasterizedShadowsState();
       if (FAILED(hr)) {
           MessageBox(hwnd, L"Error al inciailizar el modo Raterizado", L"Error", MB_OK);
           return hr;
       }*/


       // Inicializar el estado de mezcla
       /*hr = InitBlending();
       if (FAILED(hr)) {
           MessageBox(*hwnd, L"Error al inciailizar el Blendig", L"Error", MB_OK);
           return hr;
       }*/
       // -------FIN CÓDIGO COMENTADO ANTES DEL CAMBIO DE RENDERING------------------ 



    m_width = static_cast<float>(context->width);
    m_height = static_cast<float>(context->height);

    OutputDebugStringA(("DeviceManager - Resolución: " + std::to_string(m_width) + "x" + std::to_string(m_height) + "\n").c_str());
    return hr;
}

HRESULT DeviceManager::CreateDeviceAndSwapChain(HWND* hwnd, int width, int height) {

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = m_swapChainMainConfig->BufferCount;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = static_cast<DXGI_FORMAT>(m_swapChainMainConfig->Format);
    sd.BufferDesc.RefreshRate.Numerator = m_swapChainMainConfig->Numerator;
    sd.BufferDesc.RefreshRate.Denominator = m_swapChainMainConfig->Denominator;
    size_t pos;
    unsigned long flags = std::stoul(m_swapChainMainConfig->BufferUsage, &pos, 16);
    sd.BufferUsage = static_cast<DXGI_USAGE>(flags);
    sd.OutputWindow = *hwnd;
    sd.SampleDesc.Count = m_swapChainMainConfig->SampleCount;
    sd.SampleDesc.Quality = m_swapChainMainConfig->SampleQuality;
    sd.Windowed = m_swapChainMainConfig->Windowed;
    sd.SwapEffect = static_cast<DXGI_SWAP_EFFECT>(m_swapChainMainConfig->SwapEffect);

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    ID3D11Device* pTempDevice = nullptr;
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        createDeviceFlags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &m_swapChain,
        &pTempDevice,
        &m_featureLevel,
        &m_context
    );

    if (SUCCEEDED(hr)) {
        m_device = Microsoft::WRL::ComPtr<ID3D11Device>(pTempDevice);
    }

    return hr;
}

//HRESULT DeviceManager::InitRasterizedState() {
//    D3D11_RASTERIZER_DESC rasterDesc = {};
//    rasterDesc.FillMode = D3D11_FILL_SOLID;  // Dibujar caras sólidas
//    //rasterDesc.CullMode = D3D11_CULL_NONE;   // Culling deseactivado
//    rasterDesc.CullMode = D3D11_CULL_BACK;   // Culling de caras traseras
//    //rasterDesc.CullMode = D3D11_CULL_FRONT;   // Culling de caras fontales
//    rasterDesc.FrontCounterClockwise = FALSE; // Orientaci�n de las caras frontales
//    //rasterDesc.FrontCounterClockwise = TRUE; // Orientaci�n de las caras frontales
//	rasterDesc.DepthClipEnable = true;
//    rasterDesc.AntialiasedLineEnable = true;
//    
//    HRESULT hr = m_device->CreateRasterizerState(&rasterDesc, &m_rasterizerState);
//    if (FAILED(hr)) {
//        OutputDebugString(L"Error al crear el Rasterizer State.\n");
//    }
//
//    // Guardar el estado del rasterizador globalmente si es necesario
//    // m_context->RSSetState(m_rasterizerState);
//    return hr;
//}
//
//HRESULT DeviceManager::InitRasterizedShadowsState() {
//    D3D11_RASTERIZER_DESC rasterDesc = {};
//    rasterDesc.FillMode = D3D11_FILL_SOLID;
//    rasterDesc.CullMode = D3D11_CULL_FRONT; // LO MÁS IMPORTANTE: Front-face culling
//    rasterDesc.FrontCounterClockwise = FALSE; // Esto debe coincidir con la orientación de tus vértices
//    // (FALSE para CCW, TRUE para CW)
//    rasterDesc.DepthBias = 50; // Ejemplo: un pequeño bias entero. Ajustar según necesites.
//    // 50 es un valor arbitrario, podría ser 1, 100, 1000 dependiendo de la escala.
//    rasterDesc.DepthBiasClamp = 0.0f; // No clamping en este ejemplo
//    rasterDesc.SlopeScaledDepthBias = 1.0f; // Ejemplo: 1.0f para bias escalado por la pendiente. Ajustar.
//    // Un valor común es 1.0f o 2.0f.
//    rasterDesc.DepthClipEnable = TRUE;
//    rasterDesc.ScissorEnable = FALSE;
//    rasterDesc.MultisampleEnable = FALSE; // No AA para el mapa de sombras
//    rasterDesc.AntialiasedLineEnable = FALSE; // No AA para líneas en el mapa de sombras
//
//    HRESULT hr = m_device->CreateRasterizerState(&rasterDesc, &m_rasterizerShadowsState);
//    if (FAILED(hr))
//    {
//        OutputDebugString(L"Error al crear el Rasterizer Shadow State.\n");
//    }
//    return hr;
//}

//HRESULT DeviceManager::InitBlending()
//{
//    HRESULT hr = S_OK;
//
//    D3D11_BLEND_DESC blendDesc = {};
//    blendDesc.AlphaToCoverageEnable = FALSE;
//    blendDesc.IndependentBlendEnable = FALSE; // Solo un render target
//
//    // --- Configuraci�n para Alpha Blending (para UI con transparencia) ---
//    blendDesc.RenderTarget[0].BlendEnable = TRUE;
//    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
//    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
//    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
//    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
//    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
//    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
//    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
//
//    hr = m_device->CreateBlendState(&blendDesc, &m_alphaBlendState);
//    if (FAILED(hr))
//    {
//        OutputDebugStringA("Error al crear el alpha blend state.\n");
//        return hr;
//    }
//
//    // --- Configuraci�n para Blending por defecto (Opaco) ---
//    // Generalmente es el estado sin blending o con una mezcla simple.
//    // D3D11_BLEND_DESC por defecto tiene BlendEnable = FALSE para todos los render targets.
//    // As� que puedes usar un blendDesc con todo en FALSE o simplemente crear uno nuevo.
//    D3D11_BLEND_DESC defaultBlendDesc = {}; // Todos los valores por defecto son FALSE
//    defaultBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
//
//    hr = m_device->CreateBlendState(&defaultBlendDesc, &m_defaultBlendState);
//    if (FAILED(hr))
//    {
//        OutputDebugStringA("Error al crear el default (opaco) blend state.\n");
//        return hr;
//    }
//
//    // OJO : QUITAR ESTA LÍNEA !!!!!!!!!!!!!!!!!!
//    SetRasterizerState();
//
//    return S_OK;
//}

//HRESULT DeviceManager::GetBackBuffer(ID3D11Texture2D** ppBackBuffer) {
//    //if (!m_swapChain) {
//    //    OutputDebugStringA("Error: SwapChain no est� inicializado.\n");
//    //    return E_FAIL;
//    //}
//
//    //// Obtener el back buffer del swap chain
//    //HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)ppBackBuffer);
//    //if (FAILED(hr)) {
//    //    OutputDebugStringA("Error: No se pudo obtener el back buffer del swap chain.\n");
//    //}
//    //return hr;
//    return S_OK;
//}

Microsoft::WRL::ComPtr<ID3D11Device> DeviceManager::GetDevice() {
    //std::lock_guard<std::mutex> lock(m_mutex); // Bloquea el mutex durante la lectura
    return m_device;
}

Microsoft::WRL::ComPtr<ID3D11DeviceContext> DeviceManager::GetContext() {
    std::lock_guard<std::mutex> lock(m_mutex); // Bloquea el mutex durante la lectura
    return m_context;
}

void DeviceManager::Render()
{
    // Presentar el swap chain
    //m_swapChain->Present(1, 0);
}

//void DeviceManager::EnableAlphaBlending()
//{
//    // El segundo par�metro (blendFactor) es para casos avanzados, generalmente nullptr o {0,0,0,0}.
//    // El tercer par�metro (sampleMask) es generalmente 0xFFFFFFFF.
//    if (m_context && m_alphaBlendState)
//    {
//        m_context->OMSetBlendState(m_alphaBlendState, nullptr, 0xFFFFFFFF);
//    }
//}

//void DeviceManager::DisableBlending()
//{
//    // Vuelve al estado por defecto (opaco)
//    if (m_context && m_defaultBlendState)
//    {
//        m_context->OMSetBlendState(m_defaultBlendState, nullptr, 0xFFFFFFFF);
//    }
//}

//void DeviceManager::SetRasterizerState()
//{
//    if (m_context && m_rasterizerState) {
//        m_context->RSSetState(m_rasterizerState);
//    }
//    else {
//        OutputDebugString(L"Error: Contexto o Rasterizer State no est�n inicializados.\n");
//    }
//}

//void DeviceManager::SetRasterizerShadowsState()
//{
//    if (m_context && m_rasterizerShadowsState) {
//        m_context->RSSetState(m_rasterizerShadowsState);
//    }
//    else {
//        OutputDebugString(L"Error: Contexto o Rasterizer State no est�n inicializados.\n");
//    }
//}

void DeviceManager::ResetContextState()
{
    if (m_context) {
        m_context->RSSetState(nullptr);
    }
    else {
        OutputDebugString(L"Error: Contexto no est� inicializado.\n");
    }
}

void DeviceManager::Shutdown()
{
    // Antes de liberar, si est�s en modo de depuraci�n y hay objetos pendientes
    if (m_context) m_context->ClearState();
    SafeRelease(m_alphaBlendState);
    SafeRelease(m_defaultBlendState);

    // Aseg�rate de liberar el ID3D11Device al final, opcionalmente con un reporte de objetos vivos
    if (m_device)
    {
        // Esto es �til para depurar fugas de memoria de D3D
        // Comenta para builds de release si no quieres la dependencia de D3D11SDKLayers.dll
        // ID3D11Debug* debugDev;
        // HRESULT hr = m_device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debugDev));
        // if (SUCCEEDED(hr))
        // {
        //     debugDev->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
        //     debugDev->Release();
        // }
        m_device->Release();
        m_device = nullptr;
    }
    // Liberar el contexto inmediato
    //m_device->Release();
    //Saf(m_context);
}
