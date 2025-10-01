#include "ProceduralBiomaEngine.h"
#include <algorithm>
#include <cmath>
#include <Defines/Structs/Procedural/AreaMapping.h>
#include <Defines/WorldTerrain.h>
#include <FastNoiseLite/FastNoiseLite.h>
#include <Windows.h>

ProceduralBiomaEngine::ProceduralBiomaEngine() : ProceduralEngineBase()
{
    m_areaGenerator = nullptr;
    m_detailGenerator = nullptr;
}

ProceduralBiomaEngine::~ProceduralBiomaEngine() {
    if (m_areaGenerator) {
        delete m_areaGenerator;
        m_areaGenerator = nullptr;
    }
    if (m_detailGenerator) {
        delete m_detailGenerator;
        m_detailGenerator = nullptr;
    }
}

//HRESULT ProceduralBiomaEngine::Init(const int& seed, const WorldTerrain::TerrainProceduralDefinition& def)
//{
//    m_areaGenerator = new FastNoiseLite();
//    m_detailGenerator = new FastNoiseLite();
//    m_def = def;
//    
//    // Configuración del Generador de Área (Worley Noise)
//    m_areaGenerator->SetSeed(seed);
//    m_areaGenerator->SetNoiseType(FastNoiseLite::NoiseType_Cellular);
//    m_areaGenerator->SetFrequency(m_areaDef.globalAreaScale);
//    m_areaGenerator->SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Euclidean);
//    
//    // Configuración del Generador de Detalle (Inicial)
//    m_detailGenerator->SetSeed(seed + 1); // Semilla diferente
//    m_detailGenerator->SetFractalType(FastNoiseLite::FractalType_FBm);
//    
//    return S_OK;
//}

// Implementación hash simple (para obtener un índice determinista)
inline static int HashBiomeSelection(int seed, int cellID, size_t numBiomeTypes) {
    // Se asegura de que el hash sea determinístico basado en la celda y la semilla
    unsigned int hash = (unsigned int)seed * 31 + (unsigned int)cellID;
    hash = (hash ^ (hash >> 16)) * 0x85ebca6b;
    hash = (hash ^ (hash >> 13)) * 0xc2b2ae35;
    hash = hash ^ (hash >> 16);
    return hash % numBiomeTypes;
}

float ProceduralBiomaEngine::smoothstep_cpp(float edge0, float edge1, float x) const {
    x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return x * x * (3.0f - 2.0f * x);
}

