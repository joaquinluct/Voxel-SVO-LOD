#pragma once

#include "ConfigBase.h"
#include <GUID.h>
#include <IAsset.h> // Esto incluye ILifeCycle y todas sus interfaces base.
#include <string>
#include <Windows.h>

class AssetBase : public IAsset
{
public:
    AssetBase();
    ~AssetBase() override;

    // Métodos de IAsset (que ahora deben implementarse para que AssetBase sea concreta)
    void Load() override;
    void Unload() override;
    const std::string& GetAssetName() override;

    // Métodos de ILifeCycle (IInitializable, IUpdatable, IRenderable, IShutdownable)
    // Todos estos DEBEN implementarse para que AssetBase sea CONCRETA.
    HRESULT Init() override;
    HRESULT PostInit() { return S_OK; }
    void Update(float deltaTime) override;
    void Render() override;
    void Shutdown() override;

    virtual void SetConfig(std::shared_ptr<ConfigBase> config) {};

    void SetAssetName(const std::string& name) {
        m_assetName = name;
    }
    const std::string& GetAssetName() const {
        return m_assetName;
    }
    const std::string& GetID() const { return id; }
protected:
    // Puedes agregar un miembro para el nombre si GetAssetName lo va a devolver.
    std::string m_assetName;
    std::string id = generateUID();
};
