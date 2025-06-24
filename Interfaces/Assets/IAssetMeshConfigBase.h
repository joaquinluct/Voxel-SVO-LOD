#pragma once

#include <string>

struct IAssetMeshConfigBase {
    virtual ~IAssetMeshConfigBase() = default;
    virtual std::string GetType() const = 0;
    // Otros métodos virtuales comunes si quieres
};