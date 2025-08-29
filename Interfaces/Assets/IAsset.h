// IAsset.h
#pragma once
#include <d3d11.h>
#include <string>
#include <ILifeCycle.h>
#include <IClonableAsset.h>

// Interfaz base para todos los assets que serán gestionados por el AssetManager
class IAsset : public ILifeCycle, public IClonableAsset {
public:
    virtual ~IAsset() = default;
	virtual void Load() = 0; // Método para cargar el asset
	virtual void Unload() = 0; // Método para descargar el asset
	virtual const std::string& GetAssetName() = 0; // Método para obtener el nombre del asset	
};
