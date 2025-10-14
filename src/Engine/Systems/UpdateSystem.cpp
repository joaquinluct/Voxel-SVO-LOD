#include "UpdateSystem.h"
#include <Locators/ServiceLocator/ServiceLocator.h>
#include <Locators/ManagerLocator/ManagerLocator.h>
#include <Managers/UpdateManager.h> // for legacy migration reference
#include <Defines/Contants/FrameState.h>
// Avoid including heavy headers here to prevent template instantiation
// issues in translation units that include UpdateSystem.cpp.
#include <Services/FrameStateService.h>
#include <Services/ThreadPool.h>
#include <Locators/ServiceLocator/ServiceLocator.h>
#include <Locators/ManagerLocator/ManagerLocator.h>
#include <Game/Systems/World.h>
#include <Game/Systems/Water.h>
#include <Game/Systems/Lighting.h>
#include <Managers/SceneManager.h>
#include <Managers/CameraManager.h>
#include <Managers/ShaderManager.h>

UpdateSystem::UpdateSystem() {
}

UpdateSystem::~UpdateSystem() {
}

HRESULT UpdateSystem::Init(EngineContext* context) {
    // Obtain services and managers
    // Obtain services and managers via locators. Include headers where
    // necessary to avoid instantiation of templates with incomplete types.
    m_threadPool = ServiceLocator::GetService<ThreadPool>();
    m_frameStateService = ServiceLocator::GetService<FrameStateService>();
    m_lighting = ServiceLocator::GetService<Lighting>();
    m_world = ServiceLocator::GetService<World>();
    m_water = ServiceLocator::GetService<Water>();

    m_cameraManager = ManagerLocator::GetManager<CameraManager>();
    m_shaderManager = ManagerLocator::GetManager<ShaderManager>();
    m_sceneManager = ManagerLocator::GetManager<SceneManager>();

    return S_OK;
}

HRESULT UpdateSystem::PostInit() {
    return S_OK;
}

void UpdateSystem::Shutdown() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_futures.clear();
}

void UpdateSystem::Update(float deltaTime) {
    // Execute synchronous update steps on main thread
    UpdateMainData(deltaTime);
    UpdateTerrainData(deltaTime);

    // Process any ready futures (move to caller or process here)
    std::map<int, FutureUpdateJob> ready;
    CollectReadyFutures(ready);
    for (auto &p : ready) {
        UpdateJob job = p.second.get();
        if (job.isSuccessful) {
            if (job.name == FRAME_STATE_TERRAIN) {
                if (m_frameStateService) m_frameStateService->SwapBuffer(FRAME_STATE_PIPELINE);
                continue;
            }
            if (m_frameStateService) m_frameStateService->SwapBuffer(job.name);
        }
    }
}

FutureUpdateJob UpdateSystem::AddUpdateJob(const std::string& name, std::function<bool()> task, bool allowDuplicates) {
    auto cancel_token = std::make_shared<std::atomic<bool>>(false);

    auto wrappedTask = [task, name, allowDuplicates, cancel_token]() -> UpdateJob {
        UpdateJob job;
        job.name = name;
        job.allowDuplicates = allowDuplicates;
        job.startTime = std::chrono::high_resolution_clock::now();
        job.cancel_token = cancel_token;
        job.isSuccessful = false;
        if (!cancel_token->load()) {
            job.isSuccessful = task();
        }
        return job;
    };

    if (!m_threadPool) return FutureUpdateJob();
    FutureUpdateJob future = m_threadPool->enqueue(wrappedTask);
    std::lock_guard<std::mutex> lock(m_mutex);
    m_futures[std::rand()] = std::move(future);
    // Return a future handle (legacy behaviour: may be empty) to caller to
    // preserve compatibility with original UpdateManager API.
    return FutureUpdateJob();
}

void UpdateSystem::CollectReadyFutures(std::map<int, FutureUpdateJob>& outReady) {
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto it = m_futures.begin(); it != m_futures.end(); ) {
        auto &future = it->second;
        if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            outReady.emplace(it->first, std::move(future));
            it = m_futures.erase(it);
        } else {
            ++it;
        }
    }
}

void UpdateSystem::SwapBuffers() {
    if (m_frameStateService) m_frameStateService->SwapBuffers();
}

void UpdateSystem::Lock() { if (m_frameStateService) m_frameStateService->LockAll(); }
void UpdateSystem::Unlock() { if (m_frameStateService) m_frameStateService->UnlockAll(); }

UpdatedJobs UpdateSystem::UpdateMainData(float deltaTime) {
    UpdatedJobs updatedStates{};
    if (!m_frameStateService) return updatedStates;

    FrameStateBase* state = nullptr;
    state = nullptr; // placeholder to mirror previous logic
    return updatedStates;
}

UpdatedJobs UpdateSystem::UpdateTerrainData(float deltaTime) {
    UpdatedJobs updatedStates{};
    if (!m_frameStateService) return updatedStates;
    FrameStateBase* state = nullptr;
    state = nullptr;
    return updatedStates;
}
