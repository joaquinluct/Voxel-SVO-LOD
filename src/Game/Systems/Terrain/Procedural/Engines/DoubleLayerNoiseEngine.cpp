#include "DoubleLayerNoiseEngine.h"
#include <algorithm>
#include <cmath>
#include <FastNoiseLite/FastNoiseLite.h>

// Helper para smoothstep que usa C++
float DoubleLayerNoiseEngine::smoothstep_cpp(float edge0, float edge1, float x) const {
    // Asegurarse de que x está entre 0 y 1
    x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    // Función smoothstep: 3x^2 - 2x^3
    return x * x * (3.0f - 2.0f * x);
}

HRESULT DoubleLayerNoiseEngine::Init(const int& seed, const WorldTerrain::TerrainProceduralDefinition& def) {
    m_baseNoiseGenerator = new FastNoiseLite();
    m_detailNoiseGenerator = new FastNoiseLite();
    m_oceanFloorGenerator = new FastNoiseLite();
    m_def = def;

    if (!m_baseNoiseGenerator || !m_detailNoiseGenerator || !m_oceanFloorGenerator) {
        return E_FAIL;
    }

    // --- Generador de la Capa A: Morfología / Planicie ---
    m_baseNoiseGenerator->SetSeed(seed);
    m_baseNoiseGenerator->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_baseNoiseGenerator->SetFrequency(BASE_FREQ);
    m_baseNoiseGenerator->SetFractalType(FastNoiseLite::FractalType_None); // Simple, para grandes manchas.

    // --- Generador de la Capa B: Altura / Detalle ---
    m_detailNoiseGenerator->SetSeed(seed + 1); // Semilla diferente
    m_detailNoiseGenerator->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_detailNoiseGenerator->SetFrequency(DETAIL_FREQ);
    m_detailNoiseGenerator->SetFractalType(FastNoiseLite::FractalType_FBm);
    m_detailNoiseGenerator->SetFractalOctaves(5);
    m_detailNoiseGenerator->SetFractalLacunarity(2.0f);
    m_detailNoiseGenerator->SetFractalGain(0.5f);

    // --- Generador de Fondo Oceánico ---
    m_oceanFloorGenerator->SetSeed(seed + 2);
    m_oceanFloorGenerator->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_oceanFloorGenerator->SetFrequency(0.005f);

    return S_OK;
}

const float DoubleLayerNoiseEngine::GetHeight(const float& x, const float& z) const
{
    // --- 1. Lógica de Doble Capa de Ruido (Planicie + Altura) ---

    // a) Ruido Base (Baja Frecuencia): Define las regiones que deben ser planas.
    float biomeFactor = m_baseNoiseGenerator->GetNoise(x, z);
    float normalizedBiomeFactor = (biomeFactor + 1.0f) * 0.5f; // Rango [-1, 1] a [0, 1]

    // b) Ruido de Detalle (Alta Frecuencia): La altura real a aplicar.
    float detailHeightNoise = m_detailNoiseGenerator->GetNoise(x, z);

    // c) Factor de Planicie (Flattening Factor)
    // El factor será 1.0 (máxima planicie) en regiones de 'normalizedBiomeFactor' bajo.
    float flatnessFactor = 1.0f - smoothstep_cpp(FLATNESS_THRESHOLD - 0.1f, FLATNESS_THRESHOLD + 0.1f, normalizedBiomeFactor);

    // d) Mezcla de Altura: El detalle de altura se reduce en las zonas planas.
    float mixedHeightNoise = detailHeightNoise * (1.0f - flatnessFactor);

    // Reescalar la altura del terreno
    float terrainHeight = mixedHeightNoise * m_def.terrainHeight;


    // --- 2. Lógica de Desvanecimiento del Borde del Mundo (Mantenida) ---

    float halfWidth = m_def.worldWidth / 2.0f;
    float halfHeight = m_def.worldHeight / 2.0f;
    float transitionWidth = 512.0f;

    float oceanFloorNoise = m_oceanFloorGenerator->GetNoise(x, z);
    float oceanFloorHeight = m_def.waterLevel - (std::abs(oceanFloorNoise) * 5.0f);

    // Cálculo del fade factor (igual que en el código original)
    float distX = std::max(0.0f, std::abs(x) - (halfWidth - transitionWidth));
    float distZ = std::max(0.0f, std::abs(z) - (halfHeight - transitionWidth));
    float distanceToBoundary = std::max(distX, distZ);

    float fadeFactor = std::min(1.0f, distanceToBoundary / transitionWidth);
    fadeFactor = fadeFactor * fadeFactor * (3.0f - 2.0f * fadeFactor);

    // --- 3. Resultado Final ---

    float finalHeight = terrainHeight;
    if (fadeFactor > 0.0f) {
        // Interpolamos la altura final con el fondo oceánico en los bordes
        finalHeight = terrainHeight * (1.0f - fadeFactor) + oceanFloorHeight * fadeFactor;
    }

    // Si queremos que las planicies estén en la base (ej. nivel del mar), 
    // se puede añadir un desplazamiento vertical aquí:
    // return finalHeight + m_def.waterLevel;

    return finalHeight;
}
