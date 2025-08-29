#pragma once

#include <DirectXMath.h>
#include <memory>
#include <vector>
#include "FrameStateBase.h"

namespace WaterDefinition {
    struct WaterData;
}

class WaterFrameState : public FrameStateBase {
public:
    std::vector<DirectX::XMMATRIX> GetWaterTileTransformMatrices() const { return waterMatrices; }
	void SetWaterTileTransformMatrices(const std::vector<DirectX::XMMATRIX>& matrices) { waterMatrices = matrices; }
    WaterDefinition::WaterData GetWaterData() const { return waterData; }
	void SetWaterData(const WaterDefinition::WaterData& data) { waterData = data; }
private:
    std::vector<DirectX::XMMATRIX> waterMatrices; // Matrices de transformación para los tiles de agua
	WaterDefinition::WaterData waterData; // Datos específicos del agua
};

