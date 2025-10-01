#pragma once

#include <atomic>
#include <mutex>

#include <RenderState/FrameStates/FrameStateBase.h>

//class FrameStateBase;


/*
* Camera
* Terrain
* Water
* Skybox
* Common
* Lighting
* Material
* Water
* Mesh
*/


namespace FrameStateDefinition
{
    struct FrameStateBuffer {
        FrameStateBuffer() : readIndex(1), writeIndex(0) {
            buffers[0] = nullptr;
            buffers[1] = nullptr;
        }
        std::mutex mutex;
        std::atomic<int> readIndex;
        std::atomic<int> writeIndex;
        FrameStateBase* buffers[2];
    };
}
