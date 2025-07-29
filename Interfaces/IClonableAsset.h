#pragma once

#include <string>
#include <memory>
//#include <Assets/Base/AssetBase.h>

class AssetBase; // Forward declaration

class IClonableAsset {
public:
    virtual ~IClonableAsset() = default;
    virtual std::shared_ptr<AssetBase> Clone() const = 0;
};