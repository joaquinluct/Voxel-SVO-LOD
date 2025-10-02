// Procedural/Engines/ProceduralBiomeEngine.cpp

#include "ProceduralBiomeEngine.h"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <Defines/WorldTerrain.h>
#include <FastNoiseLite/FastNoiseLite.h>
#include <random> // Para la generación determinista de semillas
#include <stdexcept>

// ====================================================================
// A. INICIALIZACIÓN
// ====================================================================

HRESULT ProceduralBiomeEngine::Init(const int& seed, const WorldTerrain::TerrainProceduralDefinition& def) {
    m_noise_modulator = new FastNoiseLite();
    m_def = def;

    if (!m_noise_modulator) {
        return E_FAIL;
    }

    // Usar la SEMILLA MAESTRA para la generación de las semillas de Voronoi
    // Esto asegura que la ubicación y el peso de las regiones de bioma nunca cambien.
    m_num_biomes = 10; // Suponemos 10 biomas activos para el ejemplo
    std::mt19937 generator(seed);

    // Rango de posiciones de semillas (depende del tamaño del mundo)
    std::uniform_real_distribution<float> dist_pos(-m_def.worldWidth / 2.0f, m_def.worldWidth / 2.0f);

    // El peso (simula MinChunks/MaxChunks) también es determinista.
    std::uniform_real_distribution<float> dist_weight(0.5f, 1.5f);

    for (int i = 1; i <= m_num_biomes; ++i) {
        m_biome_seeds.push_back({
            i,
            dist_pos(generator),
            dist_pos(generator),
            dist_weight(generator)
            });
    }

    // Configurar el Ruido Perlin para la modulación de fronteras
    // Se usa un desplazamiento de la SEMILLA MAESTRA para un patrón diferente pero determinista.
    m_noise_modulator->SetSeed(seed + 1);
    m_noise_modulator->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    m_noise_modulator->SetFrequency(1.0f / m_perlin_scale);
    m_noise_modulator->SetFractalType(FastNoiseLite::FractalType_FBm);
    m_noise_modulator->SetFractalOctaves(4);

    return S_OK;
}

// ====================================================================
// B. FUNCIÓN VORONOI PONDERADA (Paso 1: Estructura de Bioma)
// ====================================================================

[[nodiscard]] int ProceduralBiomeEngine::calculate_voronoi_biome(float x, float z) const {
    if (m_biome_seeds.empty()) return 0;

    int closest_biome_id = 0;
    float min_weighted_distance = std::numeric_limits<float>::max();

    // Encontrar la semilla de bioma más cercana usando la distancia ponderada.
    for (const auto& seed : m_biome_seeds) {
        float dx = x - seed.x;
        float dz = z - seed.z;
        float distance_sq = (dx * dx) + (dz * dz);

        // Distancia ponderada: la distancia efectiva se reduce por el 'area_weight',
        // haciendo que biomas con mayor peso dominen más área.
        float weighted_distance = std::sqrt(distance_sq) / seed.area_weight;

        if (weighted_distance < min_weighted_distance) {
            min_weighted_distance = weighted_distance;
            closest_biome_id = seed.id;
        }
    }

    return closest_biome_id;
}


// ====================================================================
// C. FUNCIÓN CENTRAL (Devuelve el ID de Bioma)
// ====================================================================

const float ProceduralBiomeEngine::GetHeight(const float& x, const float& z) const {
    if (m_num_biomes == 0) return 0.0f; // ID 0 como bioma por defecto o error

    // Paso 1: Determinar el Bioma Base (Estructura)
    int base_biome_id = calculate_voronoi_biome(x, z);

    // Paso 2: Encontrar el 2do Bioma más cercano para la transición
    int second_closest_biome_id = 0;
    float min_dist_base = std::numeric_limits<float>::max();
    float min_dist_second = std::numeric_limits<float>::max();

    // Recorrer de nuevo para encontrar las dos distancias mínimas (necesario para el límite)
    for (const auto& seed : m_biome_seeds) {
        float dx = x - seed.x;
        float dz = z - seed.z;
        float weighted_distance = std::sqrt((dx * dx) + (dz * dz)) / seed.area_weight;

        if (seed.id == base_biome_id) {
            min_dist_base = weighted_distance;
            continue;
        }

        if (weighted_distance < min_dist_second) {
            min_dist_second = weighted_distance;
            second_closest_biome_id = seed.id;
        }
    }

    // Paso 3: Modulación Perlin (Suavizado de la Frontera)

    // Mide qué tan cerca está la coordenada del punto de equilibrio (0.5) entre los biomas
    float closeness_to_border = 0.5f - (min_dist_base / (min_dist_base + min_dist_second));

    // Ruido Perlin modulador (normalizado de [0, 1])
    float mod_noise = (m_noise_modulator->GetNoise(x, z) + 1.0f) * 0.5f;

    // Umbral de cambio: Si el punto está cerca del borde (ej. abs < 0.15) Y 
    // el ruido empuja la frontera (ej. mod_noise > 0.6), cambiamos al bioma vecino.
    if (std::abs(closeness_to_border) < 0.15f) {
        if (mod_noise > 0.6f && second_closest_biome_id != 0) {
            return static_cast<float>(second_closest_biome_id);
        }
    }

    // Devolvemos el ID del bioma base (la decisión principal de Voronoi)
    return static_cast<float>(base_biome_id);
}
