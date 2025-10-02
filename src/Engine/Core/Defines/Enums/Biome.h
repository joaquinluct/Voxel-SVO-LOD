#pragma once

enum class BiomeType : unsigned int {
    None = 0,
    Procedural = 1,
    Fixed = 2,
};

// El Dominio del Bioma (Tierra o Agua)
enum class BiomeDomain : unsigned char {
    Land = 0,
    Water = 1,
    Count = 2
};

enum class BiomeZone : unsigned char {
    Coastal = 0,         // Costa / Poca profundidad
    Mid = 1,             // Media altura / Medio fondo
    Mountain_Abyssal = 2, // Montaña / Fondo abisal
    Count = 3            // Número total de zonas
};

// Las 3 Texturas dentro de cada Zona (para mezcla de detalle)
enum class ZoneTexture : unsigned char {
    T1 = 0,
    T2 = 1,
    T3 = 2,
    Count = 3            // Número total de texturas por zona
};

enum class BiomeGoegraphycalType : unsigned int {
    None = 0,
    Island = 1,
    Plain,
    Meadow,
    Mud,
    Coast = 5,
    Beach,
    Lake,
    River,
    Hill,
    MountainRange = 10,
    Desert,
    Forest,
    Tundra,
    Swamp,
    Count,
};

enum class BiomeZoneProperties : unsigned int {
    None = 0,
    Beach = 1 << 0,     // 1
    Lake = 1 << 1,      // 2
    Mountain = 1 << 2,  // 4
    River = 1 << 3,     // 8
    Roads = 1 << 4,     // 16
    Trees = 1 << 5,     // 32
    Bush = 1 << 6,      // 64
    Grass = 1 << 7,     // 128
    Fauna1 = 1 << 8,    // 256
    Fauna2 = 1 << 9,    // 512
    Fauna3 = 1 << 10,   // 1024
};
