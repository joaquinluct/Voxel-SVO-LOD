#pragma once

#include <string>
#include <vector>
#include <Defines/VertexDefinition.h>
#include "IAssetVertexConfigBase.h"

template<typename T>
class IAssetVertexConfig //: public IAssetVertexConfigBase<T>
{
private:
	std::vector<T> m_vertext;
	std::vector<uint16_t> m_index;

public:	
	virtual const std::string& GetAssetName() = 0;
	std::vector<T> GetVertex() {
		return m_vertext;
	}
	std::vector<uint16_t> GetIndex() {
		return m_index;
	}	
};
