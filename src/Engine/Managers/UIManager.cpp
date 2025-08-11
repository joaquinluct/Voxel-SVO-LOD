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
UIText* UIManager::InitText(std::vector<std::shared_ptr<VertexDefinition::VertexVariant>>& vertexDef, std::string text) {
	UIText* textElement = new UIText();
    textElement->Init();
	textElement->SetText(text);
    textElement->SetFontSize(24.0f);
	textElement->SetPosition(10.0f, 10.0f); // Posición inicial del texto
    textElement->CreateMesh(vertexDef);
    m_textElements.push_back(textElement);
    return textElement;
}

HRESULT UIManager::Init()
{
    OutputDebugStringA("Incializando UIManager...\n");
    m_deviceManager = ManagerLocator::GetManager<DeviceManager>();
    m_renderTargetManager = ManagerLocator::GetManager<RenderTargetManager>();
	m_renderManager = ManagerLocator::GetManager<RenderManager>();
    
    //// Crear matriz de proyección ortográfica
    //UINT width = static_cast<UINT>(m_deviceManager->GetWidth());
    //UINT height = static_cast<UINT>(m_deviceManager->GetHeight());
    //// m_orthoMatrix = XMMatrixOrthographicLH((float)width, (float)height, 0.0f, 1.0f);

    //m_orthoMatrix = DirectX::XMMatrixOrthographicOffCenterLH(
    //    0.0f,      // left
    //    static_cast<float>(width), // right
    //    static_cast<float>(height), // top
    //    0.0f, // bottom
    //    0.0f,      // nearZ
    //    1.0f       // farZ
    //);

    //uiElements = {};
    m_textElements = {};
    HRESULT hr = S_OK;
    OutputDebugStringA(("Resultado Init " + std::to_string(hr) + " en UIManager\n").c_str());
    return hr;
}

//HRESULT UIManager::InitText(std::vector<std::shared_ptr<VertexDefinition::VertexVariant>> vertexDef)
//{
//    std::shared_ptr<UIText> textMesh = m_renderManager->GameRenderManagerGet()->RegisterTextMesh("UITextMesh");
//    if (!textMesh)
//    {
//		OutputDebugStringA("Error al registrar el mesh de texto.\n");
//        return E_FAIL;
//	}
//	textMesh->CreateMesh(vertexDef);
//	m_textElements.push_back(textMesh.get());
//	return S_OK;
//}

void UIManager::Render()
{    
    // Crear una matriz de mundo identidad para la UI
    //DirectX::XMMATRIX uiWorldMatrix = DirectX::XMMatrixIdentity();
    //m_deviceManager->EnableAlphaBlending();

    //for (UIElement* element : uiElements)
    //{
    //    // Pasar la matriz de mundo identidad y la matriz ortográfica a los elementos de la UI
    //    element->Render(m_orthoMatrix);
    //}

    //m_deviceManager->DisableBlending();
}

void UIManager::Shutdown()
{
    /*for (UIElement* element : uiElements)
    {
		SafeRelease(element);
    }
    uiElements.clear();*/
    for (UIText* textElement : m_textElements)
    {
        SafeShutDown(textElement);
	}
}

//void UIManager::AddElement(UIElement* element)
//{
//    uiElements.push_back(element);
//}
//
//void UIManager::RemoveElement(UIElement* element)
//{
//    for (auto it = uiElements.begin(); it != uiElements.end(); ++it)
//    {
//        if (*it == element)
//        {
//            uiElements.erase(it);
//            return;
//        }
//    }
//}