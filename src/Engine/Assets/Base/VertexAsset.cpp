// VertexAsset.cpp
#include "VertexAsset.h"
#include <string>
#include <DDSTextureLoader.h> // Para CreateDDSTextureFromFile
#include <windows.h> // Para OutputDebugStringA
#include <AssetLocator/AssetLocator.h>
#include <ManagerLocator/ManagerLocator.h>
#include <Text/Text.h>
#include "../../../../resources/resource.h"
#include <REGISTER_ASSET_MACRO.h>

REGISTER_ASSET_TYPE(VertexAsset, "VertexAsset")

VertexAsset::VertexAsset() {
    // Constructor
}

VertexAsset::~VertexAsset() {}

//HRESULT VertexAsset::Init() {
//    m_deviceManager = ManagerLocator::GetDeviceManager();
//    if (!m_deviceManager) {
//        OutputDebugStringA("VertexAsset::Init - ERROR: DeviceManager not found.\n");
//        return E_FAIL;
//	}
//    return S_OK;
//}

HRESULT VertexAsset::Init() {

    if (m_config == nullptr) {
        return S_OK;
    }

    std::string vertexDef = m_config->name;

    if (vertexDef.empty()) {
        OutputDebugStringA("VertexAsset::Init - ERROR: Vertex definition is empty.\n");
        return E_FAIL;
	}

    return S_OK;
}
void VertexAsset::Shutdown() {
}

void VertexAsset::Render() {}

std::shared_ptr<VertexAsset> VertexAsset::GetAsset(const std::string& name) {
	return AssetLocator::GetVertexAsset(name);
}

void VertexAsset::SetConfig(std::shared_ptr<VertexAssetConfigBase> config) {
    m_config = config;
}