// UIManager.cpp
#include "UIManager.h"
#include "UI/UIElement.h"
#include <REGISTER_MANAGER_MACRO.h>

REGISTER_MANAGER_TYPE(UIManager, "UIManager")

UIManager::UIManager () : m_deviceManager(nullptr), m_renderTargetManager(nullptr) {
	// Constructor por defecto
	// Inicializar la matriz ortográfica a identidad
	m_orthoMatrix = XMMatrixIdentity();
}

UIManager::~UIManager()
{
    Shutdown();
}

HRESULT UIManager::Init()
{
    m_deviceManager = ManagerLocator::GetManager<DeviceManager>();
    m_renderTargetManager = ManagerLocator::GetManager<RenderTargetManager>();
    
    // Crear matriz de proyección ortográfica
    UINT width = static_cast<UINT>(m_renderTargetManager->GetViewport().Width);
    UINT height = static_cast<UINT>(m_renderTargetManager->GetViewport().Height);
    // m_orthoMatrix = XMMatrixOrthographicLH((float)width, (float)height, 0.0f, 1.0f);

    m_orthoMatrix = DirectX::XMMatrixOrthographicOffCenterLH(
        0.0f,      // left
        static_cast<float>(width), // right
        static_cast<float>(height), // bottom
        0.0f,      // top
        0.0f,      // nearZ
        1.0f       // farZ
    );
        
    return S_OK;
}

void UIManager::Render()
{
    // Configurar matrices para renderizado 2D
    // Esto es CRUCIAL:  Pasar la matriz ortográfica a los elementos de la UI
    int screenWidth = static_cast<int>(m_renderTargetManager->GetViewport().Width);
    int screenHeight = static_cast<int>(m_renderTargetManager->GetViewport().Height);
    

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
        element->Release();
        delete element;
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