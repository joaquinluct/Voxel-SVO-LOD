// VoxelBasicTextureConfig.h
#pragma once

#include <DirectXMath.h> // Para MatrixBufferType si es de DirectXMath
#include "ITextureConfig.h"
#include "Utils.h"


class VoxelBasicTextureConfig : public ITextureConfig{
public:
    HRESULT Init(
        ID3D11Device* device,
        ShaderManager* shaderManger,
        ID3D11SamplerState** outSamplerState,
        ID3D11Buffer** outMatrixBuffer
    ) override; // Usar 'override' para asegurar que se implementa la función base.
};
