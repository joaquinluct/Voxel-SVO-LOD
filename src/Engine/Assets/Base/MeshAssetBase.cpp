#include "MeshAssetBase.h"
#include <Assets/Base/ShaderAsset.h>
#include <Defines/Mesh.h>
#include <Locators/AssetLocator/AssetLocator.h>
#include <Locators/ManagerLocator/ManagerLocator.h>
#include <Managers/DeviceManager.h>
#include <Services/Material.h>
#include <Util/Utils.h>

HRESULT MeshAssetBase::InitManagers() {
	m_deviceManager = ManagerLocator::GetDeviceManager();
	if (!m_deviceManager) {
		OutputDebugStringA("MeshAssetBase::Init - ERROR: DeviceManager not found.\n");
		return E_FAIL;
	}
	m_cameraManager = ManagerLocator::GetCameraManager();
	if (!m_cameraManager) {
		OutputDebugStringA("MeshAssetBase::Init - ERROR: CameraManager not found.\n");
		return E_FAIL;
	}
	m_shaderManager = ManagerLocator::GetShaderManager();
	if (!m_shaderManager) {
		OutputDebugStringA("MeshAssetBase::Init - ERROR: ShaderManager not found.\n");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT MeshAssetBase::InitConfig() {
	// Obtener el vertexDefinition del shader
	m_name = m_meshConfig->name;
	m_shaderAssetName = m_meshConfig->shader;
	m_meshType = static_cast<Mesh::Type>(m_meshConfig->meshType);

	m_shaderAsset = AssetLocator::GetShaderAsset(m_shaderAssetName);
	if (!m_shaderAsset) {
		OutputDebugStringA(("MeshAssetBase::Init - ERROR: Failed to get ShaderAsset for mesh '" + m_name + "'.\n").c_str());
		return E_FAIL;
	}

	m_vertexDef = m_shaderAsset->GetConfig()->vertex_def;
	if (m_vertexDef.empty()) {
		OutputDebugStringA(("MeshAssetBase::Init - ERROR: No vertex definition found for mesh '" + m_name + "'.\n").c_str());
		return E_FAIL;
	}
	return S_OK;
}

HRESULT MeshAssetBase::InitTexture() {
	std::string textureAssetName = m_meshConfig->texture;

	m_material = new Material();
	if (!m_material) {
		OutputDebugStringA(("MeshAssetBase::Init - ERROR: Failed to create Material resource for mesh '" + m_name + "'.\n").c_str());
		return E_FAIL;
	}

	m_material->SetShaderName(StringToWstring(m_shaderAssetName));

	HRESULT hr = m_material->Init();
	if (FAILED(hr)) {
		OutputDebugStringA(("MeshAssetBase::Init - ERROR: Failed to initialize Material resource for mesh '" + m_name + "'.\n").c_str());
		return E_FAIL;
	}

	if (!textureAssetName.empty() && textureAssetName != "none") {
		m_textureTransforms = m_meshConfig->texture_transforms;
		m_textureAsset = AssetLocator::GetTextureAsset(textureAssetName);
		m_textureAsset->SetTextureView(m_material);
		m_material->SetTextureType(m_textureAsset->GetTextureType());
		if (m_textureTransforms.size() == 4) {
			XMFLOAT4 textureTransforms = XMFLOAT4(m_textureTransforms[0], m_textureTransforms[1], m_textureTransforms[2], m_textureTransforms[3]);
			m_material->SetTextureTranforms(textureTransforms);
		}
	}

	return hr;
}

HRESULT MeshAssetBase::InitShadows()
{
	bool castShadows = m_meshConfig->cast_shadows;
	if (!castShadows) {
		return S_OK;
	}
	m_shadowMaterial = new Material();
	m_shadowMaterial->SetShaderName(StringToWstring(m_meshConfig->shader_shadows));
	HRESULT hr = m_shadowMaterial->Init();
	if (FAILED(hr)) {
		OutputDebugStringA("MeshAssetBase::Init - ERROR: Failed to initialize Material resource for shadows.\n");
		return E_FAIL;
	}
	return hr;
}

HRESULT MeshAssetBase::Init() {
	if (m_meshConfig == nullptr) {
		return S_OK;
	}

	m_name = m_meshConfig->name;

	HRESULT hr = InitManagers();
	if (FAILED(hr)) {
		OutputDebugStringA("MeshAssetBase::Init - ERROR: Managers init.\n");
		return E_FAIL;
	}

	hr = InitConfig();
	if (FAILED(hr)) {
		OutputDebugStringA("MeshAssetBase::Init - ERROR: Config init.\n");
		return E_FAIL;
	}

	hr = InitTexture();
	if (FAILED(hr)) {
		OutputDebugStringA("MeshAssetBase::Init - ERROR: Texture init.\n");
		return E_FAIL;
	}

	hr = InitShadows();
	if (FAILED(hr)) {
		OutputDebugStringA("MeshAssetBase::Init - ERROR: Shadows init.\n");
		return E_FAIL;
	}

	return S_OK;
}

void MeshAssetBase::ClearMeshData() {
	m_vertexes.clear();
	m_indexes.clear();
	m_indexCount = 0;
	m_indexCount = 0;
}

std::string MeshAssetBase::GetName() const {
	if (m_name.empty() && m_meshConfig) {
		return m_meshConfig->name;
	}
	return m_name;
}

Mesh::DrawType MeshAssetBase::GetDrawType() const {
	if (!m_meshConfig) {
		return Mesh::DrawType::None;
	}
	return static_cast<Mesh::DrawType>(m_meshConfig->drawType);
}

D3D11_PRIMITIVE_TOPOLOGY MeshAssetBase::GetPrimitiveTopology() const {
	if (!m_meshConfig) {
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}
	return static_cast<D3D11_PRIMITIVE_TOPOLOGY>(m_meshConfig->primitiveTopology);
}

Microsoft::WRL::ComPtr<ID3D11Buffer> MeshAssetBase::GetVertexBuffer() {
	std::lock_guard<std::mutex> lock(m_mutex); // Bloquea el mutex durante la lectura
	return m_vertexBuffer;
}
Microsoft::WRL::ComPtr<ID3D11Buffer> MeshAssetBase::GetIndexBuffer() {
	std::lock_guard<std::mutex> lock(m_mutex); // Bloquea el mutex durante la lectura
	return m_indexBuffer;
}

void MeshAssetBase::SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer) {
	std::lock_guard<std::mutex> lock(m_mutex); // Bloquea el mutex durante la escritura
	m_vertexBuffer = vertexBuffer;
}
void MeshAssetBase::SetIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer) {
	std::lock_guard<std::mutex> lock(m_mutex); // Bloquea el mutex durante la escritura
	m_indexBuffer = indexBuffer;
}

std::unique_lock<std::mutex > MeshAssetBase::LockBuffers() {
	return std::unique_lock<std::mutex>(m_mutex);
}

void MeshAssetBase::Shutdown() {
	if (m_vertexBuffer) {
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
	}
	if (m_indexBuffer) {
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
	}
	if (m_material) {
		SafeShutDown(m_material);
	}
	if (m_shadowMaterial) {
		SafeShutDown(m_shadowMaterial);
	}
}
