// AssetLocator.cpp
#include "AssetLocator.h"
#include <iostream> // Para debugging si quieres usar std::cout en lugar de OutputDebugStringA
#include <windows.h> // Para OutputDebugStringA
#include <string> // Para std::to_string
#include <VertexAssetConfigBase.h>
#include <Assets/Base/TextureAsset.h>

std::map<std::string, AssetLocator::AssetEntry>& AssetLocator::GetAssetBaseEntries() {
    static std::map<std::string, AssetEntry> s_serviceEntries;
    return s_serviceEntries;
}

std::map<std::string, std::shared_ptr<AssetBase>>& AssetLocator::GetAssetEntries() {
    static std::map<std::string, std::shared_ptr<AssetBase>> s_assetEntries;
    return s_assetEntries;
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
    auto& entries = AssetLocator::GetAssetBaseEntries();
    if (entries.count(name)) {
        OutputDebugStringA(("WARNING: Asset creator for '" + name + "' already registered. Overwriting.\n").c_str());
    }
    //s_serviceCreators[name] = { createFn, initFn };
    entries[name] = { nullptr, createFn, initFn, renderFn };
}


HRESULT AssetLocator::InitializeAssets(const std::vector<std::string>& orderList, const std::string& filterType) {
    //auto& entries = AssetLocator::GetAssetEntries();
    for (const std::string& assetName: orderList) {
        // 1. Obtener la configuración del asset, si existe
        const std::string configName = assetName + "Config";
        std::shared_ptr<ConfigBase> config = ConfigLocator::GetConfig<ConfigBase>(configName);
        if (!config) {
            continue;
        }
        
        auto& entries = AssetLocator::GetAssetEntries();

        if (config->type == TEXTURE_ASSET) {
            entries[assetName] = std::dynamic_pointer_cast<AssetBase>(GetAssetBase<TextureAsset>());
        }
        if (config->type == MESH_ASSET) {
            entries[assetName] = std::dynamic_pointer_cast<AssetBase>(GetAssetBase<MeshAsset>());
        }
        if (config->type == SHADER_ASSET) {
            entries[assetName] = std::dynamic_pointer_cast<AssetBase>(GetAssetBase<ShaderAsset>());
		}
        if (config->type == MATERIAL_ASSET) {
            entries[assetName] = std::dynamic_pointer_cast<AssetBase>(GetAssetBase<MaterialAsset>());
        }
        if (config->type == VERTEX_ASSET) {
            entries[assetName] = std::dynamic_pointer_cast<AssetBase>(GetAssetBase<VertexAsset>());
        }
        if (entries[assetName] != nullptr) {
            entries[assetName]->SetConfig(config);
        }
        else {
            OutputDebugStringA(("No se ha podido cargar " + assetName).c_str());
        }
    }
	return S_OK;
}

HRESULT AssetLocator::InitializeShaders(const std::vector<std::string>& orderList) {
	return InitializeAssets(orderList, SHADER_ASSET.data());
}

HRESULT AssetLocator::InitializeBaseAssets(const std::vector<std::string>& orderList) {
    auto& entries = AssetLocator::GetAssetBaseEntries();
    for (const std::string& assetName: orderList) {
        auto it = entries.find(assetName);
        if (it != entries.end()) {
            // 1. Crear la instancia del manager
            it->second.instance = it->second.creator();
            if (!it->second.instance) {
                OutputDebugStringA(("ERROR: Failed to create instance for service '" + assetName+ "'\n").c_str());
                return E_FAIL;
            }

            // 2. Inicializar la instancia (pasando la sub-nodo de configuración específica si existe)
            // configRoot[managerName] asegura que se le pasa solo la configuración relevante a ese manager.
            HRESULT hr = it->second.initializer(it->second.instance);
            if (FAILED(hr)) {
                OutputDebugStringA(("ERROR: Failed to initialize service '" + assetName+ "'\n").c_str());
                return hr;
            }
        }
        else {
            OutputDebugStringA(("ERROR: Asset '" + assetName+ "' in init order list but not registered.\n").c_str());
            // return E_FAIL;
        }
    }
    return S_OK;
}

