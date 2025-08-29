#include "SceneManager.h"
#include <DeviceManager.h>
#include <AssetLocator/AssetLocator.h>
#include <ConfigLocator/ConfigLocator.h>
#include <ServiceLocator/ServiceLocator.h>
#include <ManagerLocator/ManagerLocator.h>

#include "REGISTER_MANAGER_MACRO.h"

REGISTER_MANAGER_TYPE(SceneManager, "SceneManager")

SceneManager::SceneManager() : m_deviceManager{nullptr} {
	m_gameConfig = new GameEngineConfig();
}

SceneManager::~SceneManager() {

}

HRESULT SceneManager::Init() {

	m_deviceManager = ManagerLocator::GetDeviceManager();
	if (!m_deviceManager) {
		return E_FAIL; // Device manager service not available
	}

	return S_OK;
}