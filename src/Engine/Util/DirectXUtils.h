#pragma once

#include <DirectXMath.h>
#include <cmath> // Para std::fabs

using namespace DirectX;

namespace DirectXUtils
{
	XMFLOAT3 Add(const XMFLOAT3& v1, const XMFLOAT3& v2);
	XMFLOAT3 AddInt(const XMFLOAT3& v1, const int v2[3]);
	bool AreEqual(const XMFLOAT3& v1, const XMFLOAT3& v2);
	XMFLOAT3 Subtract(const XMFLOAT3& a, const XMFLOAT3& b);
	XMFLOAT3 Multiply(const XMFLOAT3& a, float scalar);
	XMFLOAT3 Divide(const XMFLOAT3& a, float scalar);
	XMFLOAT3 Normalize(const XMFLOAT3& v);
	XMFLOAT3 Cross(const XMFLOAT3& v1, const XMFLOAT3& v2);
	XMFLOAT3 Lerp(const XMFLOAT3& v1, const XMFLOAT3& v2, float t);
	XMFLOAT3 Transform(const XMFLOAT3& v, const XMMATRIX& m);
	bool IsNormalPointing(const XMFLOAT3& normal, const XMFLOAT3& direction, float dotThreshold = 0.999f);
	float Dot(const XMFLOAT3& a, const XMFLOAT3& b);
	

	// Constantes de direcciones cardinales (útiles para IsNormalPointing)
	// Asegúrate de que tus ejes XYZ coincidan con estas convenciones.
	// Asumo Y+ = Arriba, Z- = Norte, X+ = Este.
	static const XMFLOAT3 UP_DIR = { 0.0f, 1.0f, 0.0f };
	static const XMFLOAT3 DOWN_DIR = { 0.0f, -1.0f, 0.0f };
	static const XMFLOAT3 NORTH_DIR = { 0.0f, 0.0f, -1.0f }; // Si tu 'z' crece hacia el "sur"
	static const XMFLOAT3 SOUTH_DIR = { 0.0f, 0.0f, 1.0f };  // Si tu 'z' crece hacia el "sur"
	static const XMFLOAT3 EAST_DIR = { 1.0f, 0.0f, 0.0f };
	static const XMFLOAT3 WEST_DIR = { -1.0f, 0.0f, 0.0f };
}