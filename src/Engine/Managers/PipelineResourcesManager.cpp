#include "PipelineResourcesManager.h"
#include <Defines/EngineDefinition.h>
#include <ManagerLocator/ManagerLocator.h>
#include <Managers/DeviceManager.h>
#include <Managers/ManagerBase.h>
#include <Managers/ShaderManager.h>
#include <REGISTER_MANAGER_MACRO.h>
#include <string>
#include <Util/Text/Text.h>
#include <Windows.h>

REGISTER_MANAGER_TYPE(PipelineResourcesManager, "PipelineResourcesManager")

PipelineResourcesManager::PipelineResourcesManager() : m_deviceManager(nullptr) {
}

PipelineResourcesManager::~PipelineResourcesManager()
{
    Shutdown();
}

HRESULT PipelineResourcesManager::Init(EngineContext* context)
{
    ManagerBase::Init(context);

    m_deviceManager = ManagerLocator::GetDeviceManager();
    if (!m_deviceManager) {
        return E_FAIL;
    }
    m_shaderManager = ManagerLocator::GetManager<ShaderManager>();
    if (!m_shaderManager) {
        return E_FAIL;
    }

    return S_OK;
}

void PipelineResourcesManager::LoadShader(const std::string& shaderName) {
    m_shaderManager->LoadShaderByName(StringToWstring(shaderName));
}

void PipelineResourcesManager::LoadMesh(const std::string& meshName) {
    //m_shaderManager->UnloadShaderByName(StringToWstring(shaderName)
    // );
}

void PipelineResourcesManager::Shutdown()
{

}
