#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <string>
#include <memory>
#include <Assets/Base/AssetBase.h>
#include <Config/TerrainAssetConfigBase.h>
#include <Defines/Pass.h>
#include <Defines/Mesh.h>
#include <Services/Material.h>
#include <Assets/Base/TextureAsset.h>

class MeshAsset; // Forward declaration para evitar dependencias circulares

class TerrainAsset : public AssetBase {
private:
    std::shared_ptr<TerrainAssetConfigBase> m_terrainConfig;

    std::shared_ptr<MeshAsset> m_meshAsset;
    std::shared_ptr<TextureAsset> m_textureAsset;

    Material* m_material;
    Material* m_shadowMaterial;

    std::wstring m_shaderName;

    // Se mantiene la estrategia de los únicos buffers dinámicos
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

    UINT m_vertexTypeSize = 0;

    std::vector<float> m_textureTransforms;

public:
    TerrainAsset();
    ~TerrainAsset() override;

    // IAsset overrides (todos los métodos requeridos por la interfaz)
    std::shared_ptr<AssetBase> Clone() const override { return nullptr; }
    std::shared_ptr <MeshAsset> CloneAsMesh() const override { return nullptr; }
    std::unique_ptr<AssetBase> CloneUnique() const override { return nullptr; }
    void Load() override {}
    void Unload() override {}
    HRESULT Init() override;
    void Render() override {}
    void Update(float deltaTime) override {}
    void Shutdown() override;
    const std::string& GetAssetName() override {
        static const std::string name = "TerrainAsset";
        return name;
    }
    static const std::string& GetStaticAssetName()
    {
        static const std::string name = "TerrainAsset";
        return name;
    }
    void SetConfig(std::shared_ptr<ConfigBase> config) override;

    // Métodos para acceder a los recursos compartidos
    Material* GetMaterial() const { return m_material; }
    Material* GetShadowMaterial() const { return m_shadowMaterial; }
    const std::wstring& GetShaderName() const { return m_shaderName; }

    // Métodos para crear los buffers dinámicos del terreno
    HRESULT CreateDynamicBuffers(UINT vertexTypeSize, UINT maxVertices, UINT maxIndexes);
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetVertexBuffer() { return m_vertexBuffer; }
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetIndexBuffer() { return m_indexBuffer; }
    UINT GetVertexTypeSize() const { return m_vertexTypeSize; }

    // Métodos de inicialización
    void InitMaterial();
    void InitShadowMaterial();



	// Otros métodos específicos de la textura
    XMFLOAT4 GetTextureTransforms();

	std::shared_ptr<MeshAsset> GetMesh() const { return m_meshAsset; }
};