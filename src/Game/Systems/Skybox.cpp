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
	m_eventTimer(0.0f), m_eventInterval(3.0f), sign(false), m_rotationSpeed(1.0f)
{
}

Skybox::~Skybox()
{
}

HRESULT Skybox::Init() 
{
	/*m_renderManager = ManagerLocator::GetManager<RenderManager>();
	if (!m_renderManager) {
		return E_FAIL;
	}*/

	lighting = ServiceLocator::GetService<Lighting>();
	if (!lighting) {
		return E_FAIL;
	}

	//mesh = m_renderManager->GameRenderManagerGet()->RegisterMesh("SkyboxMesh");
	mesh = AssetLocator::GetAsset<MeshAsset>("SkyboxMesh");

	if (!mesh) {
		return E_FAIL;
	}

	HRESULT hr = mesh->Init();

	if (FAILED(hr)) {
		return hr;
	}

	return S_OK; 
}

void Skybox::Update(float deltaTime)
{
	m_eventTimer += deltaTime;

	if (m_eventTimer >= m_eventInterval) {
		m_eventTimer = 0.0f;
		XMFLOAT3 dir = lighting->GetLightDirection();
		dir.y = (m_rotationSpeed * sign ? 1.0f : -1.0f);
		if (dir.y < - 100.0f || dir.y > 100.0f) {
			sign = !sign; // Toggle the sign
		}
		lighting->SetLightDirection(dir);
	}
	// Update logic for the skybox manager
}

void Skybox::Render()
{	
	// Comentar esto para el nuevo sistema de renderizado
	mesh->Render();
}

void Skybox::Shutdown(){}