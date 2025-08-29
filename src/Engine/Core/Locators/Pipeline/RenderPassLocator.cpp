// RenderPassLocator.cpp
#include "RenderPassLocator.h"
#include <iostream> // Para debugging si quieres usar std::cout en lugar de OutputDebugStringA
#include <windows.h> // Para OutputDebugStringA
#include <string> // Para std::to_string

std::map<std::string, RenderPassLocator::RenderPassEntry>& RenderPassLocator::GetRenderPassEntries() {
    static std::map<std::string, RenderPassEntry> s_serviceEntries; 
    return s_serviceEntries;
}

RenderPassLocator& RenderPassLocator::GetInstance() {
    static RenderPassLocator instance; // Se crea una vez, la primera vez que se llama
    return instance;
}

void RenderPassLocator::RegisterRenderPassCreator(
    const std::string& name,
    CreateRenderPassLambda createFn) {    
    auto& entries = RenderPassLocator::GetRenderPassEntries();
    if (entries.count(name)) {
        OutputDebugStringA(("[RenderPassLocator] WARNING: '" + name + "' already registered. Overwriting.\n").c_str());
    }
    //s_serviceCreators[name] = { createFn, initFn };
    entries[name] = {nullptr, createFn};
    OutputDebugStringA(("[RenderPassLocator] " + name + " registrado - OK\n").c_str());
}

HRESULT RenderPassLocator::CreateRenderPasss() {
    auto& entries = RenderPassLocator::GetRenderPassEntries();
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
    //        OutputDebugStringA(("ERROR: RenderPass '" + managerName + "' in init order list but not registered.\n").c_str());
    //        return E_FAIL;
    //    }
    //}
    return S_OK;
}

void RenderPassLocator::Shutdown() {
    auto& entries = RenderPassLocator::GetRenderPassEntries();
	entries.clear(); // Limpia las entradas de servicios
    
    OutputDebugStringA("RenderPassLocator: All services and creators shut down.\n");
}