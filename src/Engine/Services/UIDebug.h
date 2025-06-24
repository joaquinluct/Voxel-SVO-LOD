#pragma once
#include "IService.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <DeviceManager.h>
#include <ShaderManager.h>
#include <UIManager.h>
#include <WorldMatrixManager.h>
#include <Material/Material.h>
#include <UI/UIText.h>
#include <Config/Services/UIDebugConfig.h>

class UIDebug : public IService {
public:
    UIDebug();
    ~UIDebug() override;

    HRESULT Init() override;
    void Render() override;
    void Shutdown() override;
    void Update(float) override;

    const std::string& GetServiceName() const override {
        static const std::string name = "UIDebug";
        return name;
    }

    static const std::string& GetStaticManagerName() {
        static const std::string name = "UIDebug";
        return name;
    }

private:
    UIDebugConfig::Values* m_config;
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<ShaderManager> m_shaderManager;
    std::shared_ptr<UIManager> m_UIManager;
	std::shared_ptr<WorldMatrixManager> m_worldMatrixManager;
    Material* g_materialText;
    UIText* myText;
};