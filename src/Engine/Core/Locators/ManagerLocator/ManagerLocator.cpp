// ManagerLocator.cpp
#include "ManagerLocator.h"
#include <iostream> // Para debugging si quieres usar std::cout en lugar de OutputDebugStringA
#include <windows.h> // Para OutputDebugStringA
#include <string> // Para std::to_string
#include <Defines/EngineDefinition.h>

std::map<std::string, ManagerLocator::ManagerEntry>& ManagerLocator::GetManagerEntries() {
    static std::map<std::string, ManagerEntry> s_managerEntries; 
    return s_managerEntries;
}

ManagerLocator& ManagerLocator::GetInstance() {
    static ManagerLocator instance; // Se crea una vez, la primera vez que se llama
    return instance;
}

void ManagerLocator::RegisterManagerCreator(
    const std::string& name,
    CreateManagerLambda createFn,
    InitializeManagerLambda initFn,
    RenderManagerLambda renderFn,
    UpdateManagerLambda updateFn) {
    auto& entries = ManagerLocator::GetManagerEntries();
    if (entries.count(name)) {
        OutputDebugStringA(("WARNING: Manager creator for '" + name + "' already registered. Overwriting.\n").c_str());
    }
    //s_managerCreators[name] = { createFn, initFn };
    entries[name] = {nullptr, createFn, initFn, renderFn, updateFn};
}

HRESULT ManagerLocator::InitializeManagers(const std::vector<std::string>& orderList, EngineContext* context) {
    auto& entries = ManagerLocator::GetManagerEntries();
    for (const std::string& managerName : orderList) {
        OutputDebugStringA(("[ManagerLocator] Comenzando la inicialización de " + managerName + " ...\n").c_str());
        if (managerName == "UpdateManager") {
            bool a = false;
        }
        auto it = entries.find(managerName);
        if (it != entries.end()) {
            // 1. Crear la instancia del manager
            it->second.instance = it->second.creator();
            if (!it->second.instance) {
                OutputDebugStringA(("[ManagerLocator] ERROR: Failed to create instance for manager '" + managerName + "'\n").c_str());
                return E_FAIL;
            }

            // 2. Inicializar la instancia (pasando la sub-nodo de configuración específica si existe)
            // configRoot[managerName] asegura que se le pasa solo la configuración relevante a ese manager.
            HRESULT hr = S_OK;
			bool isWindowDependent = it->second.instance->IsWindowDependent();
			hr = it->second.initializer(it->second.instance, context);                
            if (FAILED(hr)) {
                OutputDebugStringA(("[ManagerLocator] ERROR: Failed to initialize manager '" + managerName + "'\n").c_str());
                return hr;
            }
            it->second.instance->SetInitialized(true);
            OutputDebugStringA(("[ManagerLocator] Inicialización de " + managerName + " - OK\n").c_str());
        }
        else {
            OutputDebugStringA(("[ManagerLocator] ERROR: Manager '" + managerName + "' in init order list but not registered.\n").c_str());
            return E_FAIL;
        }
    }
    return S_OK;
}

std::shared_ptr<ManagerBase> ManagerLocator::GetManager(const std::string& name) {
    auto& entries = ManagerLocator::GetManagerEntries();
    auto it = entries.find(name);
    if (it != entries.end()) {
        return entries[name].instance;
    }
    return nullptr;
}

HRESULT ManagerLocator::RenderManagers(const std::vector<std::string>& orderList) {
    auto& entries = ManagerLocator::GetManagerEntries();
    for (const std::string& managerName : orderList) {
        auto it = entries.find(managerName);
        if (it != entries.end() && it->second.instance && it->second.renderer) {
            it->second.renderer(it->second.instance);
        }
        else {
            // Un manager puede no ser renderizable, así que esto podría ser un warning si no implementa Render
            // o un error si debería hacerlo.
            //OutputDebugStringA(("WARNING: Manager '" + managerName + "' not found, instance null, or not renderable.\n").c_str());
        }
    }
    return S_OK;
}

HRESULT ManagerLocator::UpdateManagers(const std::vector<std::string>& orderList, float delta) {
    auto& entries = ManagerLocator::GetManagerEntries();
    for (const std::string& managerName : orderList) {
        auto it = entries.find(managerName);
        if (it != entries.end() && it->second.instance && it->second.updater) {
            it->second.updater(it->second.instance, delta);
        }
        else {
            // Un manager puede no ser renderizable, así que esto podría ser un warning si no implementa Render
            // o un error si debería hacerlo.
            //OutputDebugStringA(("WARNING: Manager '" + managerName + "' not found, instance null, or not renderable.\n").c_str());
        }
    }
    return S_OK;
}

void ManagerLocator::Shutdown() {
    auto& entries = ManagerLocator::GetManagerEntries();
	entries.clear(); // Limpia las entradas de servicios
    
    OutputDebugStringA("ManagerLocator: All managers and creators shut down.\n");
}