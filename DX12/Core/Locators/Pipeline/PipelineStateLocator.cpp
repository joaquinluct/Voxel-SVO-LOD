// PipelineStateLocator.cpp
#include "PipelineStateLocator.h"
#include <iostream> // Para debugging si quieres usar std::cout en lugar de OutputDebugStringA
#include <windows.h> // Para OutputDebugStringA
#include <string> // Para std::to_string

std::map<std::string, PipelineStateLocator::PipelineStateEntry>& PipelineStateLocator::GetPipelineStateEntries() {
    static std::map<std::string, PipelineStateEntry> s_serviceEntries;
    return s_serviceEntries;
}

PipelineStateLocator& PipelineStateLocator::GetInstance() {
    static PipelineStateLocator instance; // Se crea una vez, la primera vez que se llama
    return instance;
}

void PipelineStateLocator::RegisterPipelineStateCreator(
    const std::string& name,
    CreatePipelineStateLambda createFn) {
    auto& entries = PipelineStateLocator::GetPipelineStateEntries();
    if (entries.count(name)) {
        OutputDebugStringA(("WARNING: PipelineState creator for '" + name + "' already registered. Overwriting.\n").c_str());
    }
    //s_serviceCreators[name] = { createFn, initFn };
    entries[name] = { nullptr, createFn };
}

HRESULT PipelineStateLocator::CreatePipelineStates() {
    auto& entries = PipelineStateLocator::GetPipelineStateEntries();
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
    //        OutputDebugStringA(("ERROR: PipelineState '" + managerName + "' in init order list but not registered.\n").c_str());
    //        return E_FAIL;
    //    }
    //}
    return S_OK;
}

void PipelineStateLocator::Shutdown() {
    auto& entries = PipelineStateLocator::GetPipelineStateEntries();
    entries.clear(); // Limpia las entradas de servicios

    OutputDebugStringA("PipelineStateLocator: All services and creators shut down.\n");
}