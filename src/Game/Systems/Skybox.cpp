#include "Skybox.h"
#include <AssetLocator/AssetLocator.h>
#include <ManagerLocator/ManagerLocator.h>
#include <Defines/VertexDefinition.h>
#include <Util/Utils.h>
#include <REGISTER_SERVICE_MACRO.h>

REGISTER_SERVICE_TYPE(Skybox, "Skybox")

Skybox::Skybox()
	: mesh{}, 
	lighting{ nullptr },
	m_renderManager{ nullptr },
	m_eventTimer(0.0f), m_eventInterval(1.0f), sign(false), m_rotationSpeed(1.0f)
{
}

Skybox::~Skybox()
{
}

HRESULT Skybox::InitConfig()
{
	std::vector<float> sunColor = m_config.sky_color;
	m_sunColor = XMFLOAT4(sunColor[0], sunColor[1], sunColor[2], sunColor[3]);
	std::vector<float> skyColor = m_config.sky_color;
	m_skyColor = XMFLOAT4(skyColor[0], skyColor[1], skyColor[2], skyColor[3]);

	return S_OK;
}

HRESULT Skybox::InitManagers()
{
	m_renderManager = ManagerLocator::GetManager<RenderManager>();
	if (!m_renderManager) {
		return E_FAIL;
	}
	m_cameraManager = ManagerLocator::GetManager<CameraManager>();
	if (!m_cameraManager) {
		return E_FAIL;
	}

	lighting = ServiceLocator::GetService<Lighting>();
	if (!lighting) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT Skybox::InitMesh()
{
	/*mesh = m_renderManager->SceneManagerGet()->RegisterMesh("ProcSkyboxMesh");

	if (!mesh) {
		return E_FAIL;
	}

	return mesh->Init();*/
	return S_OK;
}

HRESULT Skybox::Init() 
{
	HRESULT hr = InitConfig();
	if (FAILED(hr)) {
		return hr;
	}

	hr = InitManagers();
	if (FAILED(hr)) {
		return hr;
	}

	hr = InitMesh();
	if (FAILED(hr)) {
		return hr;
	}

	return hr; 
}

DirectX::XMMATRIX Skybox::GetRotationMatrix() const
{
	// Obtener la dirección de la luz desde el sistema de iluminación
	XMFLOAT3 lightDirection = lighting->GetLightDirection();

	// Normalizar la dirección de la luz (si no lo está ya)
	DirectX::XMVECTOR LightDir = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&lightDirection));

	// Vector de rotación: el eje alrededor del que hay que rotar
	DirectX::XMVECTOR rotationAxis = DirectX::XMVector3Cross(skyboxSunDirection, LightDir);

	// Ángulo de rotación
	float rotationAngle = DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenVectors(skyboxSunDirection, LightDir));

	// Crea la matriz de rotación
	DirectX::XMMATRIX skyboxRotation = DirectX::XMMatrixRotationAxis(rotationAxis, rotationAngle);

	return skyboxRotation;
}

void Skybox::UpdateSunPosition(float deltaTime)
{
	DirectX::XMMATRIX skyboxRotation = GetRotationMatrix();
	DirectX::XMMATRIX viewMatrix = m_cameraManager->GetCurrentViewMatrix();

	viewMatrix.r[3].m128_f32[0] = 0.0f;
	viewMatrix.r[3].m128_f32[1] = 0.0f;
	viewMatrix.r[3].m128_f32[2] = 0.0f;

	DirectX::XMMATRIX skyboxWorldViewProjection = skyboxRotation * viewMatrix;

	mesh->SetWorldMatrix(skyboxWorldViewProjection);
}

void Skybox::Update(float deltaTime)
{
	
}

void Skybox::Render()
{	
	// Comentar esto para el nuevo sistema de renderizado
	mesh->Render();
}

XMFLOAT4 Skybox::GetSunColor() const
{
	return m_sunColor;
}
XMFLOAT4 Skybox::GetSkyColor() const
{
	return m_skyColor;
}

void Skybox::Shutdown(){}