#pragma once
#include <Config/Game/System/TerrainConfig.h>
#include <DirectXMath.h>

class LODManager {
public:
    static int GetLOD(
        const TerrainConfig& config,
        const DirectX::XMFLOAT3& cameraPosition,
        const DirectX::XMFLOAT3& targetPosition
    );
};

