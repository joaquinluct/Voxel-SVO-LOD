#pragma once
#include <Assets/Base/MeshAssetBase.h>
#include <CameraManager.h>
#include <chrono>
#include <Config/Game/System/WorldConfig.h>
#include <Defines/WorldDefinition.h>
#include <Game/Systems/Chronos.h>
#include <Game/Systems/Skybox.h>
#include <Game/Systems/Terrain.h>
#include <IService.h>
#include <memory>
#include <string>
#include <Threading/SyncFlagged.h>
#include <UI/UIText.h>  
#include <vector>
#include <windows.h>

class Mouse;
class Line;
class MeshAsset;

class World : public IService
{
public:
    World();
    ~World() override;

    HRESULT Init() override;
    HRESULT InitManagers();
    HRESULT PostInit();
    HRESULT InitConfig();
    HRESULT InitSystems();
    void Update(float deltaTime) override;
    void Shutdown() override;
    const std::string& GetServiceName() const override {
        static const std::string name = "World";
        return name;
    }
    static const std::string& GetStaticServiceName()
    {
        static const std::string name = "World";
        return name;
    }

    bool HasHeight() const;

    std::shared_ptr<Terrain> GetTerrain() const { return m_terrain; }

    const std::chrono::system_clock::time_point& GetCurrentDateTime();
    const std::chrono::system_clock::time_point& GetCurrentSimulatedDateTime();
    const float GetCurrentLatitude() const { return m_latitude; }

    const std::vector<MeshAssetBase*> GetMeshes() const;

    SyncFlagged flags;
private:
    Line* m_line;
    WorldDefinition::Gameflags m_gameFlags;
    std::shared_ptr<CameraManager> m_cameraManager;
    std::shared_ptr<WorldConfig> m_config;
    std::shared_ptr<Chronos> m_chronos;
    std::shared_ptr<Terrain> m_terrain;
    std::shared_ptr<Skybox> m_skybox;
    std::unique_ptr<UIText> m_text;
    std::unique_ptr<UIText> m_text2;
    std::shared_ptr<Mouse> m_mouse;
    std::shared_ptr<MeshAsset> m_water;
    int m_timeSpeed;
    float m_latitude;
};
