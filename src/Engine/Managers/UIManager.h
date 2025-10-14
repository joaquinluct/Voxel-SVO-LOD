// UIManager.h
#pragma once
#include <Assets/Base/UIAsset.h>
#include <Engine/Rendering/RenderCommand.h>
#include <Defines/EngineDefinition.h>
#include <Defines/VertexDefinition.h>
#include <directxmath.h>
#include <ManagerBase.h>
#include <map>
#include <memory>
#include <string>
#include <UI/UIText.h>
#include <vector>
#include <unordered_map>
#include <Windows.h>

class DeviceManager;
class MeshAsset;
//class UIText;
class CommandBuffer;

class UIManager : public ManagerBase
{
public:
    UIManager();
    ~UIManager() override;
    const std::string& GetManagerName() const override {
        static const std::string name = "UIManager";
        return name;
    }
    static const std::string& GetStaticManagerName()
    {
        static const std::string name = "UIManager";
        return name;
    }

    HRESULT Init(EngineContext* context) override;
    UIText* InitText(std::shared_ptr<MeshAsset> mesh, std::vector<std::shared_ptr<VertexDefinition::VertexVariant>>& vertexDef);
    // Convenience: create a label entry (will require a mesh to render)
    UIText* CreateLabel(const std::string& name, const std::string& text);
    void Render() override;
    void Update(float deltaTime) override {};
    UIText* UpdateText(std::string meshName, std::string text);
    void Shutdown() override;

    // Produce UI render commands into a CommandBuffer (called by SceneManager)
    void FillCommandBuffer(CommandBuffer& buffer);

    // Basic panel widget API (minimal retained-mode)
    struct UIPanelDesc {
        float x = 0.0f; // normalized 0..1
        float y = 0.0f; // normalized 0..1
        float w = 0.2f;
        float h = 0.1f;
        float color[4] = {0.2f, 0.2f, 0.2f, 1.0f};
    };

    // Create a panel (returns false if already exists)
    bool CreatePanel(const std::string& name, const UIPanelDesc& desc);
    // Attach an existing label (UIText by mesh name) to a panel for layout
    bool AddLabelToPanel(const std::string& panelName, const std::string& labelMeshName);

    //void SetText(const std::wstring& text);
    /*void AddElement(UIElement* element);
    void RemoveElement(UIElement* element);*/
    //std::vector<UIElement*> uiElements;

    XMMATRIX GetOrthoMatrix() const { return m_orthoMatrix; };

    MeshAssetBase* GetUIMesh() const { return  m_uiAsset; }

private:
    UIAsset* m_uiAsset = nullptr;

    std::map<std::string, UIText*> m_textElements;
    std::shared_ptr<DeviceManager> m_deviceManager;
    // Matriz ortográfica para UI
    XMMATRIX m_orthoMatrix; // Matriz ortográfica para UI
    // Panels storage
    std::unordered_map<std::string, UIPanelDesc> m_panels;
    std::unordered_map<std::string, std::vector<std::string>> m_panelLabels;
};
