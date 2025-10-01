#pragma once
#include <map>

// Define los diferentes tipos de pases de renderizado
enum class RenderPassType : unsigned int {
    None = 0,
    ShadowPass = 1 << 0,       // 1
    MainColorPass = 1 << 1,    // 2
    PostProcessPass = 1 << 2,  // 4
    UIPass = 1 << 3,           // 8
    Skybox = 1 << 4,           // 16
    TerrainPass = 1 << 5,      // 32
    Debug = 1 << 6,            // 64
    Reflection = 1 << 7,       // 128
    Refraction = 1 << 8,       // 256
    WaterPass = 1 << 9,        // 512
};

// Mapeo de los valores del enum a sus nombres de cadena
const std::map<RenderPassType, std::string> passNames = {
    {RenderPassType::ShadowPass,    "ShadowPass"},
    {RenderPassType::MainColorPass, "MainColorPass"},
    {RenderPassType::PostProcessPass, "PostProcessPass"},
    {RenderPassType::UIPass,        "UIPass"},
    {RenderPassType::Skybox,        "Skybox"},
    {RenderPassType::TerrainPass,   "TerrainPass"},
    {RenderPassType::Debug,         "Debug"},
    {RenderPassType::Reflection,    "Reflection"},
    {RenderPassType::Refraction,    "Refraction"},
    {RenderPassType::WaterPass,     "WaterPass"},
};

inline std::vector<std::string> GetRenderPassNames(unsigned int passMask) {
    std::vector<std::string> activePasses;

    for (const auto& pair : passNames) {
        // Comprueba si el bit del pase está activado en la máscara
        if (static_cast<unsigned int>(pair.first) & passMask) {
            activePasses.push_back(pair.second);
        }
    }

    return activePasses;
}
