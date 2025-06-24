// IAsset.h
#pragma once
#include <string>
#include "IAsset.h"

// Interfaz base para todos los assets que serán gestionados por el AssetManager
class IAssetSkybox: public IAsset {
public:
    virtual ~IAssetSkybox() = default;
    virtual void Shutdown() = 0;
	virtual void GetMesh() = 0;
};
