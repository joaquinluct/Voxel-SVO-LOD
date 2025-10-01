#pragma once

#include <DirectXMath.h>
#include "FrameStateBase.h"

class LightFrameState : public FrameStateBase {
public:
    DirectX::XMFLOAT3 GetLightDirection() const { return lightDirection; }
    DirectX::XMFLOAT4 GetLightColor() const { return lightColor; }
    void SetLightDirection(const DirectX::XMFLOAT3& direction) { lightDirection = direction; }
    void SetLightColor(const DirectX::XMFLOAT4& color) { lightColor = color; }
private:
    DirectX::XMFLOAT3 lightDirection = {};
    DirectX::XMFLOAT4 lightColor = {};
};
