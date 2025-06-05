#pragma once

#include <cstdint>

#include<DirectXMath.h>

using namespace DirectX;

namespace MarchingCubesTables {
	extern const int edgeTable[256];
	extern const int triTable[256][16];
	extern const int cornerOffsets[8][3];
	extern float cornerOffsetsF[8][3];
	extern XMFLOAT3 cornerOffsetsX[8];
};