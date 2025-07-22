// UIManager.cpp
#include "UIManager.h"
#include "UI/UIElement.h"
#include <REGISTER_MANAGER_MACRO.h>

REGISTER_MANAGER_TYPE(UIManager, "UIManager")

UIManager::UIManager() : m_deviceManager(nullptr), m_renderTargetManager(nullptr), m_orthoMatrix{}, m_worldMatrixManager(nullptr) {	
}

UIManager::~UIManager()
{
    Shutdown();
}

HRESULT UIManager::Init()
{
    OutputDebugStringA("Incializando UIManager...\n");
    m_deviceManager = ManagerLocator::GetManager<DeviceManager>();
    m_renderTargetManager = ManagerLocator::GetManager<RenderTargetManager>();
    
    // Crear matriz de proyección ortográfica
    UINT width = static_cast<UINT>(m_renderTargetManager->GetViewport().Width);
    UINT height = static_cast<UINT>(m_renderTargetManager->GetViewport().Height);
    // m_orthoMatrix = XMMatrixOrthographicLH((float)width, (float)height, 0.0f, 1.0f);

    m_orthoMatrix = DirectX::XMMatrixOrthographicOffCenterLH(
        0.0f,      // left
        static_cast<float>(width), // right
        static_cast<float>(height), // top
        0.0f, // bottom
        0.0f,      // nearZ
        1.0f       // farZ
    );

    uiElements = {};
    HRESULT hr = S_OK;
    OutputDebugStringA(("Resultado Init " + std::to_string(hr) + " en UIManager\n").c_str());
    return hr;
}

void UIManager::Render()
{    
    // Crear una matriz de mundo identidad para la UI
    //DirectX::XMMATRIX uiWorldMatrix = DirectX::XMMatrixIdentity();
    m_deviceManager->EnableAlphaBlending();

    for (UIElement* element : uiElements)
    {
        // Pasar la matriz de mundo identidad y la matriz ortográfica a los elementos de la UI
        element->Render(m_orthoMatrix);
    }

    m_deviceManager->DisableBlending();
}

void UIManager::Shutdown()
{
    for (UIElement* element : uiElements)
    {
		SafeRelease(element);        
    }
    uiElements.clear();
}

void UIManager::AddElement(UIElement* element)
{
    uiElements.push_back(element);
}

void UIManager::RemoveElement(UIElement* element)
{
    for (auto it = uiElements.begin(); it != uiElements.end(); ++it)
    {
        if (*it == element)
        {
            uiElements.erase(it);
            return;
        }
    }
}