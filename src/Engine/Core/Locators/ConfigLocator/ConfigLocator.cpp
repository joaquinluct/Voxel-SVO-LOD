// ConfigLocator.cpp
#include "ConfigLocator.h"
#include <iostream> // Para debugging si quieres usar std::cout en lugar de OutputDebugStringA
#include <windows.h> // Para OutputDebugStringA
#include <string> // Para std::to_string

std::map<std::string, ConfigLocator::ConfigEntry>& ConfigLocator::GetConfigEntries() {
    static std::map<std::string, ConfigEntry> s_serviceEntries; 
    return s_serviceEntries;
}

ConfigLocator& ConfigLocator::GetInstance() {
    static ConfigLocator instance; // Se crea una vez, la primera vez que se llama
    return instance;
}

void ConfigLocator::RegisterConfigCreator(
    const std::string& name,
    CreateConfigLambda createFn) {
    auto& entries = ConfigLocator::GetConfigEntries();
    if (entries.count(name)) {
        OutputDebugStringA(("WARNING: Config creator for '" + name + "' already registered. Overwriting.\n").c_str());
    }
    //s_serviceCreators[name] = { createFn, initFn };
    entries[name] = {nullptr, createFn};
}

HRESULT ConfigLocator::CreateConfigs() {
    auto& entries = ConfigLocator::GetConfigEntries();
    for (auto& entry : entries) {
        entry.second.instance = entry.second.creator();
        if (!entry.second.instance) {
            OutputDebugStringA(("ERROR: Failed to create instance for service '" + entry.first + "'\n").c_str());
            return E_FAIL;
        }
    }
    //for (const std::string& managerName : orderList) {
    //    auto it = entries.find(managerName);
    //    if (it != entries.end()) {
    //        // 1. Crear la instancia del config
    //        it->second.instance = it->second.creator();
    //        if (!it->second.instance) {
    //            OutputDebugStringA(("ERROR: Failed to create instance for service '" + managerName + "'\n").c_str());
    //            return E_FAIL;
    //        }
    //    }
    //    else {
    //        OutputDebugStringA(("ERROR: Config '" + managerName + "' in init order list but not registered.\n").c_str());
    //        return E_FAIL;
    //    }
    //}
    return S_OK;
}

void ConfigLocator::Shutdown() {
    auto& entries = ConfigLocator::GetConfigEntries();
	entries.clear(); // Limpia las entradas de servicios
    
    OutputDebugStringA("ConfigLocator: All services and creators shut down.\n");
}