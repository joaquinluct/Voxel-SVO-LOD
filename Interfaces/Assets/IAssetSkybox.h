// IAsset.h
#pragma once
#include <Assets/Base/AssetBase.h>
#include <string>
#include "IAsset.h"

// Interfaz base para todos los assets que serán gestionados por el AssetManager
class IAssetSkybox: public AssetBase {
public:
    //virtual ~IAssetSkybox() = default;
    virtual void Shutdown() = 0;
	HRESULT Init() override = 0;
	void Load() override = 0;
	void Unload() override = 0;
};
