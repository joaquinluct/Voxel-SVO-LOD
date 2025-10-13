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
};
