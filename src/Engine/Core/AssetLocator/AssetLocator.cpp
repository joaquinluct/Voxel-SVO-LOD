// AssetLocator.cpp
#include "AssetLocator.h"
#include <iostream> // Para debugging si quieres usar std::cout en lugar de OutputDebugStringA
#include <windows.h> // Para OutputDebugStringA
#include <string> // Para std::to_string

std::map<std::string, AssetLocator::AssetEntry>& AssetLocator::GetAssetEntries() {
    static std::map<std::string, AssetEntry> s_serviceEntries;
    return s_serviceEntries;
}

AssetLocator& AssetLocator::GetInstance() {
    static AssetLocator instance; // Se crea una vez, la primera vez que se llama
    return instance;
}

void AssetLocator::RegisterAssetCreator(
    const std::string& name,
    CreateAssetLambda createFn,
    InitializeAssetLambda initFn,
    RenderAssetLambda renderFn) {
    auto& entries = AssetLocator::GetAssetEntries();
    if (entries.count(name)) {
        OutputDebugStringA(("WARNING: Asset creator for '" + name + "' already registered. Overwriting.\n").c_str());
    }
    //s_serviceCreators[name] = { createFn, initFn };
    entries[name] = { nullptr, createFn, initFn, renderFn };
}

HRESULT AssetLocator::InitializeAssets(const std::vector<std::string>& orderList) {
    auto& entries = AssetLocator::GetAssetEntries();
    for (const std::string& managerName : orderList) {
        auto it = entries.find(managerName);
        if (it != entries.end()) {
            // 1. Crear la instancia del manager
            it->second.instance = it->second.creator();
            if (!it->second.instance) {
                OutputDebugStringA(("ERROR: Failed to create instance for service '" + managerName + "'\n").c_str());
                return E_FAIL;
            }

            // 2. Inicializar la instancia (pasando la sub-nodo de configuración específica si existe)
            // configRoot[managerName] asegura que se le pasa solo la configuración relevante a ese manager.
            HRESULT hr = it->second.initializer(it->second.instance);
            if (FAILED(hr)) {
                OutputDebugStringA(("ERROR: Failed to initialize service '" + managerName + "'\n").c_str());
                return hr;
            }
        }
        else {
            OutputDebugStringA(("ERROR: Asset '" + managerName + "' in init order list but not registered.\n").c_str());
            // return E_FAIL;
        }
    }
    return S_OK;
}

std::shared_ptr<IAsset> AssetLocator::GetAsset(const std::string& name) {
    auto& entries = AssetLocator::GetAssetEntries();
    auto it = entries.find(name);
    if (it != entries.end()) {
        return entries[name].instance;
    }
    return nullptr;
}

HRESULT AssetLocator::RenderAssets(const std::vector<std::string>& orderList) {
    auto& entries = AssetLocator::GetAssetEntries();
    for (const std::string& managerName : orderList) {
        auto it = entries.find(managerName);
        if (it != entries.end() && it->second.instance && it->second.renderer) {
            it->second.renderer(it->second.instance);
        }
        else {
            // Un manager puede no ser renderizable, así que esto podría ser un warning si no implementa Render
            // o un error si debería hacerlo.
            OutputDebugStringA(("WARNING: Asset '" + managerName + "' not found, instance null, or not renderable.\n").c_str());
        }
    }
    return S_OK;
}

void AssetLocator::Shutdown() {
    auto& entries = AssetLocator::GetAssetEntries();
    entries.clear(); // Limpia las entradas de servicios

    OutputDebugStringA("AssetLocator: All services and creators shut down.\n");
}