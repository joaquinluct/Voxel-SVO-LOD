// Procedural/Engines/ProceduralBiomeEngine.h

#pragma once
#define NOMINMAX

#include "ProceduralEngineBase.h"
#include <Defines/WorldTerrain.h>
#include <FastNoiseLite/FastNoiseLite.h> // Usaremos FNL para la capa de ruido Perlin
#include <map>
#include <stdexcept>
#include <vector>

// -------------------------------------------------------------
// Estructura para las Semillas de Voronoi y los Parámetros del Bioma
// -------------------------------------------------------------
struct BiomeSeed {
    int id;
    float x; // Coordenada X de la semilla de Voronoi
    float z; // Coordenada Z de la semilla de Voronoi
    float area_weight; // Peso para influir en el tamaño del bioma (simula Min/Max Chunks)
};

class ProceduralBiomeEngine : public ProceduralEngineBase {
private:
    FastNoiseLite* m_noise_modulator; // Ruido Perlin para suavizar las fronteras
    WorldTerrain::TerrainProceduralDefinition m_def;

    std::vector<BiomeSeed> m_biome_seeds;

    // Parámetros de generación
    const float m_voronoi_scale = 500.0f; // Escala base para las regiones de bioma
    const float m_perlin_scale = 75.0f;   // Escala para el ruido de detalle en las fronteras
    int m_num_biomes = 0;

    // --- Funciones Internas de Ayuda ---

    // Función central para el cálculo de Voronoi/Distancia ponderada
    [[nodiscard]] int calculate_voronoi_biome(float x, float z) const;

public:
    ~ProceduralBiomeEngine() override {
        delete m_noise_modulator;
    }

    // Inicialización: recibe la semilla maestra.
    HRESULT Init(const int& seed, const WorldTerrain::TerrainProceduralDefinition& def) override;

    // Devuelve el ID del bioma (convertido a float) para la coordenada (x, z)
    [[nodiscard]] const float GetHeight(const float& x, const float& z) const override;
};
