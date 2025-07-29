#include "Lighting.h"
#include <ConfigLocator/ConfigLocator.h>
#include <Util/Utils.h>
#include <REGISTER_SERVICE_MACRO.h>

REGISTER_SERVICE_TYPE(Lighting, "Lighting")

Lighting::Lighting()
{
}

Lighting::~Lighting()
{
}

HRESULT Lighting::Init()
{
	config = ConfigLocator::GetConfig<LightingConfig>();
	matrixParams = std::make_unique<MatrixDefinitionBase::MatrixParams>();

	return S_OK; // Return success
}

void Lighting::Update(float deltaTime)
{
	// Update logic for the Lighting manager
}

void Lighting::Render()
{	
}

void Lighting::SetLightDirection(const DirectX::XMFLOAT3& direction)
{
	if (matrixParams) {
		matrixParams->lightDirection = direction;
	}
}
void Lighting::SetLightColor(const DirectX::XMFLOAT4& color)
{
	if (matrixParams) {
		matrixParams->lightColor = color;
	}
}
DirectX::XMFLOAT3 Lighting::GetLightDirection() const
{
	if (matrixParams) {
		return matrixParams->lightDirection;
	}
	return DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}
DirectX::XMFLOAT4 Lighting::GetLightColor() const
{
	if (matrixParams) {
		return matrixParams->lightColor;
	}
	return DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
}

void Lighting::Shutdown(){}