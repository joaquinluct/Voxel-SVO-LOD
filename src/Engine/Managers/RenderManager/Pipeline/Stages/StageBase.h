#pragma once

#include <DeviceManager.h>
#include <memory>

namespace RenderPipeline
{
    class StageBase
    {
    public:

    protected:
        std::shared_ptr<DeviceManager> m_deviceManager;
    };
}
