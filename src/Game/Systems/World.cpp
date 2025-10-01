#include "World.h"
#include <Assets/Base/MeshAssetBase.h>
#include <chrono>
#include <ConfigLocator/ConfigLocator.h>
#include <Defines/Contants/Flags/World.h>
#include <Defines/WorldDefinition.h>
#include <Game/System/WorldConfig.h>
#include <Game/Systems/Skybox.h>
#include <Game/Systems/Terrain.h>
#include <ManagerLocator/ManagerLocator.h>
#include <Managers/CameraManager.h>
#include <REGISTER_SERVICE_MACRO.h>
#include <ServiceLocator/ServiceLocator.h>
#include <Services/Mouse.h>
#include <vector>
#include <Windows.h>

REGISTER_SERVICE_TYPE(World, "World")

World::World()
{
    //m_currentTime = new std::chrono::system_clock::time_point();
    m_timeSpeed = 1500;
    m_latitude = -30;
    m_chronos = nullptr;
    m_terrain = nullptr;
    m_gameFlags = WorldDefinition::Gameflags::None;
    m_text = nullptr;
    m_text2 = nullptr;
    m_water = nullptr;
    m_line = nullptr;
}

World::~World()
{
}

// ----------------------------------------------------------------
// Iniliza los managers necesarios para el sistema
// ----------------------------------------------------------------
HRESULT World::InitManagers() {
    HRESULT hr = S_OK;
    m_cameraManager = ManagerLocator::GetManager<CameraManager>();
    if (!m_cameraManager) {
        return E_FAIL;
    }
    m_mouse = ServiceLocator::GetService<Mouse>();
    if (!m_mouse) {
        return E_FAIL;
    }
    return hr;
}

// ----------------------------------------------------------------
// Iniliza la configuración del sistema
// ----------------------------------------------------------------
HRESULT World::InitConfig() {
    m_config = ConfigLocator::GetConfig<WorldConfig>();
    if (!m_config) {
        return E_FAIL;
    }
    m_gameFlags = static_cast<WorldDefinition::Gameflags>(m_config->gameFlags);
    return S_OK;
}

// ----------------------------------------------------------------
// Iniliza los subsistemas del sistema
// ----------------------------------------------------------------
HRESULT World::InitSystems() {
    HRESULT hr = S_OK;

    if (WorldDefinition::HasCronos(m_gameFlags)) {
        m_chronos = ServiceLocator::GetService<Chronos>();
        if (!m_chronos) {
            return E_FAIL;
        }
    }

    bool hasTerrain = WorldDefinition::HasTerrain(m_gameFlags);
    this->flags.SetFlag(FLAG_WORLD_HAS_TERRAIN, hasTerrain);

    if (hasTerrain) {
        m_terrain = ServiceLocator::GetService<Terrain>();
        if (!m_terrain) {
            return E_FAIL;
        }
        hr = m_terrain->Init();
    }
    return hr;
}

// ----------------------------------------------------------------
// Inilización general del mundo
// ----------------------------------------------------------------
HRESULT World::Init()
{
    HRESULT hr = InitManagers();
    if (FAILED(hr)) {
        return hr;
    }

    hr = InitConfig();
    if (FAILED(hr)) {
        return hr;
    }

    hr = InitSystems();
    if (FAILED(hr)) {
        return hr;
    }

    return hr; // Return success
}

