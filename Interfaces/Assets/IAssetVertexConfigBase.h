#pragma once

#include <string>
#include <vector>
#include <Defines/VertexDefinition.h>

template<typename T>
class IAssetVertexConfigBase
{
public:
	virtual std::vector<uint16_t> GetIndex() = 0;
	virtual std::vector<T> GetVertex() = 0;
	virtual const std::string& GetAssetName() = 0;
};
