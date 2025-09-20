#include "InitManager.h"
#include <Config/Assets/Base/BaseIndexConfig.h>
#include <Config/Assets/Base/MainIndexConfig.h>
#include <Config/Base/Managers/EngineConfig.h>
#include <Config/Services/ServiceConfig.h>
#include <Defines/Components.h>
#include <InitManager/InitController.h>
#include <InitPipelineManager.h>
#include <Locators/AssetLocator/AssetLocator.h>
#include <Locators/ConfigLocator/ConfigLocator.h>
#include <Locators/DefineLocator/DefineLocator.h>
#include <Locators/ManagerLocator/ManagerLocator.h>
#include <Locators/Pipeline/PipelineStateLocator.h>
#include <Locators/Pipeline/RenderPassLocator.h>
#include <Locators/ServiceLocator/ServiceLocator.h>
#include <Util/Text/Text.h>
#include <Windows.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <TinyObjLoader/tiny_obj_loader.h>

#include "REGISTER_MANAGER_MACRO.h"

REGISTER_MANAGER_TYPE(InitManager, "InitManager")

InitManager::InitManager()
{
}

InitManager::~InitManager()
{
}


HRESULT InitManager::InitBase() {
    // 1. Inicializar básicas
    // ----------------------
    m_initController = std::make_shared<InitController>();

    // 1.1 Inicializar defines
    HRESULT hr = DefineLocator::InitializeDefines();
    if (FAILED(hr)) {
        OutputDebugStringA("[InitManager] Init: Failed to initialize defines.\n");
        return hr;
    }

    // 1.2 Inicializar render passes
    hr = RenderPassLocator::CreateRenderPasss();
    if (FAILED(hr)) {
        OutputDebugStringA("[InitManager] Init: Failed to initialize render passes.\n");
        return hr;
    }

    // 1.3 Inicializar pipeline states  
    hr = PipelineStateLocator::CreatePipelineStates();
    if (FAILED(hr)) {
        OutputDebugStringA("[InitManager] Init: Failed to initialize pipeline states.\n");
        return hr;
    }

    // 1.4 Inicializar render passes
    hr = ConfigLocator::CreateConfigs();
    if (FAILED(hr)) {
        OutputDebugStringA("[InitManager] Init: Failed to initialize services.\n");
        return hr;
    }
    return S_OK;
}

HRESULT InitManager::InitConfigs() {
    // 2. Clases de configuración
    // --------------------------

    // 2.1 EngineConfig
    m_config = ConfigLocator::GetConfig<EngineConfig>();
    if (!m_config) {
        return E_FAIL;
    }
    // 2.2 ServiceConfig
    m_serviceConfig = ConfigLocator::GetConfig<ServiceConfig>();
    if (!m_serviceConfig) {
        return E_FAIL;
    }
    // 2.3 AssetBaseConfig
    m_assetBaseConfig = ConfigLocator::GetConfig<BaseIndexConfig>();
    if (!m_assetBaseConfig) {
        OutputDebugStringA("[InitManager] Init: Failed to get BaseIndexConfig.\n");
        return E_FAIL;
    }
    // 2.4 AssetConfig
    m_assetConfig = ConfigLocator::GetConfig<MainIndexConfig>();
    if (!m_assetConfig) {
        OutputDebugStringA("[InitManager] Init: Failed to get BaseIndexConfig.\n");
        return E_FAIL;
    }
    // 2.5 GameEngineConfig
    m_gameEngineConfig = ConfigLocator::GetConfig<GameEngineConfig>();
    if (!m_gameEngineConfig) {
        OutputDebugStringA("[InitManager] Init: Failed to get GameEngineConfig.\n");
        return E_FAIL;
    }
    return S_OK;
}

