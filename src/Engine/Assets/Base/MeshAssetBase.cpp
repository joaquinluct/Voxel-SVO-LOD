#include "MeshAssetBase.h"
#include <Assets/Base/ShaderAsset.h>
#include <Assets/Base/TextureAsset.h>
#include <d3dcommon.h>
#include <Defines/Mesh.h>
#include <Locators/AssetLocator/AssetLocator.h>
#include <Locators/ManagerLocator/ManagerLocator.h>
#include <mutex>
#include <Services/Material.h>
#include <Util/Utils.h>
#include <wrl/client.h>

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
    return S_OK;
    // Obtener el vertexDefinition del shader
    m_name = m_meshConfig->name;
    m_shaderAssetName = m_meshConfig->shader;
    m_meshType = static_cast<Mesh::Type>(m_meshConfig->meshType);
    m_passes = m_meshConfig->render_passes;

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

HRESULT MeshAssetBase::InitBuffers() {
    m_vertexBuffer.push_back(nullptr);
    m_vertexBuffer.push_back(nullptr);
    m_indexBuffer.push_back(nullptr);
    m_indexBuffer.push_back(nullptr);
    m_vertexData.push_back({});
    m_vertexData.push_back({});
    m_indexData.push_back({});
    m_indexData.push_back({});
    m_vertexCount.push_back(0);
    m_vertexCount.push_back(0);
    m_indexCount.push_back(0);
    m_indexCount.push_back(0);
    return S_OK;
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

    hr = InitBuffers();
    if (FAILED(hr)) {
        OutputDebugStringA("MeshAssetBase::Init - ERROR: Buffers init.\n");
        return E_FAIL;
    }

    return S_OK;

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
    m_vertexCount[0] = 0;
    m_vertexCount[1] = 0;
    m_indexCount[0] = 0;
    m_indexCount[1] = 0;
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

Microsoft::WRL::ComPtr<ID3D11Buffer> MeshAssetBase::GetVertexBuffer(int index) {
    std::lock_guard<std::mutex> lock(m_mutex); // Bloquea el mutex durante la lectura
    return m_vertexBuffer[index];
}
Microsoft::WRL::ComPtr<ID3D11Buffer> MeshAssetBase::GetIndexBuffer(int index) {
    std::lock_guard<std::mutex> lock(m_mutex); // Bloquea el mutex durante la lectura
    return m_indexBuffer[index];
}

void MeshAssetBase::SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, int index) {
    std::lock_guard<std::mutex> lock(m_mutex); // Bloquea el mutex durante la escritura
    if (m_vertexBuffer.size() <= index) {
        m_vertexBuffer.push_back(vertexBuffer);
        return;
    }
    m_vertexBuffer[index] = vertexBuffer;
}
void MeshAssetBase::SetIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer, int index) {
    std::lock_guard<std::mutex> lock(m_mutex); // Bloquea el mutex durante la escritura
    if (m_indexBuffer.size() <= index) {
        m_indexBuffer.push_back(indexBuffer);
        return;
    }
    m_indexBuffer[index] = indexBuffer;
}

std::unique_lock<std::mutex > MeshAssetBase::LockBuffers() {
    return std::unique_lock<std::mutex>(m_mutex);
}

void MeshAssetBase::SwapBuffer() {
    int writeIndex = m_writeIndex.load();
    int readIndex = m_readIndex.load();
    m_writeIndex.store(readIndex);
    m_readIndex.store(writeIndex);
}

void MeshAssetBase::Shutdown() {
    for (size_t i = 0; i < m_vertexBuffer.size(); ++i) {
        if (m_vertexBuffer[i]) {
            m_vertexBuffer[i]->Release();
            m_vertexBuffer[i] = nullptr;
        }
    }
    m_vertexBuffer.clear();
    for (size_t i = 0; i < m_indexBuffer.size(); ++i) {
        if (m_indexBuffer[i]) {
            m_indexBuffer[i]->Release();
            m_indexBuffer[i] = nullptr;
        }
    }
    m_indexBuffer.clear();

    if (m_material) {
        SafeShutDown(m_material);
    }
    if (m_shadowMaterial) {
        SafeShutDown(m_shadowMaterial);
    }
}
