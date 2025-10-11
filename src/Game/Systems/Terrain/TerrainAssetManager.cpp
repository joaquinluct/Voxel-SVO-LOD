#include "TerrainAssetManager.h"
#include <Assets/Base/MeshAssetBase.h>
#include <vector>
//#include <Locators/AssetConfigLocator.h>

TerrainAssetManager::TerrainAssetManager()
    : m_asset(nullptr)
{
}

void TerrainAssetManager::Init(const TerrainConfig& config)
{
    /*m_assetName = config.tr .terrain_asset;
    const auto* assetConfig = AssetConfigLocator::GetTerrainAssetConfig(m_assetName);
    if (!assetConfig || !assetConfig->enabled) return;

    m_asset = std::make_unique<TerrainAsset>();
    m_asset->LoadFromConfig(assetConfig);*/
}

TerrainAsset* TerrainAssetManager::GetTerrainAsset() const
{
    return m_asset.get();
}

// TODO: Remove this method - TerrainAsset no longer inherits from MeshAssetBase
// Mesh functionality is now handled by specific chunk implementations
// MeshAssetBase* TerrainAssetManager::GetMeshAsset() const
// {
//     return static_cast<MeshAssetBase*>(m_asset.get());
// }
