#include "TerrainGeneratorPerling1.h"

TerrainGeneratorPerling1::TerrainGeneratorPerling1() :  m_terrainGeneratorLite(nullptr), m_terrainSeed(""), m_terrainSize{0.0f, 0.0f, 0.0f}
{
}
TerrainGeneratorPerling1::~TerrainGeneratorPerling1()
{
    if (m_terrainGeneratorLite) {
        m_terrainGeneratorLite = nullptr;
	}
    /*if (m_terrainGenerator) {
        m_terrainGenerator = nullptr;
    }*/
	// Limpiar el generador de ruido
	//m_noiseOutput.clear();
    
}

HRESULT TerrainGeneratorPerling1::Init(const char* seed) {
    m_terrainSeed = seed;

    // Configura tu generador de ruido
    //if (m_terrainGenerator) delete m_terrainGenerator.reset(); // Limpiar si ya exist�a
    //m_terrainGenerator = FastNoise::NewFromEncodedNodeTree(m_terrainSeed);
 //   if (!m_terrainGenerator) {
 //       return E_FAIL; // Error al crear el generador de ruido
	//}
    m_terrainGeneratorLite = new FastNoiseLite();
    if (!m_terrainGeneratorLite) {
        return E_FAIL; // Error al crear el generador de ruido
	}
	return S_OK; // Inicializaci�n exitosa
}

void TerrainGeneratorPerling1::PrepareNoiseOutput(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& size) {	

    m_terrainGeneratorLite->SetSeed(12343); // Semilla para un mapa de altura reproducible
    m_terrainGeneratorLite->SetNoiseType(FastNoiseLite::NoiseType_Perlin); // Puedes probar Simplex, OpenSimplex2
    m_terrainGeneratorLite->SetFrequency(0.0005f); // Frecuencia principal para grandes características (colinas, valles)
    m_terrainGeneratorLite->SetFractalType(FastNoiseLite::FractalType_FBm); // Usar ruido fractal (FBM)
    m_terrainGeneratorLite->SetFractalOctaves(8); // Número de capas de ruido para añadir detalle
    m_terrainGeneratorLite->SetFractalLacunarity(2.0f); // Cuánto más densa se vuelve cada octava (frecuencia * 2)
    m_terrainGeneratorLite->SetFractalGain(0.5f); // Cuánto disminuye la contribución de cada octava (amplitud * 0.5)

	/*m_terrainPosition = pos;
    m_terrainSize = size;
    std::vector<float> noiseOutput(size.x * size.y * size.z);
	m_noiseOutput = std::move(noiseOutput);
	ProceduralTerrainSDF(pos, size);*/
}

//std::vector<float> TerrainGeneratorPerling1::ProceduralTerrainSDF(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& size) {
//	//m_terrainGenerator->GenUniformGrid2D(m_noiseOutput.data(), pos.x, pos.z, size.x, size.z, 0.02f, 1337);
//    m_terrainGenerator->GenUniformGrid3D(m_noiseOutput.data(), pos.x, pos.y, pos.z, size.x, size.y,size.z, 0.2f, 1337);
//	/*m_terrainGenerator->GenSingle2D()
//	return m_noiseOutput;*/
//    return m_noiseOutput;
//}

float TerrainGeneratorPerling1::GetRawHeight(float x, float z) const
{
    // GetNoise suele devolver un valor entre -1.0 y 1.0.
    return m_terrainGeneratorLite->GetNoise(x, z);
}

//float TerrainGeneratorPerling1::GetTerrainDensity(const XMFLOAT3& pos)
//{    
//    // Calcular el índice 1D en el array noiseOutput.        
//	/*XMFLOAT3 posOffset = { pos.x - m_terrainSize.x, pos.y - m_terrainSize.y, pos.z - m_terrainSize.z };
//    int index = posOffset.x + (posOffset.y * m_terrainSize.x) + (posOffset.z * m_terrainSize.x * m_terrainSize.y);*/
//    const float x = pos.x - m_terrainPosition.x;
//    const float z = pos.z - m_terrainPosition.z;
//	int index = static_cast<int>(x + (z * m_terrainSize.x));
//
//    if (index >= 0 && index < static_cast<int>(m_noiseOutput.size()))
//	{   
//        return m_noiseOutput[index];
//        /*float value = m_noiseOutput[index];
//        float remappedNoiseValue = (value + 1.0f) * 0.5f;
//        return remappedNoiseValue;*/
//    }
//    return .0f;
//}
