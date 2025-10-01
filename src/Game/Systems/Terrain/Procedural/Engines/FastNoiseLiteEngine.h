#pragma once
#define NOMINMAX // Añadido para evitar el conflicto con las macros de min y max de Windows

#include "FastNoiseLite/FastNoiseLite.h"
#include "ProceduralEngineBase.h"
#include <Defines/WorldTerrain.h>
#include <string>
#include <windows.h>

class FastNoiseLiteEngine : public ProceduralEngineBase {
private:
    FastNoiseLite* m_generator;
    FastNoiseLite* m_oceanFloorGenerator; // Nuevo generador para el fondo oceánico
    WorldTerrain::TerrainProceduralDefinition m_def;

    void GeneratePlain(const int& seed);
    void GenerateValleys(const int& seed);
    void GenerateMountains(const int& seed);

public:
    ~FastNoiseLiteEngine() override {
        delete m_generator;
        delete m_oceanFloorGenerator;
    }

    HRESULT Init(const int& seed, const WorldTerrain::TerrainProceduralDefinition& def) override;
    const float GetHeight(const float& x, const float& z) const override;
};
