// MainController.cpp
#include "MainController.h"
#include <Defines/VertexDefinition.h>
#include <DefineLocator/DefineLocator.h>
#include <ConfigLocator/ConfigLocator.h>
#include <ServiceLocator/ServiceLocator.h>

MainController::MainController(HWND hwnd, int width, int height)
    : m_hwnd(hwnd), m_width(width), m_height(height), m_initController(nullptr) {
}

MainController::~MainController() {
    Shutdown(); // Asegurarse de que todo se libere al destruir el controlador
}

HRESULT MainController::Initialize(HWND hwnd, int width, int height) {
	m_initController = new InitController();
    if (!m_initController) {
        return E_FAIL;
	}

    // 1. Inicializar definiciones básicas
    HRESULT hr = DefineLocator::InitializeDefines();

    // 2. Clases de configuración (clases autocontruidas a partir de los .yml de configuracion)
    hr = ConfigLocator::CreateConfigs();
    if (FAILED(hr)) {
        OutputDebugStringA("MainController Init: Failed to initialize services.\n");
        return hr;
    }

    m_config = ConfigLocator::GetConfig<EngineConfig>();
    if (!m_config) {
        return E_FAIL;
	}
    m_serviceConfig = ConfigLocator::GetConfig<ServiceConfig>();
    if (!m_serviceConfig) {
        return E_FAIL;
    }
    std::shared_ptr<BaseIndexConfig> m_assetBaseConfig = ConfigLocator::GetConfig<BaseIndexConfig>();
    if (!m_assetBaseConfig) {
        OutputDebugStringA("MainController Init: Failed to get BaseIndexConfig.\n");
        return E_FAIL;
	}
    std::shared_ptr<MainIndexConfig> m_assetConfig = ConfigLocator::GetConfig<MainIndexConfig>();
    if (!m_assetConfig) {
        OutputDebugStringA("MainController Init: Failed to get BaseIndexConfig.\n");
        return E_FAIL;
    }
	std::shared_ptr<GameEngineConfig> m_gameEngineConfig = ConfigLocator::GetConfig<GameEngineConfig>();
    if (!m_gameEngineConfig) {
        OutputDebugStringA("MainController Init: Failed to get GameEngineConfig.\n");
        return E_FAIL;
	}

    std::map<std::string, std::vector<std::string>> components = {};
    components[COMPONENT_MANAGER.data()] = m_config->managers_init_order;
    components[COMPONENT_SERVICE.data()] = m_serviceConfig->services_init;
    components[COMPONENT_ASSET.data()] = m_assetConfig->mainIndex;
    components[COMPONENT_ASSET_BASE.data()].append_range(m_assetBaseConfig->index);
    components[COMPONENT_GAME_SERVICE.data()] = m_gameEngineConfig->services_init_order;

    for(const auto& component : components) {
        const std::string& componentTypeName = component.first;
        const std::vector<std::string>& componentsNames = component.second;

        for (const auto& componentName : componentsNames) {
			// Obtener el nombre del componente y su configuración
            const std::string& componentConfig = componentName + "Config";
            auto config = ConfigLocator::GetConfig<ConfigBase>(componentConfig);
            if (!config) {
                OutputDebugStringA(("MainController Init: Failed to get config for " + componentName + "\n").c_str());
                continue;
            }
            std::vector<std::string> dependencies; // Aquí puedes agregar las dependencias si las tienes

            // Registrar el componente en InitController
            m_initController->RegisterComponent(componentTypeName, componentName, config->dependencies);
            // Si tienes una configuración específica para el componente, puedes usarla aquí
            // Por ejemplo, si tienes ConfigBase para cada componente, podrías hacer algo como:
            //
        }
        
	}
 
    std::vector<std::pair<std::string, std::string>> initOrder = m_initController->GetInitializationOrder();

    for(const auto& [componentName, componentType] : initOrder) {
        if (componentType == COMPONENT_MANAGER) 
        {
            // Inicializar el manager
            HRESULT hr = ManagerLocator::InitializeManagers({ componentName }, m_hwnd, m_width, m_height);
            if (FAILED(hr)) {
                OutputDebugStringA(("MainController Init: Failed to initialize manager " + componentName + "\n").c_str());
                return hr;
            }
        } 
        else if (componentType == COMPONENT_SERVICE) {
            // Inicializar el servicio
            HRESULT hr = ServiceLocator::InitializeServices({ componentName });
            if (FAILED(hr)) {
                OutputDebugStringA(("MainController Init: Failed to initialize service " + componentName + "\n").c_str());
                return hr;
            }
        } 
        else if (componentType == COMPONENT_ASSET_BASE) {
            // Inicializar el asset
			HRESULT hr = AssetLocator::InitializeBaseAssets({ componentName });
            if (FAILED(hr)) {
                OutputDebugStringA(("MainController Init: Failed to initialize asset " + componentName + "\n").c_str());
                return hr;
            }
        }       
        else if (componentType == COMPONENT_ASSET) {
            // Inicializar el asset
            HRESULT hr = AssetLocator::InitializeAssets({ componentName });
            if (FAILED(hr)) {
                OutputDebugStringA(("MainController Init: Failed to initialize asset " + componentName + "\n").c_str());
                return hr;
            }
        }
        else if (componentType == COMPONENT_GAME_SERVICE) {
            // Inicializar el asset
            HRESULT hr = ServiceLocator::InitializeServices({ componentName });
            if (FAILED(hr)) {
                OutputDebugStringA(("MainController Init: Failed to initialize game service " + componentName + "\n").c_str());
                return hr;
            }
        }
	}

    m_keyboard = ServiceLocator::GetService<Keyboard>();
    m_mouse = ServiceLocator::GetService<Mouse>();

	m_renderManager = ManagerLocator::GetManager<RenderManager>();

    return S_OK;
}

