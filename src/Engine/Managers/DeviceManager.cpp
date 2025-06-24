#define NOMINMAX 
#include <windows.h>
#include "DeviceManager.h"
#include <ManagerLocator/ManagerLocator.h>
#include "REGISTER_MANAGER_MACRO.h"
#include <d3d11.h>
#include "Utils.h"

REGISTER_MANAGER_TYPE(DeviceManager, "DeviceManager")

DeviceManager::DeviceManager()
{
	m_device = nullptr;
	m_context = nullptr;
	m_swapChain = nullptr;
	m_featureLevel = D3D_FEATURE_LEVEL_11_0;
	m_alphaBlendState = nullptr;
	m_defaultBlendState = nullptr;
	m_rasterizerState = nullptr;
}

DeviceManager::~DeviceManager()
{
}

HRESULT DeviceManager::Init(HWND hwnd, int width, int height)
{   
    m_hwnd = &hwnd;
    HRESULT hr = CreateDeviceAndSwapChain(hwnd, width, height);
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Error al crear el dispositivo DirectX 11", L"Error", MB_OK);
        return hr;
    }

    // Modo Rasterizado
    hr = InitRasterizedState();
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Error al inciailizar el modo Raterizado", L"Error", MB_OK);
        return hr;
    }


	// Inicializar el estado de mezcla
	hr = InitBlending();
    if (FAILED(hr)) {
        MessageBox(hwnd, L"Error al inciailizar el Blendig", L"Error", MB_OK);
        return hr;
    }

    return hr;
}

HRESULT DeviceManager::CreateDeviceAndSwapChain(HWND hwnd, int width, int height) {

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

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
        &m_device,
        &m_featureLevel,
        &m_context
    );
    return hr;
}

HRESULT DeviceManager::InitRasterizedState() {
    D3D11_RASTERIZER_DESC rasterDesc = {};
    rasterDesc.FillMode = D3D11_FILL_SOLID;  // Dibujar caras sólidas
    rasterDesc.CullMode = D3D11_CULL_NONE;   // Culling deseactivado
    //rasterDesc.CullMode = D3D11_CULL_BACK;   // Culling de caras traseras
    //rasterDesc.CullMode = D3D11_CULL_FRONT;   // Culling de caras fontales
    rasterDesc.FrontCounterClockwise = FALSE; // Orientaci�n de las caras frontales
    //rasterDesc.FrontCounterClockwise = TRUE; // Orientaci�n de las caras frontales
    rasterDesc.DepthClipEnable = true;
    rasterDesc.AntialiasedLineEnable = true;

    HRESULT hr = m_device->CreateRasterizerState(&rasterDesc, &m_rasterizerState);
    if (FAILED(hr)) {
        OutputDebugString(L"Error al crear el Rasterizer State.\n");
    }

    // Guardar el estado del rasterizador globalmente si es necesario
    // m_context->RSSetState(m_rasterizerState);
    return S_OK;
}

HRESULT DeviceManager::InitBlending()
{
    HRESULT hr = S_OK;

    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.AlphaToCoverageEnable = FALSE;
    blendDesc.IndependentBlendEnable = FALSE; // Solo un render target

    // --- Configuraci�n para Alpha Blending (para UI con transparencia) ---
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    hr = m_device->CreateBlendState(&blendDesc, &m_alphaBlendState);
    if (FAILED(hr))
    {
        OutputDebugStringA("Error al crear el alpha blend state.\n");
        return hr;
    }

    // --- Configuraci�n para Blending por defecto (Opaco) ---
    // Generalmente es el estado sin blending o con una mezcla simple.
    // D3D11_BLEND_DESC por defecto tiene BlendEnable = FALSE para todos los render targets.
    // As� que puedes usar un blendDesc con todo en FALSE o simplemente crear uno nuevo.
    D3D11_BLEND_DESC defaultBlendDesc = {}; // Todos los valores por defecto son FALSE
    defaultBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    hr = m_device->CreateBlendState(&defaultBlendDesc, &m_defaultBlendState);
    if (FAILED(hr))
    {
        OutputDebugStringA("Error al crear el default (opaco) blend state.\n");
        return hr;
    }

    return S_OK;
}

HRESULT DeviceManager::GetBackBuffer(ID3D11Texture2D** ppBackBuffer) {
    if (!m_swapChain) {
        OutputDebugStringA("Error: SwapChain no est� inicializado.\n");
        return E_FAIL;
    }

    // Obtener el back buffer del swap chain
    HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)ppBackBuffer);
    if (FAILED(hr)) {
        OutputDebugStringA("Error: No se pudo obtener el back buffer del swap chain.\n");
    }
    return hr;
}

ID3D11Device* DeviceManager::GetDevice() {
    return m_device;
}

ID3D11DeviceContext* DeviceManager::GetContext() {
	return m_context;
}

IDXGISwapChain* DeviceManager::GetSwapChain() {
	return m_swapChain;
}

void DeviceManager::Render()
{    
	// Presentar el swap chain
    m_swapChain->Present(1, 0);
    //m_context->OMSetRenderTargets(1, &n_pRenderTargetView, m_pDepthStencilView);    
}

void DeviceManager::EnableAlphaBlending()
{
    // El segundo par�metro (blendFactor) es para casos avanzados, generalmente nullptr o {0,0,0,0}.
    // El tercer par�metro (sampleMask) es generalmente 0xFFFFFFFF.
    if (m_context && m_alphaBlendState)
    {
        m_context->OMSetBlendState(m_alphaBlendState, nullptr, 0xFFFFFFFF);
    }
}

void DeviceManager::DisableBlending()
{
    // Vuelve al estado por defecto (opaco)
    if (m_context && m_defaultBlendState)
    {
        m_context->OMSetBlendState(m_defaultBlendState, nullptr, 0xFFFFFFFF);
    }
}

void DeviceManager::SetRasterizerState()
{
    if (m_context && m_rasterizerState) {
        m_context->RSSetState(m_rasterizerState);
    }
    else {
        OutputDebugString(L"Error: Contexto o Rasterizer State no est�n inicializados.\n");
    }
}

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
    SafeRelease(m_swapChain);

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
	SafeRelease(m_swapChain);
	SafeRelease(m_device);
	//Saf(m_context);
}