#pragma once

#include <string>
#include <memory>
//#include <Assets/Base/AssetBase.h>

class MeshAsset;
class AssetBase; // Forward declaration

class IClonableAsset {
public:
    virtual ~IClonableAsset() = default;
    virtual std::shared_ptr<AssetBase> Clone() const { return nullptr; }
};