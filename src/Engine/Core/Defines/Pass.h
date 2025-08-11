#pragma once

// Define los diferentes tipos de pases de renderizado
enum class RenderPassType : unsigned int {
    None = 0,
    ShadowPass = 1 << 0,
    MainColorPass = 1 << 1,
    PostProcessPass = 1 << 2,
    UIPass = 1 << 3,
    Skybox = 1 << 4,
    Debug = 1 << 5
};

namespace RenderPasses {

    inline static std::vector<RenderPassType> GetRenderPasses(int flags) {
        std::vector<RenderPassType> passes;
        if (flags & static_cast<int>(RenderPassType::ShadowPass)) {
            passes.push_back(RenderPassType::ShadowPass);
        }
        if (flags & static_cast<int>(RenderPassType::MainColorPass)) {
            passes.push_back(RenderPassType::MainColorPass);
        }
        if (flags & static_cast<int>(RenderPassType::PostProcessPass)) {
            passes.push_back(RenderPassType::PostProcessPass);
        }
        if (flags & static_cast<int>(RenderPassType::UIPass)) {
            passes.push_back(RenderPassType::UIPass);
        }
        if (flags & static_cast<int>(RenderPassType::Skybox)) {
            passes.push_back(RenderPassType::Skybox);
        }
        if (flags & static_cast<int>(RenderPassType::Debug)) {
            passes.push_back(RenderPassType::Debug);
        }
        return passes;
    }

    inline bool IsRenderPassEnabled(int flags, RenderPassType pass) {
		return (flags & static_cast<int>(pass)) != 0;
    }

}