void MainController::Update(float deltaTime) {
	ServiceLocator::UpdateServices(m_serviceConfig->services_update_order, deltaTime);
    ManagerLocator::UpdateManagers(m_config->managers_update_order, deltaTime);
}

void MainController::Render() {
    m_renderManager->ExecRender();
    /*m_renderManager->BeginRender();
    ServiceLocator::RenderServices(m_serviceConfig->services_render_order);
    m_renderManager->Render();
    m_renderManager->EndRender();*/    
}

void MainController::Shutdown() {
    // Liberar subsistemas en orden inverso (o el que sea adecuado para dependencias)
    for (auto it = m_subsystems.rbegin(); it != m_subsystems.rend(); ++it) {
        (*it)->Release();
    }
    m_subsystems.clear(); // Limpiar la lista

    // Limpiar el Service Locator (libera los shared_ptr de los managers)
    ManagerLocator::GetInstance().Shutdown();

    // Los shared_ptr de los managers de MainController se liberarán solos
    // cuando el MainController sea destruido.
}

// Métodos para pasar eventos de ventana
void MainController::HandleKeyboardInput(WPARAM wParam, LPARAM lParam) {
    // Pasa el input al KeyboardManager, no directamente al teclado crudo
    //if (m_keyboardManager) {
    //    m_keyboardManager->ProcessKeyboardInput(wParam, lParam); // Asumiendo un método para procesar raw input
    //}
}

void MainController::HandleMouseInput(WPARAM wParam, LPARAM lParam) {
    //if (m_rawMouse) {
    //    m_rawMouse->Update(m_hwnd); // O un método específico para WM_MOUSEMOVE
    //}
}

void MainController::ToggleFullscreen() {
    // Implementa la lógica de la ventana aquí o en un WindowManager si lo creas como servicio
    // Por ahora, como está en MainWindow, esto es un puente.
    // Idealmente, un WindowManager sería un servicio y se le pediría.
}