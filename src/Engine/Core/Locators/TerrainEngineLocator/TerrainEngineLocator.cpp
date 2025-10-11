// TerrainEngineLocator.cpp
#include "TerrainEngineLocator.h"
#include <string> // Para std::to_string
#include <windows.h> // Para OutputDebugStringA

std::map<std::string, TerrainEngineLocator::TerrainEngineEntry>& TerrainEngineLocator::GetTerrainEngineEntries() {
    static std::map<std::string, TerrainEngineEntry> s_serviceEntries;
    return s_serviceEntries;
}

TerrainEngineLocator& TerrainEngineLocator::GetInstance() {
    static TerrainEngineLocator instance; // Se crea una vez, la primera vez que se llama
    return instance;
}

void TerrainEngineLocator::RegisterTerrainEngineCreator(
    const std::string& name,
    CreateTerrainEngineLambda createFn) {
    auto& entries = TerrainEngineLocator::GetTerrainEngineEntries();
    if (entries.count(name)) {
        OutputDebugStringA(("WARNING: TerrainEngine creator for '" + name + "' already registered. Overwriting.\n").c_str());
    }
    //s_serviceCreators[name] = { createFn, initFn };
    entries[name] = { nullptr, createFn };
}

HRESULT TerrainEngineLocator::CreateTerrainEngine(const std::string& engineName) {
    auto& entries = TerrainEngineLocator::GetTerrainEngineEntries();
    auto it = entries.find(engineName);
    if (it != entries.end()) {
        // 1. Crear la instancia del terrainEngine
        it->second.instance = it->second.creator();
        if (!it->second.instance) {
            OutputDebugStringA(("ERROR: Failed to create instance for service '" + engineName + "'\n").c_str());
            return E_FAIL;
        }
    }
    else {
        OutputDebugStringA(("ERROR: TerrainEngine '" + engineName + "' not registered.\n").c_str());
        return E_FAIL;
    }
    return S_OK;
}

HRESULT TerrainEngineLocator::CreateTerrainEngines() {
    auto& entries = TerrainEngineLocator::GetTerrainEngineEntries();
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
    //        // 1. Crear la instancia del terrainEngine
    //        it->second.instance = it->second.creator();
    //        if (!it->second.instance) {
    //            OutputDebugStringA(("ERROR: Failed to create instance for service '" + managerName + "'\n").c_str());
    //            return E_FAIL;
    //        }
    //    }
    //    else {
    //        OutputDebugStringA(("ERROR: TerrainEngine '" + managerName + "' in init order list but not registered.\n").c_str());
    //        return E_FAIL;
    //    }
    //}
    return S_OK;
}

void TerrainEngineLocator::Shutdown() {
    auto& entries = TerrainEngineLocator::GetTerrainEngineEntries();
    entries.clear(); // Limpia las entradas de servicios

    OutputDebugStringA("TerrainEngineLocator: All services and creators shut down.\n");
}
