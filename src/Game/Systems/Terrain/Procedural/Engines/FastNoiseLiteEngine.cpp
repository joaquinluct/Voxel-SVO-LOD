#include "FastNoiseLiteEngine.h"
#include <algorithm> // Mantenemos el include, es necesario
#include <Windows.h>

HRESULT FastNoiseLiteEngine::Init(const int& seed, const WorldTerrain::TerrainProceduralDefinition& def) {
    m_generator = new FastNoiseLite();
    m_oceanFloorGenerator = new FastNoiseLite();
    m_def = def;

    if (!m_generator || !m_oceanFloorGenerator) {
        return E_FAIL; // Error al crear los generadores de ruido
    }

    //GenerateValleys(seed); // Configura el generador principal para valles
    GeneratePlain(seed); // Configura el generador principal para llanuras

    // Configurar el generador para el fondo oceánico, con un perfil diferente
    m_oceanFloorGenerator->SetSeed(seed + 1); // Semilla diferente
    m_oceanFloorGenerator->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_oceanFloorGenerator->SetFrequency(0.005f);
    m_oceanFloorGenerator->SetFractalType(FastNoiseLite::FractalType_FBm);
    m_oceanFloorGenerator->SetFractalOctaves(4);
    m_oceanFloorGenerator->SetFractalLacunarity(2.0f);
    m_oceanFloorGenerator->SetFractalGain(0.5f);

    return S_OK; // Inicialización exitosa
}

const float FastNoiseLiteEngine::GetHeight(const float& x, const float& z) const
{
    float halfWidth = m_def.worldWidth / 2.0f;
    float halfHeight = m_def.worldHeight / 2.0f;
    float transitionWidth = 512.0f; // La zona de desvanecimiento, ajusta este valor como prefieras.

    // Paso 1: Generar la altura del terreno normal
    float baseNoise = m_generator->GetNoise(x * 0.5f, z * 0.5f);
    float detailNoise = m_generator->GetNoise(x * 1.5f, z * 1.5f) * 0.5f;
    float combinedNoise = baseNoise + detailNoise;
    float terrainHeight = combinedNoise * m_def.terrainHeight;

    // Paso 2: Generar la altura del fondo oceánico
    float oceanFloorNoise = m_oceanFloorGenerator->GetNoise(x * 0.05f, z * 0.05f);
    // **CORRECCIÓN:** Ahora restamos el ruido para crear una depresión bajo el nivel del agua.
    float oceanFloorHeight = m_def.waterLevel - (std::abs(oceanFloorNoise) * 5.0f); // Usamos std::abs para asegurar que siempre sea una fosa.

    // Paso 3: Calcular la distancia al límite más cercano y el factor de desvanecimiento
    float distX = std::max(0.0f, std::abs(x) - (halfWidth - transitionWidth));
    float distZ = std::max(0.0f, std::abs(z) - (halfHeight - transitionWidth));
    float distanceToBoundary = std::max(distX, distZ);

    // Normalizamos la distancia para obtener un factor de 0.0 a 1.0
    float fadeFactor = std::min(1.0f, distanceToBoundary / transitionWidth);

    // Utilizamos una interpolación más suave (Smoothstep) para el desvanecimiento
    fadeFactor = fadeFactor * fadeFactor * (3 - 2 * fadeFactor);

    // Paso 4: Mezclar las dos alturas
    float finalHeight = terrainHeight;
    if (fadeFactor > 0.0f) {
        // Interpolamos la altura final entre la del terreno y la del fondo oceánico
        finalHeight = terrainHeight * (1.0f - fadeFactor) + oceanFloorHeight * fadeFactor;
    }

    return finalHeight;
}

// Ahora sin 'const' en el tipo de retorno
void FastNoiseLiteEngine::GeneratePlain(const int& seed)
{
    m_generator->SetSeed(seed);
    m_generator->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_generator->SetFrequency(0.0008f);
    m_generator->SetFractalType(FastNoiseLite::FractalType_FBm);
    m_generator->SetFractalOctaves(1);
    m_generator->SetFractalLacunarity(0.5f);
    m_generator->SetFractalGain(0.1f);
}

// Ahora sin 'const' en el tipo de retorno
void FastNoiseLiteEngine::GenerateValleys(const int& seed)
{
    m_generator->SetSeed(seed);
    m_generator->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_generator->SetFrequency(0.0029f);
    m_generator->SetFractalType(FastNoiseLite::FractalType_FBm);
    m_generator->SetFractalOctaves(3);
    m_generator->SetFractalLacunarity(1.5f);
    m_generator->SetFractalGain(0.6f);
}

// Ahora sin 'const' en el tipo de retorno
void FastNoiseLiteEngine::GenerateMountains(const int& seed)
{
    m_generator->SetSeed(seed);
    m_generator->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_generator->SetFrequency(0.005f);
    m_generator->SetFractalType(FastNoiseLite::FractalType_FBm);
    m_generator->SetFractalOctaves(2);
    m_generator->SetFractalLacunarity(1.5f);
    m_generator->SetFractalGain(0.1f);
}
