#pragma once

#include <Assets/Base/MeshAssetBase.h>
#include <Assets/Base/TextureAsset.h>
#include <Config/TerrainAssetConfigBase.h>
#include <cstdint>
#include <d3d11.h>
#include <Defines/Mesh.h>
#include <DirectXMath.h>
#include <Game/Systems/Terrain/Chunk/ChunkBufferAllocator.h>
#include <Managers/DeviceManager.h>
#include <memory>
#include <Services/Material.h>
#include <string>
#include <unordered_map>
#include <vector>

class TerrainAsset : public MeshAssetBase {
private:
    std::shared_ptr<TerrainAssetConfigBase> m_terrainConfig;
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<TextureAsset> m_textureAsset;

    /*Material* m_material;
    Material* m_shadowMaterial;*/

    std::unordered_map<size_t, Chunk*> m_regionToChunkMap;

    // Control de los buffers dinámicos del terreno
    // para índices y vértices con control de fragmentación
    // y reutilización de memoria
    // Se basa en los flag dirty y deleted de los chunks
    ChunkBufferAllocator m_allocator;

    UINT m_vertexTypeSize = 0;
    std::vector<std::vector<uint8_t>> m_tempVertexData;
    std::vector<std::vector<uint16_t>> m_tempIndexData;

    size_t m_currentVertexCapacityBytes[2] = { 0, 0 };
    size_t m_currentIndexCapacityBytes[2] = { 0, 0 };

    std::vector<float> m_textureTransforms;

    bool m_isGeneratingMesh = false;

public:
    TerrainAsset();
    TerrainAsset(const TerrainAsset* other);
    ~TerrainAsset() override;

    // ----------------------------------------------------------------
    // Métodos heredados de AssetBase
    // ----------------------------------------------------------------    
    HRESULT Init() override;
    void Shutdown() override;
    const std::string& GetAssetName() override { static const std::string name = "TerrainAsset"; return name; }
    static const std::string& GetStaticAssetName() { static const std::string name = "TerrainAsset"; return name; }
    void SetConfig(std::shared_ptr<ConfigBase> config) override;
    Mesh::DrawType GetDrawType() const override;
    int GetRenderPassesValue() const override { return m_terrainConfig ? m_terrainConfig->render_passes : 0; }

    // ----------------------------------------------------------------
    // Control de la malla
    // ----------------------------------------------------------------
    void GenerateMesh(std::vector<Chunk*> chunks, int indexBuffer) override;
    bool IsGenerating() const override { return m_isGeneratingMesh; }

    std::vector<uint8_t> GetVertexData(int index) const override;
    std::vector<uint16_t> GetIndexData(int index) const override;

    // ----------------------------------------------------------------
    // Métodos para acceder a los recursos compartidos
    // ----------------------------------------------------------------
    Material* GetMaterial() const { return m_material; }
    Material* GetShadowMaterial() const { return m_shadowMaterial; }
    const std::wstring& GetShaderName() const { return m_shaderName; }

    // ----------------------------------------------------------------
    // Métodos para crear los buffers dinámicos del terreno
    // ----------------------------------------------------------------
    void CreateDynamicBuffers(size_t vertexCount, size_t indexCount, int index, Microsoft::WRL::ComPtr<ID3D11Buffer>& vertexBuffer, Microsoft::WRL::ComPtr<ID3D11Buffer>& indexBuffer);
    UINT GetVertexTypeSize() const override { return sizeof(VertexDefinition::TextureMapVertex); }
    bool ShouldCompact() const;
    Chunk* FindChunkByRegion(const ChunkBufferRegion& region);
    void UnregisterChunk(Chunk* chunk);

    // ----------------------------------------------------------------
    // Métodos de inicialización
    // ----------------------------------------------------------------
    void InitMaterial();
    void InitShadowMaterial();

    // ----------------------------------------------------------------
    // Otros métodos específicos de la textura
    // ----------------------------------------------------------------
    XMFLOAT4 GetTextureTransforms();

    std::shared_ptr<TerrainAsset> GetMesh() const { return std::make_shared<TerrainAsset>(this); }
};
