#pragma once

#include <atomic>
#include <mutex>

#include <RenderState/FrameStates/FrameStateBase.h>
#include <string_view>

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

inline constexpr std::string_view FRAME_STATE_CAMERA = "CameraFrameState";
inline constexpr std::string_view FRAME_STATE_COMMON = "CommonFrameState";
inline constexpr std::string_view FRAME_STATE_TERRAIN = "TerrainFrameState";
inline constexpr std::string_view FRAME_STATE_WATER = "WaterFrameState";
inline constexpr std::string_view FRAME_STATE_SKYBOX = "SkyboxFrameState";
inline constexpr std::string_view FRAME_STATE_LIGHTING = "LightingFrameState";
inline constexpr std::string_view FRAME_STATE_MATERIAL = "MaterialFrameState";
inline constexpr std::string_view FRAME_STATE_TIME = "TimeFrameState";
//inline constexpr std::string_view FRAME_STATE_MESH = "MeshFrameState";
//inline constexpr std::string_view FRAME_STATE_DEVICE = "DeviceFrameState";
inline constexpr std::string_view FRAME_STATE_PASS = "PassFrameState";
inline constexpr std::string_view FRAME_STATE_VIEWPORT = "ViewportFrameState";
inline constexpr std::string_view FRAME_STATE_SHADER = "ShaderFrameState";
inline constexpr std::string_view FRAME_STATE_PIPELINE = "PipelineFrameState";
inline constexpr std::string_view FRAME_STATE_RENDER = "RenderFrameState";

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
