#include "Water.h"
#include <AssetLocator/AssetLocator.h>
#include <ServiceLocator/ServiceLocator.h>
#include <ManagerLocator/ManagerLocator.h>
#include <ConfigLocator/ConfigLocator.h>
#include <Util/Utils.h>
#include <Util/DirectXUtils.h>
#include <Util/DateTime.h>
#include <RenderManager/RenderManager.h>
#include <REGISTER_SERVICE_MACRO.h>
#include <unordered_set>
#include <Util/RayTracing/RayTracing.h>

REGISTER_SERVICE_TYPE(Water, "Water")

Water::Water()
	: m_cameraManager{}, m_oceanHeight{ 0 }
{}

Water::~Water()
{
}

HRESULT Water::InitServices()
{
	m_cameraManager = ManagerLocator::GetManager<CameraManager>();
	if (!m_cameraManager) {
		OutputDebugStringA("Water: Register camera manager fail.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT Water::InitConfig()
{	
	m_config = ConfigLocator::GetConfig<WaterConfig>();

	if (m_config == nullptr) {
		return E_FAIL;
	}

	m_oceanHeight = m_config->waterLevel;

	return S_OK;
}

HRESULT Water::PostInit()
{
	std::shared_ptr<RenderManager> m_renderManager = ManagerLocator::GetManager<RenderManager>();
	/*m_waterMesh = m_renderManager->SceneManagerGet()->RegisterMesh("OceanMesh");
	if (!m_waterMesh) {
		OutputDebugStringA("Water: Register water mesh fail.\n");
		return E_FAIL;
	}
	m_waterMesh->Init();*/
	return S_OK;
}

HRESULT Water::Init()
{	
	HRESULT hr = InitServices();
	if (FAILED(hr)) {
		OutputDebugStringA("Water: Init services fail.\n");
		return hr;
	}
		
	hr = InitConfig();
	if (FAILED(hr)) {
		OutputDebugStringA("Water: Init date and time fail.\n");
		return hr;
	}

	return hr;
}

void Water::Update(float deltaTime)
{
	XMFLOAT3 cameraPos = m_cameraManager->GetCurrentCameraPosition();

	const int gridSize = 5;
	const float tileSize = 200.0f;
	const float halfGrid = (gridSize - 1) * tileSize * 0.5f;

	float baseX = floor(cameraPos.x / tileSize) * tileSize;
	float baseZ = floor(cameraPos.z / tileSize) * tileSize;

	m_matrices.clear();

	for (int z = 0; z < gridSize; ++z) {
		for (int x = 0; x < gridSize; ++x) {
			float worldX = baseX + x * tileSize - halfGrid;
			float worldZ = baseZ + z * tileSize - halfGrid;

			DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(worldX, m_oceanHeight, worldZ);
			m_matrices.push_back(worldMatrix);
		}
	}
}

void Water::Render()
{
}

float Water::GetWaterHeight() const
{
	return m_oceanHeight;
}