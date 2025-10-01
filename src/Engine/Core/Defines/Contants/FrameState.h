#pragma once

#include <string_view>


inline constexpr std::string_view FRAME_STATE_CAMERA = "CameraFrameState";
inline constexpr std::string_view FRAME_STATE_MESHES = "MeshesFrameState";
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
inline constexpr std::string_view FRAME_STATE_CONSTANT_BUFFERS = "ConstantsBufferFrameState";
inline constexpr std::string_view FRAME_STATE_SHADER = "ShaderFrameState";
inline constexpr std::string_view FRAME_STATE_PIPELINE = "PipelineFrameState";
inline constexpr std::string_view FRAME_STATE_RENDER = "RenderFrameState";
