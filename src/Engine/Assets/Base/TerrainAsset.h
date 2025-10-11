#pragma once

#include <Assets/Base/AssetBase.h>
#include <Config/TerrainAssetConfigBase.h>
#include <memory>
#include <Services/Material.h>
#include <string>
#include <Windows.h>

class TerrainAsset : public AssetBase {
private:
    std::shared_ptr<TerrainAssetConfigBase> m_terrainConfig;
    Material* m_material = nullptr;
    Material* m_shadowMaterial = nullptr;
    std::wstring m_shaderName;
    std::string m_shaderAssetName;

public:
    TerrainAsset();
    ~TerrainAsset() override;

    // ----------------------------------------------------------------
    // Métodos heredados de AssetBase
    // ----------------------------------------------------------------    
    HRESULT Init() override;
    void Shutdown() override;
    const std::string& GetAssetName() override { static const std::string name = "TerrainAsset"; return name; }
    static const std::string& GetStaticAssetName() { static const std::string name = "TerrainAsset"; return name; }
    void SetConfig(std::shared_ptr<ConfigBase> config) override;

    // ----------------------------------------------------------------
    // Funcionalidad esencial: Shader y pases de renderizado
    // ----------------------------------------------------------------
    int GetRenderPassesValue() const { return m_terrainConfig ? m_terrainConfig->render_passes : 0; }
    Material* GetMaterial() const { return m_material; }
    Material* GetShadowMaterial() const { return m_shadowMaterial; }
    const std::wstring& GetShaderName() const { return m_shaderName; }
    const std::string& GetShaderAssetName() const { return m_shaderAssetName; }

private:
    // ----------------------------------------------------------------
    // Métodos de inicialización de materiales/shaders
    // ----------------------------------------------------------------
    void InitMaterial();
    void InitShadowMaterial();
};
