#pragma once

#include <DirectXMath.h>

namespace CameraDefinition
{
	struct FrustumPlane
	{
		DirectX::XMFLOAT4 coefficients; // Ax + By + Cz + D = 0
	};
}
