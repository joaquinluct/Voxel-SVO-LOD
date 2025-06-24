// ITextureConfig.h
#pragma once

#include <d3d11.h>
#include "ShaderManager.h"

class ITextureConfig {
public:
    virtual ~ITextureConfig() = default;
    virtual HRESULT Init(ID3D11Device* device, ShaderManager* shaderManger, ID3D11SamplerState** ppSamplerState, ID3D11Buffer** ppMatrixBuffer) = 0;
};