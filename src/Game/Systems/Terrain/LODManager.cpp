#include "LODManager.h"
#include <cmath>
#include <DirectXMath.h>
#include <Game/System/TerrainConfig.h>
#include <string>

int LODManager::GetLOD(
    const TerrainConfig& config,
    const DirectX::XMFLOAT3& cameraPosition,
    const DirectX::XMFLOAT3& targetPosition
) {
    const std::string& strategy = config.geometry_lod_strategy;

    if (strategy == "Fixed") {
        return config.geometry_resolution;
    }

    float dx = cameraPosition.x - targetPosition.x;
    float dz = cameraPosition.z - targetPosition.z;
    float distance = std::sqrt(dx * dx + dz * dz);

    if (strategy == "Distance") {
        const auto& thresholds = config.geometry_lod_thresholds;
        for (int i = static_cast<int>(thresholds.size()) - 1; i >= 0; --i) {
            if (distance >= thresholds[i]) {
                return i;
            }
        }
        return 0;
    }

    if (strategy == "ScreenSpace") {
        // Aquí podrías usar tamaño proyectado en pantalla, ángulo de visión, etc.
        // Para ahora, lo tratamos como Distance.
        return config.geometry_resolution;
    }

    if (strategy == "Manual") {
        // En este caso, el LOD vendría de otro sistema (por chunk, por zona, etc.)
        return config.geometry_resolution;
    }

    return config.geometry_resolution; // Fallback
}
