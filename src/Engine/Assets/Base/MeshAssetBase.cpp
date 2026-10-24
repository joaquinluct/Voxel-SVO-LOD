#include "MeshAssetBase.h"

#include <Assets/Base/ShaderAsset.h>
#include <Assets/Base/TextureAsset.h>
#include <Defines/Mesh.h>
#include <Locators/AssetLocator/AssetLocator.h>
#include <Locators/ManagerLocator/ManagerLocator.h>
#include <Services/Material.h>
#include <Util/Utils.h>
#include <d3dcommon.h>
#include <mutex>
#include <wrl/client.h>

HRESULT MeshAssetBase::InitManagers()
{
    m_deviceManager = ManagerLocator::GetDeviceManager();
    if (!m_deviceManager)
    {
        OutputDebugStringA("MeshAssetBase::Init - ERROR: DeviceManager not found.\n");
        return E_FAIL;
    }
    m_cameraManager = ManagerLocator::GetCameraManager();
    if (!m_cameraManager)
    {
        OutputDebugStringA("MeshAssetBase::Init - ERROR: CameraManager not found.\n");
        return E_FAIL;
    }
    m_shaderManager = ManagerLocator::GetShaderManager();
    if (!m_shaderManager)
    {
        OutputDebugStringA("MeshAssetBase::Init - ERROR: ShaderManager not found.\n");
        return E_FAIL;
    }
    return S_OK;
}

HRESULT MeshAssetBase::InitConfig()
{
    // Obtener el vertexDefinition del shader
    m_name = m_meshConfig->name;
    m_shaderAssetName = m_meshConfig->shader;
    m_meshType = static_cast<Mesh::Type>(m_meshConfig->meshType);
    m_passes = m_meshConfig->render_passes;

    m_shaderAsset = AssetLocator::GetShaderAsset(m_shaderAssetName);
    if (!m_shaderAsset)
    {
        OutputDebugStringA(
            ("MeshAssetBase::Init - ERROR: Failed to get ShaderAsset for mesh '" + m_name + "'.\n").c_str());
        return E_FAIL;
    }

    m_vertexDef = m_shaderAsset->GetConfig()->vertex_def;
    if (m_vertexDef.empty())
    {
        OutputDebugStringA(
            ("MeshAssetBase::Init - ERROR: No vertex definition found for mesh '" + m_name + "'.\n").c_str());
        return E_FAIL;
    }
    return S_OK;
}

HRESULT MeshAssetBase::InitTexture()
{
    std::string textureAssetName = m_meshConfig->texture;

    m_material = new Material();
    if (!m_material)
    {
        OutputDebugStringA(
            ("MeshAssetBase::Init - ERROR: Failed to create Material resource for mesh '" + m_name + "'.\n").c_str());
        return E_FAIL;
    }

    m_material->SetShaderName(StringToWstring(m_shaderAssetName));

    // CAMBIO: Init del material ANTES de intentar cargar texturas
    // Esto permite que shaders sin texturas (skybox procedural) funcionen
    HRESULT hr = m_material->Init();
    if (FAILED(hr))
    {
        OutputDebugStringA(
            ("MeshAssetBase::Init - ERROR: Failed to initialize Material for mesh '" + m_name + "'.\n").c_str());
        delete m_material;
        m_material = nullptr;
        return hr;
    }

    // CAMBIO: Intentar cargar texturas solo si están definidas y disponibles
    if (!textureAssetName.empty())
    {
        OutputDebugStringA(
            ("MeshAssetBase::Init - Attempting to load texture '" + textureAssetName + "' for mesh '" + m_name + "'\n")
                .c_str());

        auto textureAsset = AssetLocator::GetTextureAsset(textureAssetName);
        if (textureAsset)
        {
            // Si la textura existe, intentar cargarla
            hr = textureAsset->Init();
            if (SUCCEEDED(hr))
            {
                OutputDebugStringA(("MeshAssetBase::Init - Texture '" + textureAssetName +
                                    "' loaded successfully for mesh '" + m_name + "'\n")
                                       .c_str());
            }
            else
            {
                OutputDebugStringA(("MeshAssetBase::Init - WARNING: Failed to initialize texture '" + textureAssetName +
                                    "' for mesh '" + m_name + "'. Continuing without texture.\n")
                                       .c_str());
                // NO retornamos E_FAIL aquí - el mesh puede funcionar sin textura
            }
        }
        else
        {
            OutputDebugStringA(("MeshAssetBase::Init - WARNING: Texture asset '" + textureAssetName +
                                "' not found for mesh '" + m_name + "'. Continuing without texture.\n")
                                   .c_str());
            // NO retornamos E_FAIL aquí - el mesh puede funcionar sin textura
        }
    }
    else
    {
        OutputDebugStringA(
            ("MeshAssetBase::Init - No texture specified for mesh '" + m_name + "' (texture-less mesh).\n").c_str());
    }

    return S_OK;
}

HRESULT MeshAssetBase::InitShadows()
{
    bool castShadows = m_meshConfig->cast_shadows;
    if (!castShadows)
    {
        return S_OK;
    }
    m_shadowMaterial = new Material();
    m_shadowMaterial->SetShaderName(StringToWstring(m_meshConfig->shader_shadows));
    HRESULT hr = m_shadowMaterial->Init();
    if (FAILED(hr))
    {
        OutputDebugStringA("MeshAssetBase::Init - ERROR: Failed to initialize Material resource for shadows.\n");
        return E_FAIL;
    }
    return hr;
}

