#pragma once
#include <d3d11.h>

class iMaterial {
public:
    virtual void SetTexture(ID3D11ShaderResourceView* texture) = 0;
    virtual void Apply(ID3D11DeviceContext* context) = 0;
};