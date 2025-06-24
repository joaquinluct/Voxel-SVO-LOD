// IAsset.h
#pragma once
#include "IAsset.h"
#include <string>
#include <vector>
#include <Material/includes/VertexDefinition.h>

// Interfaz base para todos los assets que serán gestionados por el AssetManager
class IAssetMesh : public IAsset {
public:
    virtual ~IAssetMesh() = default;
    virtual void Shutdown() = 0;
};