HRESULT MeshAssetBase::InitBuffers()
{
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

HRESULT MeshAssetBase::Init()
{
    // Initialize managers first (even if m_meshConfig is null)
    // This allows dynamic meshes to work without config
    HRESULT hr = InitManagers();
    if (FAILED(hr))
    {
        OutputDebugStringA("MeshAssetBase::Init - ERROR: Managers init.\n");
        return E_FAIL;
    }

    hr = InitBuffers();
    if (FAILED(hr))
    {
        OutputDebugStringA("MeshAssetBase::Init - ERROR: Buffers init.\n");
        return E_FAIL;
    }

    // If no config, we're done (dynamic mesh case)
    if (m_meshConfig == nullptr)
    {
        OutputDebugStringA("MeshAssetBase::Init - ?? WARNING: m_meshConfig is NULL, skipping config-based init\n");
        return S_OK;
    }

    // Config-based initialization
    m_name = m_meshConfig->name;
    OutputDebugStringA(("[MeshAssetBase::Init] ? Config found, m_name assigned: '" + m_name + "'\n").c_str());

    hr = InitConfig();
    if (FAILED(hr))
    {
        OutputDebugStringA("MeshAssetBase::Init - ERROR: Config init.\n");
        return E_FAIL;
    }

    hr = InitTexture();
    if (FAILED(hr))
    {
        OutputDebugStringA("MeshAssetBase::Init - ERROR: Texture init.\n");
        return E_FAIL;
    }

    hr = InitShadows();
    if (FAILED(hr))
    {
        OutputDebugStringA("MeshAssetBase::Init - ERROR: Shadows init.\n");
        return E_FAIL;
    }

    OutputDebugStringA("[MeshAssetBase::Init] ? Base initialization completed\n");
    return S_OK;
}

void MeshAssetBase::ClearMeshData()
{
    m_vertexes.clear();
    m_indexes.clear();
    m_vertexCount[0] = 0;
    m_vertexCount[1] = 0;
    m_indexCount[0] = 0;
    m_indexCount[1] = 0;
}

std::string MeshAssetBase::GetName() const
{
    if (m_name.empty() && m_meshConfig)
    {
        return m_meshConfig->name;
    }
    return m_name;
}

Mesh::DrawType MeshAssetBase::GetDrawType() const
{
    if (!m_meshConfig)
    {
        return Mesh::DrawType::None;
    }
    return static_cast<Mesh::DrawType>(m_meshConfig->drawType);
}

D3D11_PRIMITIVE_TOPOLOGY MeshAssetBase::GetPrimitiveTopology() const
{
    if (!m_meshConfig)
    {
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }
    return static_cast<D3D11_PRIMITIVE_TOPOLOGY>(m_meshConfig->primitiveTopology);
}

Microsoft::WRL::ComPtr<ID3D11Buffer> MeshAssetBase::GetVertexBuffer(int index)
{
    std::lock_guard<std::mutex> lock(m_mutex); // Bloquea el mutex durante la lectura
    return m_vertexBuffer[index];
}
Microsoft::WRL::ComPtr<ID3D11Buffer> MeshAssetBase::GetIndexBuffer(int index)
{
    std::lock_guard<std::mutex> lock(m_mutex); // Bloquea el mutex durante la lectura
    return m_indexBuffer[index];
}

void MeshAssetBase::SetVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer, int index)
{
    std::lock_guard<std::mutex> lock(m_mutex); // Bloquea el mutex durante la escritura
    if (m_vertexBuffer.size() <= index)
    {
        m_vertexBuffer.push_back(vertexBuffer);
        return;
    }
    m_vertexBuffer[index] = vertexBuffer;
}
void MeshAssetBase::SetIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer, int index)
{
    std::lock_guard<std::mutex> lock(m_mutex); // Bloquea el mutex durante la escritura
    if (m_indexBuffer.size() <= index)
    {
        m_indexBuffer.push_back(indexBuffer);
        return;
    }
    m_indexBuffer[index] = indexBuffer;
}

std::unique_lock<std::mutex> MeshAssetBase::LockBuffers()
{
    return std::unique_lock<std::mutex>(m_mutex);
}

void MeshAssetBase::SwapBuffer()
{
    int writeIndex = m_writeIndex.load();
    int readIndex = m_readIndex.load();
    m_writeIndex.store(readIndex);
    m_readIndex.store(writeIndex);
}

void MeshAssetBase::Shutdown()
{
    for (size_t i = 0; i < m_vertexBuffer.size(); ++i)
    {
        if (m_vertexBuffer[i])
        {
            m_vertexBuffer[i]->Release();
            m_vertexBuffer[i] = nullptr;
        }
    }
    m_vertexBuffer.clear();
    for (size_t i = 0; i < m_indexBuffer.size(); ++i)
    {
        if (m_indexBuffer[i])
        {
            m_indexBuffer[i]->Release();
            m_indexBuffer[i] = nullptr;
        }
    }
    m_indexBuffer.clear();

    if (m_material)
    {
        SafeShutDown(m_material);
    }
    if (m_shadowMaterial)
    {
        SafeShutDown(m_shadowMaterial);
    }
}
