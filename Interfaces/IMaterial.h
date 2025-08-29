#pragma once
#include <d3d11.h>

class IMaterial {
public:
	virtual ~IMaterial() = default;
    virtual void SetTexture(ID3D11ShaderResourceView* texture, std::string textureType = "") = 0;    
	virtual void Shutdown() = 0;
};