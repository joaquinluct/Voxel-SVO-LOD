#pragma once
#define NOMINMAX

#include "Terrain/ChunkService.h"
#include <Assets/Base/MeshAssetBase.h>
#include <chrono>
#include <Config/Game/System/TerrainConfig.h>
#include <Defines/Pass.h>
#include <Defines/Texture.h>
#include <Defines/WorldTerrain.h>
#include <Game/Systems/Terrain/Chunk/Chunk.h>
#include <ICamera.h>
#include <IService.h>
#include <Managers/CameraManager.h>
#include <Managers/KeyboardManager.h>
#include <memory>
#include <string>
#include <windows.h>

class TerrainAsset;
class ProceduralService;

class Terrain : public IService
{
public:
    Terrain();
    ~Terrain() override;

    // IService implementation
    HRESULT Init() override;
    HRESULT InitServices();
    HRESULT InitConfig();
    void Update(float deltaTime) override;
    void Shutdown() override {};
    const std::string& GetServiceName() const override { static const std::string name = "Terrain"; return name; }
    static const std::string& GetStaticServiceName() { static const std::string name = "Terrain"; return name; }

    // Generación de los Mesh del terreno
    void GenerateMesh();

    // Getters
    MeshAssetBase* GetTerrainMesh() const;
    std::vector<Chunk*> GetChunks(std::shared_ptr<ICamera> camera);
    std::vector<Chunk*> GetVisibleChunks();
    std::shared_ptr<ChunkService> GetChunkService() const { return m_chunkService; }

    // Métods de gestión del terreno
    float GetTerrainHeight(float x, float z) const;
    DirectX::XMFLOAT3 GetTerrainNormal(float posX, float posZ);
    TextureDefines::CBTerrainBlendBuffer GetTerrainBlenderData();
    TextureDefines::CBTerrain2BlendBuffer GetTerrain2BlenderData();
    TextureDefines::TerrainBiomeBufferData GetTerrainBiomeBufferData();
    void EmptyRecycleBin();

    // Control de estado
    bool IsDirty() const { return m_dirty; }
    void SetDirty(const bool dirty) { m_dirty = dirty; }

    bool IsGenerating() const { return m_isGenerating; }

    // Methods para depuración
    int GetNumVisibleChunks() const { return m_numVisibleChunks; }
    int GetNumChunks() const { return m_numChunks; }

private:
    // Servicios y managers
    std::shared_ptr<CameraManager> m_cameraManager = nullptr;
    std::shared_ptr<ChunkService> m_chunkService = nullptr;
    std::shared_ptr<ProceduralService> m_proceduralService = nullptr;
    TerrainAsset* m_terrainAsset = nullptr;

    // Configuración del terreno
    std::shared_ptr<TerrainConfig> m_config;
    WorldTerrain::TerrainDefinition m_def;

    // Datos físicos del terreno (que vienen de la configuración)
    float m_chunkSize;
    const float m_materTerrainHeight = 50.0f; // Multipicador para a altura do terreno

    RenderPasses::ExecPassTerrainParams m_terrainParams = {};

    // Control de estado
    bool m_dirty = false;
    bool m_isGenerating = false;
    XMFLOAT3 m_lastCameraPosition;

    // Propiedades para depuración
    int m_numVisibleChunks = 0;
    int m_numChunks = 0;
};
