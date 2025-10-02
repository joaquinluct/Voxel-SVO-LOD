#pragma once
#include <ConfigBase.h>
#include <string>
#include <vector>

// Usamos 'unsigned int' para los enums (BiomeType, BiomeDomain, BiomeGoegraphycalType)
// ya que es común para la serialización/deserialización desde archivos.

struct BiomeBaseConfig : public ConfigBase
{
    virtual ~BiomeBaseConfig() = default;

    // ------------------------------------------------------------------
    // PROPIEDADES IDENTIFICATIVAS Y DE TIPO (del YAML)
    // ------------------------------------------------------------------
    int id = 0;                // ID único del bioma (ej. 1, 2, 3...)
    std::string name = "None"; // Nombre del bioma (ej. "Plains")
    unsigned int type = 0;     // BiomeType (0=None, 1=Procedural, 2=Fixed)
    unsigned int domain = 0;   // BiomeDomain (0=Terrestre, 1=Acuático)

    // ------------------------------------------------------------------
    // CARACTERÍSTICAS (BOOLS)
    // ------------------------------------------------------------------
    bool canHaveBeaches = false;
    bool canHaveLakes = false;
    bool canHaveMountains = false;
    bool canHaveRivers = false;
    bool canHaveRoads = false;
    bool canHaveTrees = false;

    // ------------------------------------------------------------------
    // RANGO DE APARICIÓN (ALTURA)
    // ------------------------------------------------------------------
    float minHeight = 0.0f; // Altura mínima de aparición (relativa al rango del mundo)
    float maxHeight = 0.1f; // Altura máxima de aparición

    // ------------------------------------------------------------------
    // PUNTOS DE TRANSICIÓN VERTICAL (para el Shader)
    // ------------------------------------------------------------------
    // Definimos los límites superiores de la Zona 1 y 2, 
    // como proporción del rango total (maxHeight - minHeight)
    float zone1_max_ratio = 0.125f; // Límite Zona 1 -> Zona 2
    float zone2_max_ratio = 0.625f; // Límite Zona 2 -> Zona 3

    // ------------------------------------------------------------------
    // RESTRICCIONES DE VECINDAD (para el Algoritmo Procedural)
    // ------------------------------------------------------------------
    std::vector<std::string> neverNearOf{}; // Lista de biomas a evitar
    std::vector<std::string> alwaysNearOf{}; // Lista de biomas preferidos como vecinos

    // ------------------------------------------------------------------
    // TEXTURAS (3 ZONAS x 3 TEXTURAS) y TIPO GEOGRÁFICO
    // ------------------------------------------------------------------
    // ZONA 1 (Coastal / Poca Profundidad)
    std::vector<std::string> textures_zone1{};
    unsigned int zone1_type = 0; // BiomeGoegraphycalType (ej. Beach, Coast)

    // ZONA 2 (Mid-Range / Medio Fondo)
    std::vector<std::string> textures_zone2{};
    unsigned int zone2_type = 0; // BiomeGoegraphycalType (ej. Plain, Meadow)

    // ZONA 3 (Mountain / Fondo Abisal)
    std::vector<std::string> textures_zone3{};
    unsigned int zone3_type = 0; // BiomeGoegraphycalType (ej. Hill, Abyssal)
};
