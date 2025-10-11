#include "Terrain.h"
#include <AssetLocator/AssetLocator.h>
#include <Assets/Base/TerrainAsset.h>
#include <ConfigLocator/ConfigLocator.h>
#include <Defines/Procedural.h>
#include <Defines/WorldTerrain.h>
#include <DirectXMath.h>
#include <Game/System/TerrainConfig.h>
// #include <Game/Systems/Terrain/Chunk/IChunkFactory.h> // Moved to Factory/IChunkFactory.h
class IChunk;
// #include <Game/Systems/Terrain/ChunkService.h> // Removed - ChunkService eliminated
#include <Game/Systems/Terrain/Factory/TerrainDistributionFactory.h>
#include <Game/Systems/Terrain/Factory/TerrainGeometryFactory.h>
#include <Game/Systems/Terrain/Engines/Geometry/TessellationGeometryEngine.h>
#include <Game/Systems/Terrain/Engines/Geometry/AggregatedGeometryEngine.h>
#include <Game/Systems/Terrain/Procedural/Engines/ProceduralEngineBase.h>
#include <Game/Systems/Terrain/ProceduralService.h>
#include <ManagerLocator/ManagerLocator.h>
#include <Managers/CameraManager.h>
#include <REGISTER_SERVICE_MACRO.h>
#include <ServiceLocator/ServiceLocator.h>
#include <Util/DirectXUtils.h>
#include <vector>
#include <Windows.h>

REGISTER_SERVICE_TYPE(Terrain, "Terrain")

Terrain::Terrain()
    : m_config{}, m_proceduralService{}, m_cameraManager{},
    m_terrainAsset(nullptr), m_chunkSize(10.0f), m_dirty(true), m_isGenerating(false),
    m_lastCameraPosition{ 0.0f, 0.0f, 0.0f }, m_geometryFactory{ nullptr }, m_terrainFactory{ nullptr }
{
}

Terrain::~Terrain() {}

HRESULT Terrain::Init()
{
    HRESULT hr = InitConfig();
    if (FAILED(hr)) return hr;

    hr = InitServices();
    if (FAILED(hr)) return hr;

    //hr = InitMesh();

    return S_OK;
}

HRESULT Terrain::InitMesh()
{
    m_terrainAsset = AssetLocator::GetTerrainAsset(m_config->shader).get();
    if (!m_terrainAsset) {
        return E_FAIL;
    }
    return m_terrainAsset->Init();
}

HRESULT Terrain::InitServices()
{
    m_cameraManager = ManagerLocator::GetManager<CameraManager>();
    // ChunkService removed: engines and factories provide chunk management via IChunkFactory
    m_proceduralService = ServiceLocator::GetService<ProceduralService>();
    m_terrainFactory = new TerrainDistributionFactory();
    m_geometryFactory = new TerrainGeometryFactory();

    // Inicializar engines simples según config (demo)
    if (m_config) {
        if (m_config->geometry_engine == "TessellationGeometryEngine") {
            // Static engine instance (see TerrainGeometryFactory.cpp)
            extern TessellationGeometryEngine s_tessEngine; // forward to static
            s_tessEngine.Init(m_config.get());
            OutputDebugStringA("[Terrain] TessellationGeometryEngine initialized from config.\n");
        }
        else if (m_config->geometry_engine == "AggregatedGeometryEngine") {
            extern AggregatedGeometryEngine s_aggEngine;
            s_aggEngine.Init(m_config.get());
            OutputDebugStringA("[Terrain] AggregatedGeometryEngine initialized from config.\n");
        }
    }

    if (!m_cameraManager || !m_proceduralService)
        return E_FAIL;

    //HRESULT hr = m_terrainAsset->Init();
    //if (FAILED(hr)) return hr;

    //m_chunkService->SetTerrainAsset(m_terrainAsset);

    m_terrainFactory->Init(m_config.get());

    //m_chunkSize = m_terrainFactory->


    return S_OK;
}

HRESULT Terrain::InitConfig()
{
    m_config = ConfigLocator::GetConfig<TerrainConfig>();
    if (!m_config) return E_FAIL;

    //m_chunkSize = static_cast<float>(m_config->chunk_size);

    m_terrainEngineName = m_config->terrain_engine;

    /*HRESULT hr = m_proceduralService->Init({
        .seed = m_config->seed,
        .engine = static_cast<Procedural::ProceduralEngine>(m_config->procedural_engine),
        .terrainHeight = m_config->terrainHeight,
        .waterLevel = m_config->waterLevel,
        .worldWidth = m_config->worldWidth,
        .worldHeight = m_config->worldHeight,
        });
    if (FAILED(hr)) return hr;

    hr = m_chunkService->Init({
        .chunSize = m_chunkSize,
        .initialRenderDistanceChunks = m_config->initialRenderDistanceChunks
        });
    if (FAILED(hr)) return hr;

    m_chunkService->SetProceduralService(m_proceduralService);*/
    return S_OK;
}

void Terrain::Update(float deltaTime)
{
    XMFLOAT3 cameraPos = m_cameraManager->GetCurrentCameraPosition();
    m_terrainFactory->UpdateTerrain(m_terrainEngineName, cameraPos);
}

std::vector<IChunk*> Terrain::GetVisibleChunks()
{
    // ChunkService removed - chunks now managed by TerrainDistributionFactory
    return std::vector<IChunk*>{};
}

float Terrain::GetTerrainHeight(float x, float z) const
{
    if (m_proceduralService) {
        //return m_proceduralService->GetEngine()->GetHeight(x, z) * m_config->terrainHeight;
    }
    return 0.0f;
}

void Terrain::Generate() {
    const auto& camera = m_cameraManager->GetCurrentCamera();
    m_terrainFactory->Generate(m_terrainEngineName, camera.get());
}

void Terrain::Render() {
    if (!m_terrainFactory) {
        OutputDebugStringA("[Terrain] No terrain factory available for rendering\n");
        return;
    }

    const auto& camera = m_cameraManager->GetCurrentCamera();
    if (!camera) {
        OutputDebugStringA("[Terrain] No active camera available for rendering\n");
        return;
    }

    OutputDebugStringA("[Terrain] Rendering terrain with engine: ");
    OutputDebugStringA(m_terrainEngineName.c_str());
    OutputDebugStringA("\n");

    m_terrainFactory->Render(m_terrainEngineName, camera.get());
}

void Terrain::BindForRender(ID3D11DeviceContext* context) {
    if (!m_config || !context) return;

    // TODO: Buffer binding is now handled by the specific chunk implementations
    // through the TerrainDistributionFactory and TerrainGeometryFactory
    // Each engine (TessellationGeometryEngine, AggregatedGeometryEngine) 
    // should handle its own buffer binding strategy

    // Fallback: if config requests Tessellation engine, bind engine resources
    if (m_config->geometry_engine == "TessellationGeometryEngine") {
        extern TessellationGeometryEngine s_tessEngine; // forward to static
        s_tessEngine.Bind(context);
        return;
    }

    // Otherwise no-op - specific engines handle their own binding
}
