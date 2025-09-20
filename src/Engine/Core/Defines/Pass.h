#pragma once

#include <vector>
#include <memory>

#include <Defines/TerrainChunk.h>
#include <Defines/Enums/Pass.h>
#include <Game/Systems/Terrain/Chunk/Chunk.h>
#include <Config/Base/Managers/RenderManagerConfig.h>

class Material;
class MeshAsset;

namespace RenderPasses {

    struct ExecPassTerrainParams {
		Chunk* chunk;
        Material* material;
        Material* shadowMaterial;
        std::shared_ptr<MeshAsset> mesh;
    };

    struct ExecPassParams {
        ~ExecPassParams() {}
        bool calulateConstantBuffers;
		ExecPassTerrainParams terrainParams;
    };
        
    inline static std::vector<RenderPassType> GetRenderPasses(int flags) {
        std::vector<RenderPassType> passes;
        if (flags & static_cast<int>(RenderPassType::ShadowPass)) {
            passes.push_back(RenderPassType::ShadowPass);
        }
        if (flags & static_cast<int>(RenderPassType::MainColorPass)) {
            passes.push_back(RenderPassType::MainColorPass);
        }
        if (flags & static_cast<int>(RenderPassType::TerrainPass)) {
            passes.push_back(RenderPassType::TerrainPass);
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
        if (flags & static_cast<int>(RenderPassType::WaterPass)) {
            passes.push_back(RenderPassType::WaterPass);
        }
        if (flags & static_cast<int>(RenderPassType::Reflection)) {
            passes.push_back(RenderPassType::Reflection);
        }
        if (flags & static_cast<int>(RenderPassType::Refraction)) {
            passes.push_back(RenderPassType::Refraction);
        }
        return passes;
    }

    inline bool IsRenderPassEnabled(int flags, RenderPassType pass) {
		return (flags & static_cast<int>(pass)) != 0;
    }
}