HRESULT ProceduralBiomaEngine::Init(const int& seed, const WorldTerrain::TerrainProceduralDefinition& def) {
    m_areaGenerator = new FastNoiseLite();
    m_detailGenerator = new FastNoiseLite();
    m_def = def;
    // ... Cargar o definir m_areaDef y m_biomeDefinitions ...

    m_biomeDefinitions = {};
    AreaTypeDefinition desertBiome;
    desertBiome.name = "Desert";
    desertBiome.biomeId = 0;
    desertBiome.color[0] = 0.93f; desertBiome.color[1] = 0.79f; desertBiome.color[2] = 0.69f; desertBiome.color[3] = 1.0f;
    desertBiome.baseHeightOffset = 20.0f;
    desertBiome.minHeightConstraint = 15.0f;
    desertBiome.maxHeightConstraint = 50.0f;
    desertBiome.noiseAmplitude = 5.0f;
    desertBiome.noiseConfig.noiseType = FastNoiseLite::NoiseType_Perlin;
    desertBiome.noiseConfig.fractalType = FastNoiseLite::FractalType_FBm;
    desertBiome.noiseConfig.frequency = 0.01f;
    desertBiome.noiseConfig.octaves = 4;
    desertBiome.noiseConfig.lacunarity = 2.0f;
    desertBiome.noiseConfig.gain = 0.5f;
    m_biomeDefinitions.push_back(desertBiome);
    /* AreaTypeDefinition forestBiome;
     forestBiome.name = "Forest";
     forestBiome.biomeId = 1;
     forestBiome.color[0] = 0.13f; forestBiome.color[1] = 0.55f; forestBiome.color[2] = 0.13f; forestBiome.color[3] = 1.0f;
     forestBiome.baseHeightOffset = 30.0f;
     forestBiome.minHeightConstraint = 25.0f;
     forestBiome.maxHeightConstraint = 80.0f;
     forestBiome.noiseAmplitude = 15.0f;
     forestBiome.noiseConfig.noiseType = FastNoiseLite::NoiseType_Perlin;
     forestBiome.noiseConfig.fractalType = FastNoiseLite::FractalType_FBm;
     forestBiome.noiseConfig.frequency = 0.02f;
     forestBiome.noiseConfig.octaves = 5;
     forestBiome.noiseConfig.lacunarity = 2.0f;
     forestBiome.noiseConfig.gain = 0.5f;
     m_biomeDefinitions.push_back(forestBiome);
     AreaTypeDefinition mountainBiome;
     mountainBiome.name = "Mountain";
     mountainBiome.biomeId = 2;
     mountainBiome.color[0] = 0.5f; mountainBiome.color[1] = 0.5f; mountainBiome.color[2] = 0.5f; mountainBiome.color[3] = 1.0f;
     mountainBiome.baseHeightOffset = 100.0f;
     mountainBiome.minHeightConstraint = 80.0f;
     mountainBiome.maxHeightConstraint = 300.0f;
     mountainBiome.noiseAmplitude = 50.0f;
     mountainBiome.noiseConfig.noiseType = FastNoiseLite::NoiseType_Perlin;
     mountainBiome.noiseConfig.fractalType = FastNoiseLite::FractalType_Ridged;
     mountainBiome.noiseConfig.frequency = 0.005f;
     mountainBiome.noiseConfig.octaves = 6;
     mountainBiome.noiseConfig.lacunarity = 2.0f;
     mountainBiome.noiseConfig.gain = 0.5f;
     m_biomeDefinitions.push_back(mountainBiome);
     AreaTypeDefinition plainsBiome;
     plainsBiome.name = "Plains";
     plainsBiome.biomeId = 3;
     plainsBiome.color[0] = 0.5f; plainsBiome.color[1] = 1.0f; plainsBiome.color[2] = 0.5f; plainsBiome.color[3] = 1.0f;
     plainsBiome.baseHeightOffset = 10.0f;
     plainsBiome.minHeightConstraint = 5.0f;
     plainsBiome.maxHeightConstraint = 30.0f;
     plainsBiome.noiseAmplitude = 3.0f;
     plainsBiome.noiseConfig.noiseType = FastNoiseLite::NoiseType_Perlin;
     plainsBiome.noiseConfig.fractalType = FastNoiseLite::FractalType_FBm;
     plainsBiome.noiseConfig.frequency = 0.015f;
     plainsBiome.noiseConfig.octaves = 4;
     plainsBiome.noiseConfig.lacunarity = 2.0f;
     plainsBiome.noiseConfig.gain = 0.5f;
     m_biomeDefinitions.push_back(plainsBiome);*/


     // --- Configuración del Generador de Área (Worley Noise) ---
    m_areaGenerator->SetSeed(seed);
    m_areaGenerator->SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    m_areaGenerator->SetFrequency(m_areaDef.globalAreaScale);
    m_areaGenerator->SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Euclidean);
    // Nota: El retorno se cambia dinámicamente en GetHeight.

    // --- Configuración del Generador de Detalle (Inicial) ---
    m_detailGenerator->SetSeed(seed + 1); // Semilla diferente
    m_detailGenerator->SetFractalType(FastNoiseLite::FractalType_FBm);

    return S_OK;
}

// Helper: Obtiene el índice de bioma basado en CellValue y un offset de coordenadas.
int ProceduralBiomaEngine::GetBiomeIndex(const float& x, const float& z, const float& coordOffsetX, const float& coordOffsetZ) const {
    // 1. Configurar para obtener el valor de la celda (hash del ID).
    m_areaGenerator->SetCellularReturnType(FastNoiseLite::CellularReturnType_CellValue);

    // 2. Obtener el valor de la celda.
    float cellValue = m_areaGenerator->GetNoise(x + coordOffsetX, z + coordOffsetZ);

    // 3. Normalizar el valor de ruido [-1, 1] a un ID de celda grande (para usar en el hash).
    int cellID = (int)((cellValue + 1.0f) * 100000.0f);

    return HashBiomeSelection(m_areaDef.seed, cellID, m_biomeDefinitions.size());
}


