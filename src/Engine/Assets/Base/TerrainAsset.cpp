// TerrainAsset.cpp
#include "TerrainAsset.h"
#include <AssetLocator/AssetLocator.h>
#include <Assets/Base/TextureAsset.h>
#include <REGISTER_ASSET_MACRO.h>
#include <TerrainAssetConfigBase.h>
#include <Util/Text/Text.h>
#include <windows.h>

REGISTER_ASSET_TYPE(TerrainAsset, "TerrainAsset")

TerrainAsset::TerrainAsset()
    : m_terrainConfig(nullptr)
{
}

TerrainAsset::~TerrainAsset() {
    Shutdown();
}

void TerrainAsset::SetConfig(std::shared_ptr<ConfigBase> config) {
    m_terrainConfig = std::dynamic_pointer_cast<TerrainAssetConfigBase>(config);
}

HRESULT TerrainAsset::Init() {
    if (!m_terrainConfig) {
        return S_OK;
    }

    // Inicializar información básica del shader
    m_shaderName = StringToWstring(m_terrainConfig->shader);
    m_shaderAssetName = m_terrainConfig->shader;

    // Inicializar materiales que contienen la información del shader
    InitMaterial();
    InitShadowMaterial();

    return S_OK;
}

void TerrainAsset::InitMaterial() {
    m_material = new Material();
    if (!m_material) {
        OutputDebugStringA("TerrainAsset::InitMaterial - ERROR: Failed to create Material.\n");
        return;
    }
    m_material->SetShaderName(m_shaderName);
    m_material->Init();

    // Configurar textura básica si está especificada
    std::string textureAssetName = m_terrainConfig->texture;
    if (!textureAssetName.empty() && textureAssetName != "none") {
        auto textureAsset = AssetLocator::GetTextureAsset(textureAssetName);
        if (textureAsset) {
            textureAsset->SetTextureView(m_material);
            m_material->SetTextureType(textureAsset->GetTextureType());
        }
    }
}

void TerrainAsset::InitShadowMaterial() {
    if (!m_terrainConfig->cast_shadows) {
        return;
    }
    m_shadowMaterial = new Material();
    m_shadowMaterial->SetShaderName(StringToWstring(m_terrainConfig->shader_shadows));
    m_shadowMaterial->Init();
}



void TerrainAsset::Shutdown() {
    if (m_material) { 
        delete m_material; 
        m_material = nullptr; 
    }
    if (m_shadowMaterial) { 
        delete m_shadowMaterial; 
        m_shadowMaterial = nullptr; 
    }
}
