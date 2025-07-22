// IAsset.h
#pragma once
#include "IAsset.h"
#include <string>
#include <vector>
#include <Defines/VertexDefinition.h>

// Interfaz base para todos los assets que serán gestionados por el AssetManager
template<typename T>
class IAssetMesh : public IAsset {
public:
    virtual ~IAssetMesh() = default;
	HRESULT Init() override { return S_OK; };
	void Shutdown() override {};
	void Render() override {};
	void Update(float deltaTime) override {};
	void Load() override {};
	void Unload() override {};
    //virtual void Shutdown() = 0;
    virtual std::vector<T> GetVertex() = 0;
	virtual std::vector<uint16_t> GetIndex() = 0;
};