const float ProceduralBiomaEngine::GetHeight(const float& x, const float& z) const
{
    // --- 1. Obtener Distancias y Biomas (F1, F2, ID_A, ID_B) ---

    // F1: Distancia a la celda más cercana
    m_areaGenerator->SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);
    float F1 = m_areaGenerator->GetNoise(x, z);

    // F2: Distancia a la segunda celda más cercana
    m_areaGenerator->SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance2);
    float F2 = m_areaGenerator->GetNoise(x, z);

    // IDs de los dos biomas más cercanos (A principal, B colindante)
    // Bioma A (F1): Coordenadas estándar
    int idxA = GetBiomeIndex(x, z, 0.0f, 0.0f);

    // Bioma B (F2): Coordenadas perturbadas para obtener un ID diferente.
    // Usamos un offset grande (ej. 1000) para forzar la consulta de un bioma vecino.
    const float OFFSET = 1000.0f;
    int idxB = GetBiomeIndex(x, z, OFFSET, OFFSET);

    const auto& biomeA = m_biomeDefinitions[idxA];
    const auto& biomeB = m_biomeDefinitions[idxB];

    // --- 2. Determinación del Factor de Mezcla (blendT) ---

    float distanceToEdge = F2 - F1;
    float blendT = 0.0f; // 0.0 = Bioma A, 1.0 = Bioma B

    // Si los biomas son iguales (no hay frontera activa) O si estamos afuera de la zona de mezcla.
    if (idxA == idxB || distanceToEdge > m_areaDef.blendDistance) {
        // No hay mezcla.
    }
    else {
        // Estamos en la zona de transición.
        // Normalizar la distancia al borde [0.0, m_areaDef.blendDistance]
        blendT = smoothstep_cpp(0.0f, m_areaDef.blendDistance, distanceToEdge);
    }

    // --- 3. Interpolación de TODOS los Parámetros ---

    // Alturas de transición (clave para evitar acantilados)
    float blendedBaseHeight = std::lerp(biomeA.baseHeightOffset, biomeB.baseHeightOffset, blendT);
    float blendedMinHeight = std::lerp(biomeA.minHeightConstraint, biomeB.minHeightConstraint, blendT);
    float blendedMaxHeight = std::lerp(biomeA.maxHeightConstraint, biomeB.maxHeightConstraint, blendT);

    // Parámetros de Ruido (para que la textura del ruido cambie suavemente)
    float blendedAmplitude = std::lerp(biomeA.noiseAmplitude, biomeB.noiseAmplitude, blendT);
    float blendedFrequency = std::lerp(biomeA.noiseConfig.frequency, biomeB.noiseConfig.frequency, blendT);

    // Nota: Otros parámetros (Octaves, Gain, etc.) también deberían interpolarse
    // si desea una transición completa entre dos tipos de ruido.

    // --- 4. Generación de Altura Final ---

    // Configurar el generador con los parámetros interpolados
    m_detailGenerator->SetNoiseType(biomeA.noiseConfig.noiseType);
    m_detailGenerator->SetFractalType(biomeA.noiseConfig.fractalType);
    m_detailGenerator->SetFrequency(blendedFrequency);

    float noiseValue = m_detailGenerator->GetNoise(x, z);

    // Cálculo final de la altura mezclada
    float finalHeight = (noiseValue * blendedAmplitude) + blendedBaseHeight;

    // --- 5. Aplicar Restricción Final (Cotas) ---
    // Esto asegura que la unión respete la cota más restrictiva de ambos biomas.
    finalHeight = std::clamp(finalHeight, blendedMinHeight, blendedMaxHeight);

    // Aplicar la escala maestra del terreno
    return finalHeight * m_def.terrainHeight;
}
