#pragma once

#include <DirectXMath.h>
#include <cmath> // Para std::ceil
#include <cfloat> // Para FLT_MAX
#include <vector>
#include <utility> // Para std::pair
#include <Defines/TerrainChunk.h>

namespace ChunkHelper {
    /**
     * @brief Calcula la distancia de renderizado en chunks a partir de la distancia del far plane.
     * * @param chunkSize El tamaño de un chunk en unidades del mundo (e.g., 16.0f).
     * @param farPlane La distancia de renderizado de la cámara (e.g., 4500.0f).
     * @return La distancia en chunks, redondeada hacia arriba para cubrir el rango completo.
     */
    inline int GetChunkDistance(float chunkSize, float farPlane) {
        // La distancia en chunks es simplemente la distancia total dividida por el tamaño de cada chunk.
        // Usamos std::ceil para redondear al número entero superior.
        // Esto asegura que cubrimos por completo la distancia del farPlane, incluso si no es un múltiplo exacto del tamaño del chunk.
        return static_cast<int>(std::ceil(farPlane / chunkSize));
    }

    inline TerrainChunk::NeighborPair FindNeighboringVerticesInDirection(
        const DirectX::XMFLOAT3& targetPosition,
        const std::vector<DirectX::XMFLOAT3>& lod3Positions,
        TerrainChunk::NeighborDirection direction)
    {
        float targetCoord;
        auto getCoord = [&](const DirectX::XMFLOAT3& pos) -> float {
            switch (direction) {
            case TerrainChunk::NeighborDirection::NORTH:
            case TerrainChunk::NeighborDirection::SOUTH:
                return pos.x;
            case TerrainChunk::NeighborDirection::EAST:
            case TerrainChunk::NeighborDirection::WEST:
                return pos.z;
            }
            return 0.0f; // fallback
            };

        targetCoord = getCoord(targetPosition);

        DirectX::XMFLOAT3 lower = {};
        DirectX::XMFLOAT3 upper = {};
        float minLowerDist = FLT_MAX;
        float minUpperDist = FLT_MAX;

        for (const auto& pos : lod3Positions) {
            float coord = getCoord(pos);

            if (coord <= targetCoord) {
                float dist = std::abs(targetCoord - coord);
                if (dist < minLowerDist) {
                    minLowerDist = dist;
                    lower = pos;
                }
            }
            else {
                float dist = std::abs(coord - targetCoord);
                if (dist < minUpperDist) {
                    minUpperDist = dist;
                    upper = pos;
                }
            }
        }

        return { lower, upper };
    }
}