// AssetManager.h
#pragma once
#include <ManagerBase.h>
#include "AssetType.h"
#include <Assets/IAsset.h>
#include "AssetLoadContext.h"
#include <Assets/Base/TextureAsset.h>
#include <yaml-cpp/yaml.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <future>
#include <mutex>
#include <functional> // Para std::function
#include <queue>      // Para la cola de comandos GPU
#include "IGPUCreateCommand.h" // Interfaz para comandos de creación de recursos GPU
#include "Assets/Base/MeshAsset.h" // Incluir el nuevo Mesh
#include <Config/Assets/Base/BaseIndexConfig.h>
#include <Config/Assets/Base/MainIndexConfig.h>
#include <AssetLocator/AssetLocator.h>
#include <ManagerLocator/ManagerLocator.h>

// ====================================================================
// AssetManager
// ====================================================================

class AssetManager : public ManagerBase {
public:
    AssetManager();
    ~AssetManager() override;

    bool IsWindowDependent() const override { return true; }

    HRESULT Init(HWND* hwnd, int width, int height) override;
    HRESULT InitShaders(HWND* hwnd, int width, int height);

    void Render() override;
    void Update(float deltaTime) override; // CRÍTICO: procesa cargas asíncronas y comandos GPU
    void Shutdown() override;

    //ID3D11ShaderResourceView* LoadTexture(std::shared_ptr<ID3D11Device> device, const std::string& filePath);
    ID3D11ShaderResourceView* LoadTexture(std::string assetName);

    const std::string& GetManagerName() const override {
        static const std::string name = "AssetManager";
        return name;
    }
    static const std::string& GetStaticManagerName()
    {
        static const std::string name = "AssetManager";
        return name;
    }
    const std::vector<std::string>& GetAssetList() const {
        if (m_config) {
            return m_config->mainIndex;
		}
	}

    const std::vector<std::string>& GetBaseAssetList() const {
        if (m_configBase) {
            return m_configBase->index;
        }
    }

    // ====================================================================
    // API para comandos GPU (para que los hilos de carga los pongan en cola)
    // ====================================================================
    // void AddGPUCreateCommand(std::unique_ptr<IGPUCreateCommand> command);

private:
    MainIndexConfig* m_config{};
    BaseIndexConfig* m_configBase{};
};

// ====================================================================
// Implementación de funciones template y helpers (en el .h o incluidos)
// ====================================================================
