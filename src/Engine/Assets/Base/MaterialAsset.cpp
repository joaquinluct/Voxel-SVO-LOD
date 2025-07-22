#include "MaterialAsset.h"

#include <REGISTER_ASSET_MACRO.h>
#include <Util/Utils.h> // Para SafeRelease
#include <AssetManager.h>
#include <TextureLoader/TextureLoader.h>

REGISTER_ASSET_TYPE(MaterialAsset, "MaterialAsset")

MaterialAsset::MaterialAsset() {
}
MaterialAsset::~MaterialAsset() {
}
void MaterialAsset::Shutdown() {}
