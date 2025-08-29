#pragma once

#include <string>
#include "ConfigBase.h"
#include <IAsset.h> // Esto incluye ILifeCycle y todas sus interfaces base.
#include <Config/ITextureConfig.h>

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

    virtual void SetConfig(std::shared_ptr<ConfigBase> config) = 0;

    void SetAssetName(const std::string& name) {
        m_assetName = name;
	}
    const std::string& GetAssetName() const {
        return m_assetName;
    }    
protected:
    // Puedes agregar un miembro para el nombre si GetAssetName lo va a devolver.
    std::string m_assetName;
};