#pragma once

#include <Defines/EngineDefinition.h>
#include <Defines/Threading.h>
#include <future>
#include <memory>
#include <vector>

class DeviceManager;
class CameraManager;
class FrameStateService;
class World;
class Lighting;
class Chronos;
class Shadows;

using UpdateJob = Threading::Job;
using UpdatedJobs = std::vector<UpdateJob>;
using FutureUpdateJob = std::future<UpdateJob>;
using FutureUpdateJobs = std::vector<FutureUpdateJob>;

struct JobContext {
    EngineContext* engineContext;
    std::shared_ptr<DeviceManager> deviceManager;
    std::shared_ptr<CameraManager> cameraManager;
    std::shared_ptr<FrameStateService> frameStateService;
    std::shared_ptr<World> world;
    std::shared_ptr<Lighting> ligting;
    std::shared_ptr<Chronos> chronos;
    std::shared_ptr<Shadows> shadows;
};

