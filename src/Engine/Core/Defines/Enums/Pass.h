#pragma once

// Define los diferentes tipos de pases de renderizado
enum class RenderPassType : unsigned int {
	None = 0,
	ShadowPass = 1 << 0,       // 1
	MainColorPass = 1 << 1,    // 2
	PostProcessPass = 1 << 2,  // 4
	UIPass = 1 << 3,           // 8
	Skybox = 1 << 4,           // 16
	TerrainPass = 1 << 5,      // 32
	Debug = 1 << 6,            // 64
	Reflection = 1 << 7,       // 128
	Refraction = 1 << 8,       // 256
	WaterPass = 1 << 9,        // 512
};
