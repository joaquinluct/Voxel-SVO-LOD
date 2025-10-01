#include "Terrain.h"
#include <AssetLocator/AssetLocator.h>
#include <Assets/Base/TerrainAsset.h>
#include <ConfigLocator/ConfigLocator.h>
#include <Defines/CameraDefinition.h>
#include <Defines/WorldTerrain.h>
#include <DirectXMathConvert.inl>
#include <Game/Systems/Terrain/Chunk/Chunk.h>
#include <Game/Systems/Terrain/Procedural/Engines/ProceduralEngineBase.h>
#include <Game/Systems/Terrain/ProceduralService.h>
#include <iCamera.h>
#include <Locators/Registers/REGISTER_SERVICE_MACRO.h>
#include <ManagerLocator/ManagerLocator.h>
#include <memory>
#include <ServiceLocator/ServiceLocator.h>
#include <Util/DirectXUtils.h>
#include <Util/RayTracing/RayTracing.h>
#include <vector>

REGISTER_SERVICE_TYPE(Terrain, "Terrain")

Terrain::Terrain()
    : m_config{}, m_def{}, m_proceduralService{}, m_cameraManager{},
    m_chunkService{}, m_dirty{ true }, m_isGenerating{ false }, m_lastCameraPosition{ 0.0f, 0.0f, 0.0f }, m_chunkSize{ 10.0f }, m_materTerrainHeight{ 1.0f }, m_numChunks{ 0 }, m_numVisibleChunks{ 0 }
{
}

Terrain::~Terrain()
{
}

HRESULT Terrain::InitServices()
{
    m_cameraManager = ManagerLocator::GetManager<CameraManager>();
    if (!m_cameraManager) {
        OutputDebugStringA("Terrain: Register camera manager fail.\n");
        return E_FAIL;
    }
    m_chunkService = ServiceLocator::GetService<ChunkService>();
    if (!m_chunkService) {
        OutputDebugStringA("Terrain: Register render manager fail.\n");
        return E_FAIL;
    }
    m_proceduralService = ServiceLocator::GetService<ProceduralService>();
    if (!m_proceduralService) {
        return E_FAIL;
    }
    m_terrainAsset = AssetLocator::GetTerrainAsset("ProceduralTerrain").get();
    HRESULT hr = m_terrainAsset->Init();
    if (FAILED(hr)) {
        return hr;
    }

    m_chunkService->SetTerrainAsset(m_terrainAsset);

    return S_OK;
}

HRESULT Terrain::InitConfig()
{
    m_config = ConfigLocator::GetConfig<TerrainConfig>();

    if (m_config == nullptr) {
        return E_FAIL;
    }

    HRESULT hr = S_OK;
    int flags = m_config->flags;

    if (WorldTerrain::IsProceduralTerrain(flags)) {
        m_def.proceduralDefinition = {};
        m_def.proceduralDefinition.seed = m_config->seed;
        m_def.proceduralDefinition.engine = static_cast<Procedural::ProceduralEngine>(m_config->procedural_engine);
        m_def.proceduralDefinition.terrainHeight = m_config->terrainHeight;
        m_def.proceduralDefinition.waterLevel = m_config->waterLevel;
        m_def.proceduralDefinition.worldHeight = m_config->worldHeight;
        m_def.proceduralDefinition.worldWidth = m_config->worldWidth;
        hr = m_proceduralService->Init(m_def.proceduralDefinition);
        if (FAILED(hr)) {
            OutputDebugStringA("Terrain: Init procedural service fail.\n");
            return hr;
        }
    }
    if (WorldTerrain::IsChunksizedTerrain(flags)) {
        m_def.chunkDefinition = {};
        m_def.chunkDefinition.chunSize = static_cast<float>(m_config->chunk_size);
        m_chunkSize = m_def.chunkDefinition.chunSize;
        m_def.chunkDefinition.initialRenderDistanceChunks = m_config->initialRenderDistanceChunks;
        hr = m_chunkService->Init(m_def.chunkDefinition);
        if (FAILED(hr)) {
            OutputDebugStringA("Terrain: Init chunk service fail.\n");
            return hr;
        }
        if (m_proceduralService) {
            m_chunkService->SetProceduralService(m_proceduralService);
        }
    }
    return S_OK;
}

