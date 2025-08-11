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

HRESULT Skybox::Init() 
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

	mesh = m_renderManager->GameRenderManagerGet()->RegisterMesh("ProcSkyboxMesh");
	/*mesh = AssetLocator::GetAsset<MeshAsset>("SkyboxMesh");*/

	if (!mesh) {
		return E_FAIL;
	}

	HRESULT hr = mesh->Init();

	if (FAILED(hr)) {
		return hr;
	}

	return S_OK; 
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
	UpdateSunPosition(deltaTime);
	//m_eventTimer += deltaTime;

	//if (m_eventTimer >= m_eventInterval) {
	//	m_eventTimer = 0.0f;
		XMFLOAT3 dir = lighting->GetLightDirection();
	//	dir.x += deltaTime * m_rotationSpeed * sign;
	//	if (dir.x < -100.0f || dir.x > 100.0f) {
	//		sign = !sign; // Toggle the sign
	//	}
	//	dir.y += (m_rotationSpeed * sign ? 1.0f : -1.0f);
	//	if (dir.y < - 100.0f || dir.y > 100.0f) {
	//		sign = !sign; // Toggle the sign
	//	}
		lighting->SetLightDirection(dir);
	//}	
}

void Skybox::Render()
{	
	// Comentar esto para el nuevo sistema de renderizado
	mesh->Render();
}

void Skybox::Shutdown(){}