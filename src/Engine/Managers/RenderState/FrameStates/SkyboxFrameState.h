#pragma once

#include <DirectXMath.h>
#include "FrameStateBase.h"

class SkyboxFrameState : public FrameStateBase {
public:
	DirectX::XMFLOAT4 GetSkyColor() const { return skyColor; }
	DirectX::XMFLOAT4 GetSunColor() const { return sunColor; }

	void SetSkyColor(const DirectX::XMFLOAT4& color) { skyColor = color; }
	void SetSunColor(const DirectX::XMFLOAT4& color) { sunColor = color; }

private:
	DirectX::XMFLOAT4 skyColor;
	DirectX::XMFLOAT4 sunColor;
};


