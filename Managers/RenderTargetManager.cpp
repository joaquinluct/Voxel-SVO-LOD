#include <windows.h>
#include "RenderTargetManager.h"
#include "../Interfaces/iDirectXManager.h"
#include "../Util/Utils.h"

RenderTargetManager::RenderTargetManager()
{
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	m_viewport = {};
    m_pRenderTargetView = nullptr;
    m_pDepthStencilView = nullptr;
    m_pContext = nullptr;
    m_width = screenWidth;
    m_height = screenHeight;

    // Inicialización para el efecto de limpieza gradual
    m_enableClearEffect = false;
    m_clearEffectFactor = 0.0f;
    m_clearEffectSpeed = 0.01f; // Valor predeterminado
}

RenderTargetManager::~RenderTargetManager()
{
    // Release resources
    Release();
}

void RenderTargetManager::Clear(const FLOAT clearColor[4])
{
    // Clear the render target and depth stencil view
    if (m_pContext && m_pRenderTargetView)
        m_pContext->ClearRenderTargetView(m_pRenderTargetView, clearColor);

    if (m_pContext && m_pDepthStencilView)
        m_pContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

HRESULT RenderTargetManager::Init(DeviceManager* deviceManager)
{
    ID3D11Texture2D* backBuffer = nullptr;
    HRESULT hr = deviceManager->GetBackBuffer(&backBuffer);
    if (FAILED(hr)) {
        OutputDebugString(L"Error al crear el Render Target View.\n");
        return hr;
    }
    if (!backBuffer) return E_POINTER;

    // Crear Render Target View
    hr = deviceManager->GetDevice()->CreateRenderTargetView(backBuffer, nullptr, &m_pRenderTargetView);
    backBuffer->Release(); // Liberamos el back buffer después de usarlo
    if (FAILED(hr)) {
        OutputDebugString(L"Error al crear el Render Target View.\n");
        return hr;
    }

    // Crear Depth Stencil View
    D3D11_TEXTURE2D_DESC depthStencilDesc = {};
    depthStencilDesc.Width = m_width;
    depthStencilDesc.Height = m_height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ID3D11Texture2D* depthStencilBuffer = nullptr;
    hr = deviceManager->GetDevice()->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencilBuffer);
    if (FAILED(hr)) {
        OutputDebugString(L"Error al crear el Render Target View.\n");
        return hr;
    }

    hr = deviceManager->GetDevice()->CreateDepthStencilView(depthStencilBuffer, nullptr, &m_pDepthStencilView);
    depthStencilBuffer->Release(); // Liberamos el buffer después de usarlo
    if (FAILED(hr)) {
        OutputDebugString(L"Error al crear el Render Target View.\n");
        return hr;
    }

    // Configurar el contexto
    m_pContext = deviceManager->GetContext();
    //if (m_pContext) m_pContext->AddRef();

    SetRenderTargets();
    SetViewport();

    return S_OK;
}

HRESULT RenderTargetManager::Init()
{
    // Initialize the render target view and depth stencil view here
    // This is a placeholder implementation, you should replace it with actual initialization code
    return S_OK;
}

HRESULT RenderTargetManager::Release()
{
    SafeRelease(m_pRenderTargetView);
    SafeRelease(m_pDepthStencilView);
    SafeRelease(m_pContext);
    m_width = 0;
    m_height = 0;
    return S_OK;
}
void RenderTargetManager::SetRenderTargets()
{
    if (m_pContext && m_pRenderTargetView) {
        m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
        //m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);
    }
}
void RenderTargetManager::SetViewport()
{
    if (m_pContext) {
        D3D11_VIEWPORT viewport = {};
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;
        viewport.Width = static_cast<FLOAT>(m_width);
        viewport.Height = static_cast<FLOAT>(m_height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        m_pContext->RSSetViewports(1, &viewport);

        m_viewport = viewport; // Guardar el viewport
    }
}

HRESULT RenderTargetManager::Render()
{
    if (m_enableClearEffect) {
        // Calcula el color de limpieza gradual
        m_clearEffectFactor += m_clearEffectSpeed;
        if (m_clearEffectFactor > 1.0f) {
            m_clearEffectFactor = 0.0f;
        }
        FLOAT clearColor[4] = { m_clearEffectFactor, m_clearEffectFactor, m_clearEffectFactor, 1.0f };
        Clear(clearColor);
    }
    else {
        // Limpia la pantalla con un color predeterminado (gris oscuro)
        FLOAT clearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
        Clear(clearColor);
    }
    return S_OK;
}

void RenderTargetManager::EnableClearEffect(bool enable)
{
    m_enableClearEffect = enable;
    if (enable) {
        ResetClearEffect(); // Reiniciar el efecto al habilitarlo
    }
}

void RenderTargetManager::SetClearEffectSpeed(float speed)
{
    m_clearEffectSpeed = speed;
}

void RenderTargetManager::ResetClearEffect()
{
    m_clearEffectFactor = 0.0f;
}