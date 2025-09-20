// UIManager.h
#pragma once
#include <Windows.h>
#include <directxmath.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <ManagerBase.h>
#include <Defines/VertexDefinition.h>
#include <UI/UIText.h>

class DeviceManager;
class MeshAsset;
//class UIText;

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
    void Render() override;
    void Update(float deltaTime) override {};
    UIText* UpdateText(std::string meshName, std::string text);
    void Shutdown() override;

	//void SetText(const std::wstring& text);
    /*void AddElement(UIElement* element);
    void RemoveElement(UIElement* element);*/
    //std::vector<UIElement*> uiElements;

    XMMATRIX GetOrthoMatrix() const { return m_orthoMatrix; };

private:
	std::map<std::string, UIText*> m_textElements;
    std::shared_ptr<DeviceManager> m_deviceManager;        
	// Matriz ortográfica para UI
	XMMATRIX m_orthoMatrix; // Matriz ortográfica para UI
};