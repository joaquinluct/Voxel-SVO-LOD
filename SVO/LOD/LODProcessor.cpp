#include "LODProcessor.h"
#include <cmath>
#include "../../Util/LOD.cpp"

void LODProcessor::ProcessLOD(SVO_Node* node, const DirectX::XMFLOAT3& nodeOrigin, const DirectX::XMFLOAT3& cameraPosition, float nodeSize, const LODSettings& lodSettings, std::vector<VisibleNodeInfo>& visibleNodes) {
    if (!node || !node->IsOccupied()) return;

    DirectX::XMFLOAT3 nodeMin = nodeOrigin;
    DirectX::XMFLOAT3 nodeMax = { nodeOrigin.x + nodeSize, nodeOrigin.y + nodeSize, nodeOrigin.z + nodeSize };
    float distance = DistanceToAABB(cameraPosition, nodeMin, nodeMax);

    if (distance < lodSettings.nearDistance) {
        if (node->IsLeaf()) {
            visibleNodes.push_back({node, nodeOrigin, nodeSize});
        } else {
            float childSize = nodeSize / 2.0f;
            for (int i = 0; i < 8; ++i) {
                if (node->HasChild(i)) {
                    DirectX::XMFLOAT3 childOrigin = nodeOrigin;
                    if (i & 1) childOrigin.x += childSize;
                    if (i & 2) childOrigin.y += childSize;
                    if (i & 4) childOrigin.z += childSize;
                    ProcessLOD(node->GetChild(i), childOrigin, cameraPosition, childSize, lodSettings, visibleNodes);
                }
            }
        }
    } else if (distance < lodSettings.midDistance) {
        visibleNodes.push_back({node, nodeOrigin, nodeSize});
    }
    else if (distance < lodSettings.farDistance) {
        visibleNodes.push_back({ node, nodeOrigin, nodeSize });
    }
    // Si está más lejos que farDistance, no se añade el nodo
}
