#pragma once

#include <Defines/Types/ThreadTypes.h>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "IUpdateSystem.h"

class CameraManager;
class ShaderManager;
class SceneManager;
class Lighting;
class Water;
class World;
class Skybox;
class FrameStateService;
class ThreadPool;

// UpdateSystem: reemplaza a UpdateManager. Ejecuta las actualizaciones en el
// hilo principal y gestiona la encolación de trabajos asíncronos vía ThreadPool.
class UpdateSystem : public IUpdateSystem {
public:
    UpdateSystem();
    ~UpdateSystem() override;

    // Lifecycle
    HRESULT Init(EngineContext* context);
    HRESULT PostInit();
    void Shutdown();

    // Main update called from Engine::MainLoop
    void Update(float deltaTime) override;

    // Async job submission
    FutureUpdateJob AddUpdateJob(const std::string& name, std::function<bool()> task, bool allowDuplicates = false);

    // Drain ready futures into the provided map (thread-safe)
    void CollectReadyFutures(std::map<int, FutureUpdateJob>& outReady);

    // Utilities (compat)
    void SwapBuffers();
    void Lock();
    void Unlock();

    // State update helpers (exposed for modularity)
    UpdatedJobs UpdateMainData(float deltaTime);
    UpdatedJobs UpdateTerrainData(float deltaTime);

private:
    // Managers & services
    std::shared_ptr<CameraManager> m_cameraManager;
    std::shared_ptr<ShaderManager> m_shaderManager;
    std::shared_ptr<SceneManager> m_sceneManager;
    std::shared_ptr<FrameStateService> m_frameStateService;
    std::shared_ptr<Lighting> m_lighting;
    std::shared_ptr<World> m_world;
    std::shared_ptr<Water> m_water;
    std::shared_ptr<Skybox> m_skybox;

    // Threading
    std::shared_ptr<ThreadPool> m_threadPool;
    std::mutex m_mutex;
    std::map<int, FutureUpdateJob> m_futures;

    // Mesh being rendered (legacy)
    std::shared_ptr<class MeshAsset> m_mesh;
};
