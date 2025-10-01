#pragma once
#define NOMINMAX // Para evitar el conflicto con las macros de min y max de Windows

#include "FastNoiseLite/FastNoiseLite.h"
#include "ProceduralEngineBase.h"
#include <Defines/WorldTerrain.h>
#include <string>
#include <windows.h>

// Clase concreta que implementa la generación de terreno
// usando dos capas de ruido (Biomas/Planicie + Altura Detallada).
class DoubleLayerNoiseEngine : public ProceduralEngineBase {
private:
    // Capa A: Define regiones amplias de planicie o montaña (Baja Frecuencia).
    FastNoiseLite* m_baseNoiseGenerator;

    // Capa B: Define el detalle geográfico y la altura (Alta Frecuencia).
    FastNoiseLite* m_detailNoiseGenerator;

    // Generador para el fondo oceánico (mantenido del código original).
    FastNoiseLite* m_oceanFloorGenerator;

    WorldTerrain::TerrainProceduralDefinition m_def;

    // Constantes de configuración (Ajustables para modificar las 'manchas' planas)
    const float BASE_FREQ = 0.0005f;      // Frecuencia muy baja para las regiones (ej. 1/2000 unidades)
    const float DETAIL_FREQ = 0.01f;      // Frecuencia más alta para el detalle (ej. 1/100 unidades)
    const float FLATNESS_THRESHOLD = 0.6f; // Umbral de ruido (en rango [0,1]) para empezar la transición a planicie.

    // Función auxiliar de smoothstep (implementada en el .cpp)
    float smoothstep_cpp(float edge0, float edge1, float x) const;

public:
    ~DoubleLayerNoiseEngine() override {
        delete m_baseNoiseGenerator;
        delete m_detailNoiseGenerator;
        delete m_oceanFloorGenerator;
    }

    // Inicializa ambos generadores con la semilla y configura sus parámetros.
    HRESULT Init(const int& seed, const WorldTerrain::TerrainProceduralDefinition& def) override;

    // Implementación de la lógica de doble capa de ruido.
    const float GetHeight(const float& x, const float& z) const override;
};
