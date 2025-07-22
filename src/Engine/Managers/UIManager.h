// UIManager.h
#pragma once
#include <Windows.h>
#include <string>
#include <vector>
#include "IManager.h"
#include "IInitializable.h"
#include "IRenderable.h"
#include "IUpdatable.h"
#include "IShutdownable.h"
#include <directxmath.h>
#include "UI/UIElement.h"
#include "DeviceManager.h"
#include "RenderTargetManager.h"
#include <ManagerLocator/ManagerLocator.h>

class UIManager : public IManager, public IInitializable, public IRenderable, public IUpdatable, public IShutdownable
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

    HRESULT Init() override;
    void Render() override;
    void Update(float deltaTime) override {};
    void Shutdown() override;

    void AddElement(UIElement* element);
    void RemoveElement(UIElement* element);
    std::vector<UIElement*> uiElements;

    XMMATRIX GetOrthoMatrix() const { return m_orthoMatrix; };

private:
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<RenderTargetManager> m_renderTargetManager;
	std::shared_ptr<WorldMatrixManager> m_worldMatrixManager;
	XMMATRIX m_orthoMatrix; // Matriz ortográfica para UI
};