// UI.cpp
#include "UI.h"
#include "UIElement.h"

UI::UI() : m_deviceManager(nullptr), m_renderTargetManager(nullptr) {
	// Constructor por defecto
	// Inicializar la matriz ortográfica a identidad
	m_orthoMatrix = XMMatrixIdentity();
}

UI::~UI()
{
    Release();
}

HRESULT UI::Init(DeviceManager* deviceManager, RenderTargetManager* renderTargetManager)
{
    m_deviceManager = deviceManager;
    m_renderTargetManager = renderTargetManager;

    // Crear matriz de proyección ortográfica
    UINT width = static_cast<UINT>(m_renderTargetManager->GetViewport().Width);
    UINT height = static_cast<UINT>(m_renderTargetManager->GetViewport().Height);
    m_orthoMatrix = XMMatrixOrthographicLH((float)width, (float)height, 0.0f, 1.0f);
        
    return S_OK;
}

void UI::Render(DeviceManager* deviceManager, ShaderManager* m_shaderManager, WorldMatrixManager* worldMatrixManager, const XMMATRIX& orthoMatrix)
{
    // Configurar matrices para renderizado 2D
    // Esto es CRUCIAL:  Pasar la matriz ortográfica a los elementos de la UI
    int screenWidth = static_cast<int>(m_renderTargetManager->GetViewport().Width);
    int screenHeight = static_cast<int>(m_renderTargetManager->GetViewport().Height);
    DirectX::XMMATRIX newOrthoMatrix = DirectX::XMMatrixOrthographicOffCenterLH(
        0.0f,      // left
        static_cast<float>(screenWidth), // right
        static_cast<float>(screenHeight), // bottom
        0.0f,      // top
        0.0f,      // nearZ
        1.0f       // farZ
    );

    // Crear una matriz de mundo identidad para la UI
    //DirectX::XMMATRIX uiWorldMatrix = DirectX::XMMatrixIdentity();
    deviceManager->EnableAlphaBlending();

    for (UIElement* element : uiElements)
    {
        // Pasar la matriz de mundo identidad y la matriz ortográfica a los elementos de la UI
        element->Render(deviceManager, m_shaderManager, worldMatrixManager, newOrthoMatrix);
    }

    deviceManager->DisableBlending();
}

void UI::Release()
{
    for (UIElement* element : uiElements)
    {
        element->Release();
        delete element;
    }
    uiElements.clear();
}

void UI::AddElement(UIElement* element)
{
    uiElements.push_back(element);
}

void UI::RemoveElement(UIElement* element)
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