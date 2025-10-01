#pragma once
#define NOMINMAX

#include "ProceduralEngineBase.h"
#include <Defines/Structs/Procedural/AreaMapping.h>
#include <Defines/WorldTerrain.h>
#include <FastNoiseLite/FastNoiseLite.h>
#include <vector>
#include <Windows.h>

class ProceduralBiomaEngine : public ProceduralEngineBase {

private:
    FastNoiseLite* m_areaGenerator;     // Ruido Celular para Biomas
    FastNoiseLite* m_detailGenerator;   // Ruido de Detalle (reconfigurado dinámicamente)

    WorldTerrain::TerrainProceduralDefinition m_def;
    AreaMappingDefinition m_areaDef;

    std::vector<AreaTypeDefinition> m_biomeDefinitions;

    float smoothstep_cpp(float edge0, float edge1, float x) const;

    // Helper para obtener el índice de bioma usando CellValue y un offset de coordenadas.
    int GetBiomeIndex(const float& x, const float& z, const float& coordOffsetX, const float& coordOffsetZ) const;

public:
    ProceduralBiomaEngine();
    ~ProceduralBiomaEngine() override;
    HRESULT Init(const int& seed, const WorldTerrain::TerrainProceduralDefinition& def) override;
    const float GetHeight(const float& x, const float& z) const override;
};
