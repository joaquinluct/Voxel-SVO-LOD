#pragma once

#include <Defines/Procedural.h>
#include <cstdint>

namespace WorldTerrain
{
	enum class TerrainFlags
	{
		Undefined = 0,
		Infinite = 1 << 0,
		Procedural = 2 << 0,
		Chunk = 3 << 0,
		Voxel = 4 << 0,
		LOD = 5 << 0
	};

	inline bool IsInfiniteTerrain(int flags) {
		return flags & static_cast<int>(TerrainFlags::Infinite);
	}

	inline bool IsProceduralTerrain(int flags) {
		return flags & static_cast<int>(TerrainFlags::Procedural);
	}

	inline bool IsChunksizedTerrain(int flags) {
		return flags & static_cast<int>(TerrainFlags::Chunk);
	}

	inline bool IsVoxelizedTerrain(int flags) {
		return flags & static_cast<int>(TerrainFlags::Voxel);
	}

	inline bool IsLODTerrain(int flags) {
		return flags & static_cast<int>(TerrainFlags::LOD);
	}

	struct TerrainFiniteDefinition
	{
		float width;
		float height;
	};

	struct TerrainProceduralDefinition
	{
		int32_t seed;
		Procedural::ProceduralEngine engine;

		float terrainHeight; // Multipicador para la altura del terreno
		float waterLevel;

		float worldWidth;
		float worldHeight;
	};

	struct TerrainChunkDefinition
	{
		float chunSize;
		int32_t initialRenderDistanceChunks;
	};

	struct TerrainDefinition
	{
		TerrainFlags flags;
		TerrainFiniteDefinition finiteDefinition;
		TerrainProceduralDefinition proceduralDefinition;
		TerrainChunkDefinition chunkDefinition;
	};
}
