#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <string>
#include <memory>
#include <map>
#include <vector>

#include <ManagerBase.h>

class EngineConfig;
class ServiceConfig;
class MainIndexConfig;
class BaseIndexConfig;
class GameEngineConfig;
class InitController;
class InitPipelineManager;

class InitManager : public ManagerBase
{
private:
    std::shared_ptr<EngineConfig> m_config;
    std::shared_ptr<ServiceConfig> m_serviceConfig;
    std::shared_ptr<MainIndexConfig> m_assetConfig;
    std::shared_ptr<BaseIndexConfig> m_assetBaseConfig;
    std::shared_ptr<GameEngineConfig> m_gameEngineConfig;

    std::shared_ptr<InitController> m_initController;
public:
    InitManager();
    ~InitManager() override;

    HRESULT Init(EngineContext* context) override;
    HRESULT InitBase();
    HRESULT InitConfigs();
	HRESULT InitComponents(EngineContext* context);
    HRESULT ExtractComponents();
    HRESULT PostInit();
    HRESULT InitPipeline(EngineContext* context);

    const std::string& GetManagerName() const override {
        static const std::string name = "InitManager";
        return name;
    }

    static const std::string& GetStaticManagerName() {
        static const std::string name = "InitManager";
        return name;
    }
 
};