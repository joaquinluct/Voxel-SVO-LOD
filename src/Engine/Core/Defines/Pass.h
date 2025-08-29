#pragma once
#include <Defines/TerrainChunk.h>
#include <Game/Systems/Terrain/Chunk/Chunk.h>

class Material;
class MeshAsset;

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