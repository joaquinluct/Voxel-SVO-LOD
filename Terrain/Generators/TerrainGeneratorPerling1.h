#pragma once

#include <DirectXMath.h>
//#include "../../Libs/FastNoise2/include/FastNoise/FastNoise.h" 
#include "DirectXUtils.h"
#include <winerror.h>
#include "FastNoiseLite/FastNoiseLite.h"

class TerrainGeneratorPerling1
{
public:
	TerrainGeneratorPerling1();
	~TerrainGeneratorPerling1();
	HRESULT Init(const char* seed);
	void PrepareNoiseOutput(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& size);
    //float GetTerrainDensity(const XMFLOAT3& pos);
	//std::vector<float> ProceduralTerrainSDF(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& size);
	float GetRawHeight(float x, float z) const;
	//FastNoise::SmartNode<> m_terrainGenerator;
	FastNoiseLite* m_terrainGeneratorLite;

private:
	DirectX::XMFLOAT3 m_terrainPosition;
	DirectX::XMFLOAT3 m_terrainSize;
	const char* m_terrainSeed;
	//std::vector<float> m_noiseOutput;
};
