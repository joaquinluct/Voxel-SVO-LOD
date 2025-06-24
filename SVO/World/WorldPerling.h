#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include "Material/Material.h"

using namespace DirectX;

class WorldPerling
{	
public:
	WorldPerling();
	~WorldPerling();
	HRESULT Init(Material* material);
	void Release();
	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* context);
private:
	Material* m_material;
};