HRESULT Terrain::Init()
{
    HRESULT hr = InitServices();
    if (FAILED(hr)) {
        OutputDebugStringA("Terrain: Init services fail.\n");
        return hr;
    }

    hr = InitConfig();
    if (FAILED(hr)) {
        OutputDebugStringA("Terrain: Init date and time fail.\n");
        return hr;
    }

    return hr;
}

MeshAssetBase* Terrain::GetTerrainMesh() const {
    return (m_terrainAsset ? m_terrainAsset : nullptr);
}

void Terrain::GenerateMesh() {
    //std::vector<Chunk*> chunks = GetRawChunks(m_cameraManager->GetCurrentCamera());
    std::vector<Chunk*> chunks = GetChunks(m_cameraManager->GetCurrentCamera());
    m_terrainAsset->GenerateMesh(chunks, m_terrainAsset->GetWriteIndex());
}

// ------------------------------------------------------------------------------------------
// Update
// ------------------------------------------------------------------------------------------
void Terrain::Update(float deltaTime)
{
    XMFLOAT3 cameraPos = m_cameraManager->GetCurrentCameraPosition();

    if (DirectXUtils::Distance(cameraPos, m_lastCameraPosition) > m_chunkSize) {
        m_lastCameraPosition = cameraPos;
        m_dirty = true;
    }
    /*else {
        m_dirty = m_chunkService->IsDirty();
    }*/


    if (m_dirty && !m_isGenerating) {
        m_isGenerating = true;
        m_chunkService->UpdateChunks(cameraPos);
        m_numChunks = m_chunkService->GetNumChunks();
        m_dirty = false;
        m_isGenerating = false;
    }
}

std::vector<Chunk*> Terrain::GetVisibleChunks() {
    return m_chunkService->GetVisibleChunks();
}

std::vector<Chunk*> Terrain::GetChunks(std::shared_ptr<ICamera> camera) {
    if (!m_chunkService) {
        return {};
    }

    std::vector<CameraDefinition::FrustumPlane> frustumPlanes;
    XMFLOAT3 cameraPosition = m_cameraManager->GetCurrentCameraPosition();

    // 1. Extraer los planos del frustum
    m_cameraManager->ExtractCurrentFrustumPlanes(frustumPlanes);

    /*m_numVisibleChunks = static_cast<int>(m_chunkService->GetChunksAsVector().size());
    return m_chunkService->GetChunksAsVector();*/

    // 2. Obtener los chunks visibles en el frustum
    std::vector<Chunk*> visibleChunks = m_chunkService->GetFrustumChunks(frustumPlanes, cameraPosition);

    // # CODIGO PARA DEBUG
    m_numVisibleChunks = static_cast<int>(visibleChunks.size());
    return visibleChunks;
    // #FIN CODIGO PARA DEBUG

    // 3. Ordenar por distancia (los chunks más cercanos primero)
    std::sort(visibleChunks.begin(), visibleChunks.end(), [&camera](const auto& a, const auto& b) {
        return a->GetDistanceToCamera(camera) < b->GetDistanceToCamera(camera);
        });

    // 4. Aplicar Occlusion Culling
    std::vector<Chunk*> finalVisibleChunks;
    Util::RayTracing rayTracer;
    const DirectX::XMFLOAT3 cameraPos = camera->GetPosition();

    // Aquí está el cambio clave: crea una lista de oclusores.
    // Los primeros 20 chunks son candidatos a ocluir a otros.
    // Puedes ajustar este número para equilibrar rendimiento y precisión.
    int numOccluders = static_cast<int>(std::min(visibleChunks.size(), static_cast<size_t>(10)));
    std::vector<Chunk*> occluderChunks;
    for (int i = 0; i < numOccluders; ++i) {
        occluderChunks.push_back(visibleChunks[i]);
    }

    for (const auto& chunk : visibleChunks) {
        bool isOccluded = false;

        // Vector de dirección de la cámara al centro del chunk actual
        DirectX::XMFLOAT3 chunkCenter = chunk->GetAABB().GetCenter();
        DirectX::XMFLOAT3 dir = {
            chunkCenter.x - cameraPos.x,
            chunkCenter.y - cameraPos.y,
            chunkCenter.z - cameraPos.z
        };
        float maxDistance = rayTracer.Distance(cameraPos, chunkCenter);
        DirectX::XMVECTOR normalizedDir = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&dir));
        DirectX::XMStoreFloat3(&dir, normalizedDir);

        // Iterar sobre la lista de oclusores, no sobre la lista de chunks ya visibles
        for (const auto& occluderChunk : occluderChunks) {
            // Evita que un chunk se ocluya a sí mismo
            if (chunk == occluderChunk) continue;

            // Usa la función Trace de tu clase RayTracing
            float intersectionDistance = rayTracer.Trace(cameraPos, dir, maxDistance, occluderChunk->GetAABB());

            // Si hay una intersección y ocurre ANTES de que el rayo llegue al chunk actual...
            if (intersectionDistance > 0.0f && intersectionDistance < maxDistance) {
                isOccluded = true;
                break;
            }
        }

        if (!isOccluded) {
            finalVisibleChunks.push_back(chunk);
        }
    }
    m_numVisibleChunks = static_cast<int>(finalVisibleChunks.size());
    return finalVisibleChunks;
}