HRESULT InitManager::ExtractComponents() {
    // 3. Obtener todos los commponentes para determinar el orden de dependencia
    // -------------------------------------------------------------------------
    std::map<std::string, std::vector<std::string>> components = {};
    components[COMPONENT_MANAGER.data()] = m_config->managers_init_order;
    components[COMPONENT_SERVICE.data()] = m_serviceConfig->services_init;
    components[COMPONENT_ASSET.data()] = m_assetConfig->mainIndex;
    components[COMPONENT_ASSET_BASE.data()].append_range(m_assetBaseConfig->index);
    components[COMPONENT_GAME_SERVICE.data()] = m_gameEngineConfig->services_init_order;

    for (const auto& component : components) {
        const std::string& componentTypeName = component.first;
        const std::vector<std::string>& componentsNames = component.second;

        for (const auto& componentName : componentsNames) {
            // Obtener el nombre del componente y su configuración
            const std::string& componentConfig = componentName + "Config";
            auto config = ConfigLocator::GetConfig<ConfigBase>(componentConfig);
            if (!config) {
                OutputDebugStringA(("**InitManager::ExtractComponents: No se ha podido obtener la configuración de " + componentName + "\n").c_str());
                continue;
            }
            // Registrar el componente en InitController
            m_initController->RegisterComponent(componentTypeName, componentName, config->dependencies);
        }
    }
    return S_OK;
}

HRESULT InitManager::InitComponents(EngineContext* context)
{
    // 4. Establecer el orden de inicilización por dependencia
    // -------------------------------------------------------
    std::vector<std::pair<std::string, std::string>> initOrder = m_initController->GetInitializationOrder();

    // 5. Ejecutar la inicilización en el orden establecido por dependencia
    // --------------------------------------------------------------------
    for (const auto& [componentName, componentType] : initOrder) {

        if (componentName == "SceneManager") {
            bool a = false;
        }

        if (componentType == COMPONENT_MANAGER)
        {
            // Inicializar el manager
            HRESULT hr = ManagerLocator::InitializeManagers({ componentName }, context);
            if (FAILED(hr)) {
                OutputDebugStringA(("[InitManager] Init: Failed to initialize manager " + componentName + "\n").c_str());
                return hr;
            }
        }
        else if (componentType == COMPONENT_SERVICE) {
            // Inicializar el servicio
            HRESULT hr = ServiceLocator::InitializeServices({ componentName });
            if (FAILED(hr)) {
                OutputDebugStringA(("[InitManager] Init: Failed to initialize service " + componentName + "\n").c_str());
                return hr;
            }
        }
        else if (componentType == COMPONENT_ASSET_BASE) {
            // Inicializar el asset
            HRESULT hr = AssetLocator::InitializeBaseAssets({ componentName });
            if (FAILED(hr)) {
                OutputDebugStringA(("[InitManager] Init: Failed to initialize asset " + componentName + "\n").c_str());
                return hr;
            }
        }
        else if (componentType == COMPONENT_ASSET) {
            // Inicializar el asset
            HRESULT hr = AssetLocator::InitializeAssets({ componentName });
            if (FAILED(hr)) {
                OutputDebugStringA(("[InitManager] Init: Failed to initialize asset " + componentName + "\n").c_str());
                return hr;
            }
        }
        else if (componentType == COMPONENT_GAME_SERVICE) {
            // Inicializar el asset
            HRESULT hr = ServiceLocator::InitializeServices({ componentName });
            if (FAILED(hr)) {
                OutputDebugStringA(("[InitManager] Init: Failed to initialize game service " + componentName + "\n").c_str());
                return hr;
            }
        }
    }
    return S_OK;
}

HRESULT InitManager::InitPipeline(EngineContext* context)
{
    OutputDebugStringA("[InitManager] Inicialización del Pipeline...\n");
    HRESULT hr = ManagerLocator::InitializeManagers({ "InitPipelineManager" }, context);
    if (FAILED(hr)) {
        return hr;
    }
    std::shared_ptr<InitPipelineManager> initPipelineManager = ManagerLocator::GetManager<InitPipelineManager>();
    if (!initPipelineManager) {
        return E_FAIL;
    }
    //initPipelineManager->Init(context);
    return S_OK;
}

