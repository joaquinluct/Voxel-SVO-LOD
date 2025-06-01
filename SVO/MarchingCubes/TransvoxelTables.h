#pragma once

#include <cstdint>

namespace Transvoxel {

// Estructura para celdas regulares (Marching Cubes)
// struct RegularCellData {
//     uint8_t vertexCount;
//     uint8_t triangleCount;
//     const uint8_t* triangles;
// };

struct RegularCellData
{
	unsigned char	geometryCounts;		// High nibble is vertex count, low nibble is triangle count.
	unsigned char	vertexIndex[15];	// Groups of 3 indexes giving the triangulation.
	
	long GetVertexCount(void) const
	{
		return (geometryCounts >> 4);
	}
	
	long GetTriangleCount(void) const
	{
		return (geometryCounts & 0x0F);
	}
};

// Estructura para celdas de transición (Transvoxel)
// struct TransitionCellData {
//     uint8_t vertexCount;
//     uint8_t triangleCount;
//     const uint8_t* triangles;
// };
struct TransitionCellData
{
	long			geometryCounts;		// High nibble is vertex count, low nibble is triangle count.
	unsigned char	vertexIndex[36];	// Groups of 3 indexes giving the triangulation.
	
	long GetVertexCount(void) const
	{
		return (geometryCounts >> 4);
	}
	
	long GetTriangleCount(void) const
	{
		return (geometryCounts & 0x0F);
	}
};

// Tablas oficiales Transvoxel (deben definirse en el .cpp)
extern const uint8_t regularCellClass[256];
extern const RegularCellData regularCellData[16];
extern const int regularVertexData[256][12];

extern const uint8_t transitionCellClass[512];
extern const TransitionCellData transitionCellData[56];
extern const uint8_t transitionCornerData[13];
extern const uint16_t transitionVertexData[512][12];

} // namespace Transvoxel
