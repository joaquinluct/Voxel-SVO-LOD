// Floor.h
#pragma once
#include <vector>
#include <cstdint>
#include "Defines/VertexDefinition.h"
#include <Assets/Base/MeshAsset.h>
#include <Assets/IAssetVertexConfig.h>

class FloorVertexDef : public IAssetVertexConfig<VertexDefinition::SimpleVertex> {
public:
    FloorVertexDef() = default;
    //~SkyboxCube() override {};
    const std::string& GetAssetName() {
        static const std::string assetName = "FloorVertexDef";
        return assetName;
    }

    std::vector<VertexDefinition::SimpleVertex> GetVertex() {
        return {
            {0.0f, 0.0f, 0.0f}, {100.0f,  0.0f, 0.0f}, {100.0f, 0.0f, 100.0f}, {0.0f,  0.0f,  100.0f}
        };
    }

    std::vector<uint16_t> GetIndex() {
        return { 0, 1, 2, 0, 2, 3};
    };
};
