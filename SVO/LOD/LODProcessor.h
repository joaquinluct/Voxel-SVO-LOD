#pragma once
#include "../SVOBase/SVO_Node.h"
#include <vector>
#include <DirectXMath.h>
#include "../../Util/Utils.h"

struct LODSettings {
    float nearDistance;
    float midDistance;
    float farDistance;
};

class LODProcessor {
public:
    static void ProcessLOD(SVO_Node* node, const DirectX::XMFLOAT3& nodeOrigin, const DirectX::XMFLOAT3& cameraPosition, float nodeSize, const LODSettings& lodSettings, std::vector<VisibleNodeInfo>& visibleNodes);
};