std::shared_ptr<AssetBase> AssetLocator::GetAssetBase(const std::string& name) {
    auto& entries = AssetLocator::GetAssetBaseEntries();
    auto it = entries.find(name);
    if (it != entries.end()) {
        return entries[name].instance;
    }
    return nullptr;
}

std::shared_ptr<AssetBase> AssetLocator::GetAsset(const std::string& name) {
    auto& entries = AssetLocator::GetAssetEntries();
    auto it = entries.find(name);
    if (it != entries.end()) {
        return entries[name];
	}
    return nullptr;
}

std::shared_ptr<VertexAsset> AssetLocator::GetVertexAsset(const std::string& name) {
    auto& entries = AssetLocator::GetAssetEntries();
    auto it = entries.find(name);
    if (it != entries.end()) {
		std::shared_ptr<VertexAsset> vAsset = std::dynamic_pointer_cast<VertexAsset>(it->second);
        if (vAsset) {
			std::string configName = name + "Config";
			auto config = ConfigLocator::GetConfig<VertexAssetConfigBase>(configName);
			vAsset->SetConfig(config);
        } else {
            OutputDebugStringA(("ERROR: Asset '" + name + "' is not a VertexAsset.\n").c_str());
		}
        return vAsset;
	}
    return nullptr;
}

std::shared_ptr<TextureAsset> AssetLocator::GetTextureAsset(const std::string& name) {
    auto& entries = AssetLocator::GetAssetEntries();
    auto it = entries.find(name);
    if (it != entries.end()) {
        std::shared_ptr<TextureAsset> vAsset = std::dynamic_pointer_cast<TextureAsset>(it->second);
        if (vAsset) {
            std::string configName = name + "Config";
            auto config = ConfigLocator::GetConfig<ITextureConfig>(configName);
            if (config) {
                vAsset->SetConfig(config);
                if (config->map.size()) {
                    auto mapConfig = ConfigLocator::GetConfig<ITextureMapConfig>(config->map + "Config");
                    config->mapConfig = mapConfig;
                }
            }
        }
        else {
            OutputDebugStringA(("ERROR: Asset '" + name + "' is not a VertexAsset.\n").c_str());
        }
        return vAsset;
    }
    return nullptr;
}

std::shared_ptr<ShaderAsset> AssetLocator::GetShaderAsset(const std::string& name) {
    auto& entries = AssetLocator::GetAssetEntries();
    auto it = entries.find(name);
    if (it != entries.end()) {
        std::shared_ptr<ShaderAsset> vAsset = std::dynamic_pointer_cast<ShaderAsset>(it->second);
        if (vAsset) {
            std::string configName = name + "Config";
            auto config = ConfigLocator::GetConfig<IAssetShaderConfig>(configName);
            vAsset->SetConfig(config);
        }
        else {
            OutputDebugStringA(("ERROR: Asset '" + name + "' is not a VertexAsset.\n").c_str());
        }
        return vAsset;
    }
    return nullptr;
}

HRESULT AssetLocator::RenderAssets(const std::vector<std::string>& orderList) {
    //auto& entries = AssetLocator::GetAssetEntries();
    //for (const std::string& assetName: orderList) {
    //    auto it = entries.find(assetName);
    //    if (it != entries.end() && it->second.instance && it->second.renderer) {
    //        it->second.renderer(it->second.instance);
    //    }
    //    else {
    //        // Un manager puede no ser renderizable, así que esto podría ser un warning si no implementa Render
    //        // o un error si debería hacerlo.
    //        OutputDebugStringA(("WARNING: Asset '" + assetName+ "' not found, instance null, or not renderable.\n").c_str());
    //    }
    //}
    //return S_OK;
    return S_OK;
}

void AssetLocator::Shutdown() {
    auto& entries = AssetLocator::GetAssetBaseEntries();
    entries.clear(); // Limpia las entradas de servicios

    OutputDebugStringA("AssetLocator: All services and creators shut down.\n");
}