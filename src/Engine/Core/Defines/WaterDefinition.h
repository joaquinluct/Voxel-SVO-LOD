#pragma once

#include <DirectXMath.h>

namespace WaterDefinition
{
	struct WaterData
	{
		float waveHeight; // Height of the water surface
		float waveSpeed;  // Speed of the waves
		float waveLength; // Length of the waves
		float waveFrequency; // Frequency of the waves
		float waterDensity; // Density of the water
		float waterViscosity; // Viscosity of the water
		float reflectionIntensity; // Intensity of reflections on the water surface
		float refractionIndex; // Index of refraction for light passing through water
		float foamAmount; // Amount of foam on the water surface
		float causticsIntensity; // Intensity of caustics effects on the bottom surface
		DirectX::XMFLOAT4 waterColor; // RGB color of the water
		bool isUnderwater; // Flag to indicate if the camera is underwater
		bool hasFoam; // Flag to indicate if foam effects are enabled
		bool hasCaustics; // Flag to indicate if caustics effects are enabled
		bool isReflective; // Flag to indicate if the water surface is reflective
		bool isRefractive; // Flag to indicate if the water surface is refractive
	};

	struct WaterInstancing
	{
		static const int MAX_INSTANCES = 256;
		DirectX::XMMATRIX instanceWorldMatrix[MAX_INSTANCES];
	};

	static WaterData GetTemporalWaterData() {
		return WaterDefinition::WaterData{
			.waveHeight = 0.3f,
			.waveSpeed = 0.3f,
			.waveLength = 25.0f,
			.waveFrequency = 0.8f,
			.waterDensity = 1.0f,
			.waterViscosity = 1.0f,
			.reflectionIntensity = 1.0f, // Valores por defecto para futuros efectos
			.refractionIndex = 1.333f, // El índice de refracción del agua
			.foamAmount = 0.0f, // Desactivado por ahora
			.causticsIntensity = 0.0f, // Desactivado por ahora
			.waterColor = { 0.0f, 0.5f, 0.8f, 1.0f },
			.isUnderwater = FALSE,
			.hasFoam = FALSE,
			.hasCaustics = FALSE,
			.isReflective = FALSE,
			.isRefractive = FALSE,
		};
	}
}
