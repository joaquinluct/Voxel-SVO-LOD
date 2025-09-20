#pragma once

#include <d3d11.h>
#include <vector>
#include <memory>
#include <Defines/Pass.h>
#include <Managers/ShaderManager.h>
#include <Managers/CameraManager.h>
#include <Managers/DeviceManager.h>
#include <Managers/RenderManager/RenderPass.h>
#include <SceneManager.h>
#include <Config/Base/RenderPass/TerrainPassConfig.h>

class Shadows;
class Lighting;
class MeshAsset;
class TerrainAsset;
class Terrain;
class FrameStateService;
class World;

class TerrainPass : public RenderPass
{
private:
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<CameraManager> m_cameraManager;
    std::shared_ptr<ShaderManager> m_shaderManager;
	std::shared_ptr<TerrainAsset> m_terrainAsset;    
	std::shared_ptr<FrameStateService> m_FrameStateService;

    std::shared_ptr<Lighting> m_lighthing;
    std::shared_ptr<Shadows> m_shadows;
    std::shared_ptr<World> m_world;
    std::shared_ptr<Terrain> m_terrain;

    std::shared_ptr<TerrainPassConfig> config;
    std::wstring shaderName;

    // AÑADIDOS para la estrategia de búfer dinámico
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;

    std::vector<uint8_t> m_tempVertexData;
    std::vector<UINT> m_tempIndexData;

    // Almacena las mallas visibles y el tamaño de los datos de vértice para la copia
    std::vector<Chunk*> m_visibleChunks;
    size_t m_vertexTypeSize;

	// Para hacer un único draw call por chunk 
    // (prueba: la idea es juntar los del mismo LOD)
	// De momento, solo una malla por pase.
    std::vector<uint8_t> m_combinedVertexData;
    std::vector<UINT> m_combinedIndexData;
    size_t m_singleDrawVertexTypeSize = 0;

public:
	HRESULT Init() override;
    HRESULT InitManagers();
    HRESULT CreateDynamicBuffers();

    std::vector<PipelineOperationType> BeginPass(const MeshAsset* mesh, FrameStateService* renderState) override;
    std::vector<PipelineOperationType> ExecPass(const MeshAsset* mesh, FrameStateService* renderState) override;
    std::vector<PipelineOperationType> EndPass() override;

    std::map<std::string, std::shared_ptr<MeshAsset>> GetMeshes(const SceneManager* SceneManager, FrameStateService* renderState) override;

    std::shared_ptr<PassConfigBase> GetConfig() const override { return config; };

    void SetInitialOperations(const MeshAsset* mesh, FrameStateService* renderState);
};