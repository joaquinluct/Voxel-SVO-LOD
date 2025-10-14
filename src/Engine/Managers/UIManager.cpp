// UIManager.cpp
#include "UIManager.h"
#include <Engine/Rendering/RenderCommand.h>
#include <Assets/Base/MeshAsset.h>
#include <DeviceManager.h>
#include <ManagerLocator/ManagerLocator.h>
#include <UI/UIText.h>
//#include <UI/UIElement.h>

#include <REGISTER_MANAGER_MACRO.h>

REGISTER_MANAGER_TYPE(UIManager, "UIManager")

UIManager::UIManager() : m_deviceManager(nullptr), m_orthoMatrix{}, m_uiAsset{ nullptr } {
}

UIText* UIManager::CreateLabel(const std::string& name, const std::string& text) {
    // If mesh is already registered under the name, update text
    auto it = m_textElements.find(name);
    if (it != m_textElements.end()) {
        it->second->SetText(text);
        return it->second;
    }

    // Otherwise create a simple UIText placeholder (without mesh)
    UIText* label = new UIText();
    label->Init();
    label->SetText(text);
    m_textElements[name] = label;
    return label;
}


UIManager::~UIManager()
{
    Shutdown();
}
UIText* UIManager::InitText(std::shared_ptr<MeshAsset> mesh, std::vector<std::shared_ptr<VertexDefinition::VertexVariant>>& vertexDef) {
    std::string meshName = mesh->GetName();
    UIText* textElement;
    if (m_textElements.find(meshName) != m_textElements.end()) {
        return m_textElements[meshName];
    }
    textElement = new UIText();
    textElement->SetMesh(mesh);
    textElement->Init();
    textElement->SetText("Texto inicial.");
    textElement->SetFontSize(24.0f);
    textElement->SetPosition(10.0f, 10.0f); // Posición inicial del texto
    textElement->CreateMesh(vertexDef);
    m_textElements[meshName] = textElement;
    return textElement;
}

UIText* UIManager::UpdateText(std::string meshName, std::string text) {
    auto it = m_textElements.find(meshName);
    if (it == m_textElements.end()) {
        return nullptr;
    }
    UIText* textElement = it->second;
    textElement->SetText(text);
    return textElement;
}

HRESULT UIManager::Init(EngineContext* context)
{
    ManagerBase::Init(context);

    OutputDebugStringA("Incializando UIManager...\n");
    m_deviceManager = ManagerLocator::GetManager<DeviceManager>();
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

bool UIManager::CreatePanel(const std::string& name, const UIPanelDesc& desc) {
    if (m_panels.find(name) != m_panels.end()) return false;
    m_panels[name] = desc;
    return true;
}

bool UIManager::AddLabelToPanel(const std::string& panelName, const std::string& labelMeshName) {
    auto it = m_panels.find(panelName);
    if (it == m_panels.end()) return false;
    m_panelLabels[panelName].push_back(labelMeshName);
    return true;
}

//HRESULT UIManager::InitText(std::vector<std::shared_ptr<VertexDefinition::VertexVariant>> vertexDef)
//{
//    std::shared_ptr<UIText> textMesh = m_renderManager->SceneManagerGet()->RegisterTextMesh("UITextMesh");
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

void UIManager::FillCommandBuffer(CommandBuffer& buffer) {
    // Emit panel quads first
    for (auto& kv : m_panels) {
        const UIPanelDesc& d = kv.second;
        buffer.AddCommand<DrawQuadCommand>(d.x, d.y, d.w, d.h);
        // Optionally emit attached labels as quads positioned inside panel (simple layout)
        auto it = m_panelLabels.find(kv.first);
        if (it != m_panelLabels.end()) {
            float lx = d.x + 0.02f;
            float ly = d.y + 0.02f;
            for (const auto& meshName : it->second) {
                // Try to find the UIText by meshName and emit a DrawTextCommand if available
                auto tit = m_textElements.find(meshName);
                if (tit != m_textElements.end()) {
                    UIText* txt = tit->second;
                    std::string s = "";
                    try { s = txt->GetText(); } catch (...) { s = ""; }
                    buffer.AddCommand<DrawTextCommand>(s, lx, ly, 16.0f, 1.0f, 1.0f, 1.0f, 1.0f);
                } else {
                    buffer.AddCommand<DrawQuadCommand>(lx, ly, 0.1f, 0.03f);
                }
                ly += 0.035f;
            }
        }
    }
}

void UIManager::Shutdown()
{
    /*for (UIElement* element : uiElements)
    {
        SafeRelease(element);
    }
    uiElements.clear();*/
    for (auto& textElement : m_textElements)
    {
        delete(textElement.second);
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
