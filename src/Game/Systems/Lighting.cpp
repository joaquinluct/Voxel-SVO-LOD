#include "Lighting.h"
#include <ManagerLocator/ManagerLocator.h>
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
	matrixParams = std::make_unique<MatrixDefinitionBase::LightMatrixParams>();

	m_keyboard = ManagerLocator::GetKeyboardManager();

	std::vector<float> lightDirection = config->directional_light;

	if (lightDirection.size() == 3) {
		matrixParams->lightDirection = DirectX::XMFLOAT3(lightDirection[0], lightDirection[1], lightDirection[2]);
	} else {
		matrixParams->lightDirection = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f); // Default direction if not set
	}

	std::vector<float> lightColor = config->directional_light_color;

	if (lightColor.size() == 4) {
		matrixParams->lightColor = DirectX::XMFLOAT4(lightColor[0], lightColor[1], lightColor[2], lightColor[3]);
	} else {
		matrixParams->lightColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); // Default color if not set
	}

	return S_OK; // Return success
}

void Lighting::Update(float deltaTime)
{
	float speed = 0.5f;
	XMFLOAT3 lightDirection = matrixParams->lightDirection;
	// Update logic for the Lighting manager
	if (m_keyboard->IsKeyDown('T')) {
		lightDirection.z += speed * deltaTime; // Move light up
	}
	if (m_keyboard->IsKeyDown('G')) {
		lightDirection.z -= speed * deltaTime; // Move light up
	}
	if (m_keyboard->IsKeyDown('F')) {
		lightDirection.x += speed * deltaTime; // Move light up
	}
	if (m_keyboard->IsKeyDown('H')) {
		lightDirection.x -= speed * deltaTime; // Move light up
	}
	if (m_keyboard->IsKeyDown('I')) {
		lightDirection.y += speed * deltaTime; // Move light up
	}
	if (m_keyboard->IsKeyDown('K')) {
		lightDirection.y -= speed * deltaTime; // Move light up
	}
	matrixParams->lightDirection = lightDirection;
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