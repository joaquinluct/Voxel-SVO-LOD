// ServiceLocator.cpp
#include "ServiceLocator.h"
#include <iostream> // Para debugging si quieres usar std::cout en lugar de OutputDebugStringA
#include <windows.h> // Para OutputDebugStringA
#include <string> // Para std::to_string

std::map<std::string, ServiceLocator::ServiceEntry>& ServiceLocator::GetServiceEntries() {
    static std::map<std::string, ServiceEntry> s_serviceEntries; 
    return s_serviceEntries;
}

ServiceLocator& ServiceLocator::GetInstance() {
    static ServiceLocator instance; // Se crea una vez, la primera vez que se llama
    return instance;
}

void ServiceLocator::RegisterServiceCreator(
    const std::string& name,
    CreateServiceLambda createFn,
    InitializeServiceLambda initFn,
    RenderServiceLambda renderFn,
    UpdateServiceLambda updateFn) {
    auto& entries = ServiceLocator::GetServiceEntries();
    if (entries.count(name)) {
        OutputDebugStringA(("WARNING: Service creator for '" + name + "' already registered. Overwriting.\n").c_str());
    }
    //s_serviceCreators[name] = { createFn, initFn };
    entries[name] = {nullptr, createFn, initFn, renderFn, updateFn};
}

HRESULT ServiceLocator::InitializeServices(const std::vector<std::string>& orderList) {
    auto& entries = ServiceLocator::GetServiceEntries();
    for (const std::string& serviceName : orderList) {
        auto it = entries.find(serviceName);
        if (it != entries.end()) {
			// 0. Obtener la configuración específica del servicio si existe
            const std::string configName = serviceName + "Config";
            std::shared_ptr<ConfigService> config = ConfigLocator::GetConfig<ConfigService>(configName);
            
            // 1. Crear la instancia del manager
            it->second.instance = it->second.creator();
            if (!it->second.instance) {
                OutputDebugStringA(("ERROR: Failed to create instance for service '" + serviceName + "'\n").c_str());
                return E_FAIL;
            }

			// 1.1 Aplicar la configuración si existe
			config && config.get()->enabled ? it->second.instance->Start() : it->second.instance->Stop();

            // 2. Inicializar la instancia (pasando la sub-nodo de configuración específica si existe)
            // configRoot[serviceName] asegura que se le pasa solo la configuración relevante a ese manager.
            HRESULT hr = it->second.initializer(it->second.instance);
            if (FAILED(hr)) {
                OutputDebugStringA(("ERROR: Failed to initialize service '" + serviceName + "'\n").c_str());
                return hr;
            }
            
            std::shared_ptr<IService> service = std::static_pointer_cast<IService>(it->second.instance);
            service->SetNeedsShadowPass(config && config->shadow_pass);
        }
        else {
            OutputDebugStringA(("ERROR: Service '" + serviceName + "' in init order list but not registered.\n").c_str());
            return E_FAIL;
        }
    }
    return S_OK;
}

std::shared_ptr<IService> ServiceLocator::GetService(const std::string& name) {
    auto& entries = ServiceLocator::GetServiceEntries();
    auto it = entries.find(name);
    if (it != entries.end()) {
        return entries[name].instance;
    }
    return nullptr;
}

HRESULT ServiceLocator::RenderServices(const std::vector<std::string>& orderList) {
    auto& entries = ServiceLocator::GetServiceEntries();
    for (const std::string& serviceName : orderList) {
        auto it = entries.find(serviceName);
        if (it != entries.end() && it->second.instance && it->second.renderer) {
            if (it->second.instance.get()->IsRunning()) {
                it->second.renderer(it->second.instance);
            }
            else {
                OutputDebugStringA(("WARNING: Service '" + serviceName + "' is not running, skipping render.\n").c_str());
            }
        }
        else {
            // Un manager puede no ser renderizable, así que esto podría ser un warning si no implementa Render
            // o un error si debería hacerlo.
            OutputDebugStringA(("WARNING: Service '" + serviceName + "' not found, instance null, or not renderable.\n").c_str());
        }
    }
    return S_OK;
}

HRESULT ServiceLocator::RenderShadowPassServices(const std::vector<std::string>& orderList) {
    auto& entries = ServiceLocator::GetServiceEntries();
    for (const std::string& serviceName : orderList) {
        auto it = entries.find(serviceName);
        if (it != entries.end() && it->second.instance && it->second.renderer && it->second.instance->NeedsShadowPass()) {
            if (it->second.instance.get()->IsRunning()) {
                it->second.renderer(it->second.instance);
            }
            else {
                OutputDebugStringA(("WARNING: Service '" + serviceName + "' is not running, skipping render shadow pass.\n").c_str());
            }
        }
    }
    return S_OK;
}

HRESULT ServiceLocator::UpdateServices(const std::vector<std::string>& orderList, float deltaTime) {
    auto& entries = ServiceLocator::GetServiceEntries();
    for (const std::string& serviceName : orderList) {
        auto it = entries.find(serviceName);
        if (it != entries.end() && it->second.instance && it->second.updater) {
            it->second.updater(it->second.instance, deltaTime);
        }
        else {
            // Un manager puede no ser renderizable, así que esto podría ser un warning si no implementa Render
            // o un error si debería hacerlo.
            OutputDebugStringA(("WARNING: Service '" + serviceName + "' not found, instance null, or not renderable.\n").c_str());
        }
    }
    return S_OK;
}

void ServiceLocator::Shutdown() {
    auto& entries = ServiceLocator::GetServiceEntries();
	entries.clear(); // Limpia las entradas de servicios
    
    OutputDebugStringA("ServiceLocator: All services and creators shut down.\n");
}