// ----------------------------------------------------------------
// Post-inilización del mundo
// (después de que todos los sistemas han sido inicializados)
// ----------------------------------------------------------------
HRESULT World::PostInit()
{
    bool hasSkybox = WorldDefinition::HasSkybox(m_gameFlags);
    if (hasSkybox) {
        m_skybox = ServiceLocator::GetService<Skybox>();
        if (!m_skybox) {
            return E_FAIL;
        }
        HRESULT hr = m_skybox->Init();
        if (FAILED(hr)) {
            return hr;
        }
    }

    /*HRESULT hr = S_OK;
    std::shared_ptr<RenderManager> m_renderManager = ManagerLocator::GetManager<RenderManager>();
    m_text = m_renderManager->SceneManagerGet()->RegisterTextMeshAsUnique("UITextMesh", "World_texto1");
    m_text->SetPosition(10, 50);
    m_text->SetText("Texto1");
    hr = m_text->Init();
    if (FAILED(hr)) {
        return hr;
    }*/

    /*Material* material = new Material();
    material->SetShaderName(L"DefaultShader");
    material->Init();
    XMFLOAT3 startPos(0.0f, 0.0f, 0.0f);
    XMFLOAT3 endPos(0.0f, 100.0f, 0.0f);
    m_line = new Line(material, startPos, endPos, XMFLOAT4{ .0f,1.0f,.0f,1.0f });
    m_line->Init();*/

    /*m_text2 = m_renderManager->SceneManagerGet()->RegisterTextMeshAsUnique("UITextMesh", "World_texto2");
    m_text2->SetPosition(10, 20);
    m_text2->SetText("Texto2");
    return m_text2->Init();*/

    /*m_water = m_renderManager->SceneManagerGet()->RegisterMesh("OceanMesh");
    hr = m_water->Init();
    if (FAILED(hr)) {
        return hr;
    }*/

    //return hr;
    return S_OK;
}

// ----------------------------------------------------------------
// Actualización del mundo
// ----------------------------------------------------------------
void World::Update(float deltaTime)
{
    //if (m_chronos) {
    //	m_chronos->Update(deltaTime);
    //  //m_text->SetText(DateTime::FormatTimePoint(m_chronos->GetCurrentSimulatedDateTime()));
    //}
    if (m_terrain) {
        m_terrain->Update(deltaTime);
    }

    //m_text->SetText("Pos: " + DirectXUtils::ToString(m_cameraManager->GetCurrentCameraPosition()));


    /*m_text->SetText(
        "Chunks Totales: " + ParseInt(m_terrain->GetNumChunks()) +
        " Chunks visibles: " + ParseInt(m_terrain->GetNumVisibleChunks()) +
        " Posición " + DirectXUtils::ToString(m_cameraManager->GetCurrentCameraPosition()) +
        " Altura: " + ParseFloat(m_terrain->GetTerrainHeight(m_cameraManager->GetCurrentCameraPosition().x, m_cameraManager->GetCurrentCameraPosition().z)) +
        m_cameraManager->GetDebugInfo()
    );*/

    //m_line->Render();

    /*m_text2->SetText(
        "Chunks visibles: " + ParseInt(m_terrain->GetNumVisibleChunks())
    );*/
}

const std::chrono::system_clock::time_point& World::GetCurrentDateTime() {
    std::chrono::system_clock::time_point result = std::chrono::system_clock::now();
    if (m_chronos) {
        result = m_chronos->GetCurrentDateTime();
    }

    return result;
}
const std::chrono::system_clock::time_point& World::GetCurrentSimulatedDateTime() {
    std::chrono::system_clock::time_point result = std::chrono::system_clock::now();
    if (m_chronos) {
        result = m_chronos->GetCurrentSimulatedDateTime();
    }
    return result;
}

// ----------------------------------------------------------------
// Devuelve un vector con todos los meshes del mundo
// (skybox, terrain, water, etc)
// ----------------------------------------------------------------
const std::vector<MeshAssetBase*> World::GetMeshes() const
{
    bool hasSkybox = WorldDefinition::HasSkybox(m_gameFlags);
    bool hasTerrain = WorldDefinition::HasTerrain(m_gameFlags);

    std::vector<MeshAssetBase*> meshes;
    if (hasSkybox && m_skybox) {
        MeshAssetBase* skyboxMesh = m_skybox->GetMesh();
        if (skyboxMesh) {
            meshes.push_back(skyboxMesh);
        }
    }
    if (hasTerrain && m_terrain) {
        MeshAssetBase* terrainAsset = m_terrain->GetTerrainMesh();
        if (terrainAsset) {
            meshes.push_back(terrainAsset);
        }
    }
    return meshes;
}

bool World::HasHeight() const {
    return (m_terrain != nullptr);
}

void World::Shutdown() {
    if (m_text) {
        m_text->Shutdown();
    }
    if (m_text2) {
        m_text2->Shutdown();
    }
}
