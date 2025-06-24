#pragma once

#include <string>

struct IAssetSkyboxConfigBase {
    virtual ~IAssetSkyboxConfigBase() = default;
    virtual std::string GetType() const = 0;
    // Otros métodos virtuales comunes si quieres
};