HRESULT InitManager::Init(EngineContext* context)
{
    HRESULT hr = InitBase();
    if (FAILED(hr)) {
        return hr;
    }

    OutputDebugStringA("[InitManager] inicialización base - OK.\n");

    hr = InitConfigs();
    if (FAILED(hr)) {
        return hr;
    }

    OutputDebugStringA("[InitManager] inicialización de Configuración - OK.\n");

    hr = ExtractComponents();
    if (FAILED(hr)) {
        return hr;
    }

    hr = InitComponents(context);
    if (FAILED(hr)) {
        return hr;
    }

    OutputDebugStringA("[InitManager] inicialización de Componentes - OK.\n");

    hr = PostInit();
    if (FAILED(hr)) {
        return hr;
    }

    OutputDebugStringA("[InitManager] Post-inicialización - OK.\n");

    hr = InitPipeline(context);
    if (FAILED(hr)) {
        return hr;
    }

    OutputDebugStringA("[InitManager] Inicialización del pipeline - OK.\n");

    return S_OK;
}

HRESULT InitManager::PostInit()
{
    OutputDebugStringA("[InitManager] Comenzando la Post-inicialización...\n");
    // 3. Obtener todos los commponentes para determinar el orden de dependencia
    // -------------------------------------------------------------------------
    std::map<std::string, std::vector<std::string>> components = {};
    components[COMPONENT_MANAGER.data()] = m_config->managers_post_init;
    components[COMPONENT_SERVICE.data()] = m_serviceConfig->services_post_init;
    components[COMPONENT_ASSET.data()] = m_assetConfig->post_init;
    components[COMPONENT_ASSET_BASE.data()].append_range(m_assetBaseConfig->post_init);
    components[COMPONENT_GAME_SERVICE.data()] = m_gameEngineConfig->services_post_init;

    for (const auto& component : components) {
        const std::string& componentTypeName = component.first;
        const std::vector<std::string>& componentsNames = component.second;
        for (const auto& componentName : componentsNames) {
            if (StrToLower(componentName) == "none") {
                continue;
            }
            if (componentTypeName == COMPONENT_MANAGER)
            {
                std::shared_ptr<ManagerBase> component = ManagerLocator::GetManager(componentName);
                if (component) {
                    HRESULT hr = component->PostInit();
                    if (FAILED(hr)) {
                        OutputDebugStringA(("[InitManager] PostInit: Failed to post-initialize manager " + componentName + "\n").c_str());
                        return hr;
                    }
                }
            }
            else if (componentTypeName == COMPONENT_SERVICE) {
                std::shared_ptr<IService> component = ServiceLocator::GetService(componentName);
                if (component) {
                    HRESULT hr = component->PostInit();
                    if (FAILED(hr)) {
                        OutputDebugStringA(("[InitManager] PostInit: Failed to post-initialize service " + componentName + "\n").c_str());
                        return hr;
                    }
                }
            }
            else if (componentTypeName == COMPONENT_ASSET_BASE) {
                // Inicializar el asset
                std::shared_ptr<AssetBase> component = AssetLocator::GetAssetBase(componentName);
                if (component) {
                    HRESULT hr = component->PostInit();
                    if (FAILED(hr)) {
                        OutputDebugStringA(("[InitManager] PostInit: Failed to post-initialize asset " + componentName + "\n").c_str());
                        return hr;
                    }
                }
            }
            else if (componentTypeName == COMPONENT_ASSET) {
                // Inicializar el asset
                std::shared_ptr<AssetBase> component = AssetLocator::GetAsset(componentName);
                if (component) {
                    HRESULT hr = component->PostInit();
                    if (FAILED(hr)) {
                        OutputDebugStringA(("[InitManager] PostInit: Failed to post-initialize asset " + componentName + "\n").c_str());
                        return hr;
                    }
                }
            }
            else if (componentTypeName == COMPONENT_GAME_SERVICE) {
                // Inicializar el asset
                std::shared_ptr<IService> component = ServiceLocator::GetService(componentName);
                if (component) {
                    HRESULT hr = component->PostInit();
                    if (FAILED(hr)) {
                        OutputDebugStringA(("[InitManager] PostInit: Failed to post-initialize asset " + componentName + "\n").c_str());
                        return hr;
                    }
                }
            }
        }
    }
    return S_OK;
}
