#pragma once
#include <IService.h>
#include <UIManager.h>
#include <d3d11.h>
//#include <wrl/client.h>
#include <DirectXMath.h>
#include <DeviceManager.h>
#include <ShaderManager.h>
#include <CameraManager.h>
#include <WorldMatrixManager.h>
#include <Services/Material.h>
#include <UI/UIText.h>
#include <Config/Services/UIDebugConfig.h>
#include <Config/ITextureConfig.h>
#include <ManagerLocator/ManagerLocator.h>
#include <ConfigLocator/ConfigLocator.h>
#include <d3dcompiler.h>
#include <TextureLoader/TextureLoader.h>
#include "../../../Resources/resource.h"
#include <Util/Text/Text.h> 

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
    UIDebugConfig* m_config;
    std::shared_ptr<CameraManager> m_cameraManager;
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<ShaderManager> m_shaderManager;
    std::shared_ptr<UIManager> m_UIManager;
	std::shared_ptr<WorldMatrixManager> m_worldMatrixManager;
    Material* g_materialText;
    UIText* m_text1;
    UIText* m_text2;
    UIText* m_cameraUIText;
};