TextureDefines::CBTerrainBlendBuffer Terrain::GetTerrainBlenderData() {
    TextureDefines::CBTerrainBlendBuffer data = {};
    data.dirtHeight = 0.0f; // Default value, can be set later
    data.grassHeight = 50.0f;
    data.slopeEnd = 0.5f;
    data.slopeStart = 0.8f;
    data.snowHeight = 160.0f;
    data.terrainScale = 0.03f; // Default value, can be set later
    return data;
}

TextureDefines::CBTerrain2BlendBuffer Terrain::GetTerrain2BlenderData() {
    TextureDefines::CBTerrain2BlendBuffer data = {};
    data.grassTransitionHeight = 0.0f; // La hierba empieza desde la base
    data.grassTransitionSlope = 0.4f;  // Se mezcla a pendientes moderadas

    data.dirtTransitionHeight = 50.0f;  // La tierra empieza a aparecer a partir de 5m de altura
    data.dirtTransitionSlope = 0.5f;   // Se mezcla en pendientes moderadas a altas

    data.rockTransitionHeight = 150.0f; // La roca empieza a aparecer a 10m de altura
    data.rockTransitionSlope = 0.8f;   // Se mezcla en pendientes altas

    data.snowTransitionHeight = 200.0f; // La nieve aparece a partir de 15m de altura
    data.snowTransitionSlope = 0.4f;   // La nieve se acumula en pendientes bajas

    data.beachTransitionHeight = -10.0f; // La playa empieza desde la base
    data.beachTransitionSlope = 0.2f; // Se mezcla en pendientes suaves

    data.terrainScale = 0.002f; // Un valor bajo para que las texturas no se vean demasiado estiradas
    return data;
}

float Terrain::GetTerrainHeight(float x, float z) const
{
    if (m_proceduralService) {
        std::shared_ptr<ProceduralEngineBase> engine = m_proceduralService->GetEngine();
        return engine->GetHeight(x, z) * m_materTerrainHeight;
    }
    return 0.0f; // Default value if procedural service is not available
}

void Terrain::EmptyRecycleBin()
{
    if (m_chunkService)
    {
        m_chunkService->EmptyRecycleBin();
    }
}
