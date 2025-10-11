#pragma once

#include <Assets/Base/MeshAssetBase.h>
#include <Assets/Base/TerrainAsset.h>
#include <Config/Game/System/TerrainConfig.h>
#include <memory>
#include <string>

class TerrainAssetManager {
public:
    TerrainAssetManager();

    void Init(const TerrainConfig& config);

    TerrainAsset* GetTerrainAsset() const;
    // TODO: Remove - TerrainAsset no longer inherits from MeshAssetBase
    // MeshAssetBase* GetMeshAsset() const;

private:
    std::unique_ptr<TerrainAsset> m_asset;
    std::string m_assetName;
};
