#include "Skybox.h"

#include <AssetLocator/AssetLocator.h>
#include <Assets/Base/MeshAsset.h>
#include <CameraManager.h>
#include <Config/Assets/Mesh/SkyboxMeshConfig.h>
#include <Config/Game/System/SkyboxConfig.h>
#include <ConfigLocator/ConfigLocator.h>
#include <DirectXMath.h>
#include <Game/Systems/Lighting.h>
#include <IService.h>
#include <ManagerLocator/ManagerLocator.h>
#include <Managers/PipelineResourcesManager.h>
#include <REGISTER_SERVICE_MACRO.h>
#include <RenderManager/RenderManager.h>
#include <ServiceLocator/ServiceLocator.h>
#include <exception>
#include <memory>
#include <string>
#include <windows.h>

REGISTER_SERVICE_TYPE(Skybox, "Skybox")

Skybox::Skybox()
    : mesh{}, lighting{nullptr}, m_renderManager{nullptr}, m_eventTimer(0.0f), m_eventInterval(1.0f), sign(false),
      m_rotationSpeed(1.0f)
{
}

Skybox::~Skybox()
{
}

HRESULT Skybox::InitConfig()
{
    // Try to load config from ConfigLocator with robust error handling
    try
    {
        m_config = ConfigLocator::GetConfig<SkyboxConfig>();

        // Deep validation: check if config is truly valid
        if (m_config)
        {
            // Try to access the vectors safely
            bool sunColorValid = false;
            bool skyColorValid = false;

            try
            {
                sunColorValid = !m_config->sun_color.empty() && m_config->sun_color.size() >= 4;
            }
            catch (...)
            {
                OutputDebugStringA("[SKYBOX] ERROR: sun_color vector is corrupted\n");
            }

            try
            {
                skyColorValid = !m_config->sky_color.empty() && m_config->sky_color.size() >= 4;
            }
            catch (...)
            {
                OutputDebugStringA("[SKYBOX] ERROR: sky_color vector is corrupted\n");
            }

            // If both are valid, use them
            if (sunColorValid && skyColorValid)
            {
                try
                {
                    m_sunColor = XMFLOAT4(m_config->sun_color[0], m_config->sun_color[1], m_config->sun_color[2],
                                          m_config->sun_color[3]);
                    m_skyColor = XMFLOAT4(m_config->sky_color[0], m_config->sky_color[1], m_config->sky_color[2],
                                          m_config->sky_color[3]);
                    OutputDebugStringA("[SKYBOX] Config loaded successfully from YAML\n");
                    return S_OK;
                }
                catch (const std::exception &e)
                {
                    OutputDebugStringA("[SKYBOX] ERROR: Exception accessing config values: ");
                    OutputDebugStringA(e.what());
                    OutputDebugStringA("\n");
                }
                catch (...)
                {
                    OutputDebugStringA("[SKYBOX] ERROR: Unknown exception accessing config values\n");
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        OutputDebugStringA("[SKYBOX] ERROR: Exception loading config: ");
        OutputDebugStringA(e.what());
        OutputDebugStringA("\n");
    }
    catch (...)
    {
        OutputDebugStringA("[SKYBOX] ERROR: Unknown exception loading config\n");
    }

    // If we reach here, config loading failed - use hardcoded defaults
    OutputDebugStringA("[SKYBOX] WARNING: Using hardcoded default colors (config failed)\n");
    m_sunColor = XMFLOAT4(1.0f, 0.9f, 0.6f, 1.0f); // Warm yellow sun
    m_skyColor = XMFLOAT4(0.4f, 0.4f, 0.9f, 1.0f); // Blue sky

    return S_OK;
}

HRESULT Skybox::InitManagers()
{
    m_renderManager = ManagerLocator::GetManager<RenderManager>();
    if (!m_renderManager)
    {
        return E_FAIL;
    }
    m_cameraManager = ManagerLocator::GetManager<CameraManager>();
    if (!m_cameraManager)
    {
        return E_FAIL;
    }

    lighting = ServiceLocator::GetService<Lighting>();
    if (!lighting)
    {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT Skybox::InitMesh()
{
    OutputDebugStringA("[SKYBOX] Inicializando mesh del skybox\n");

    // Get the mesh name from config (default is "ProcSkyboxMesh")
    std::string meshName = "ProcSkyboxMesh"; // Default name matches the SkyboxConfig
    if (m_config && !m_config->mesh.empty())
    {
        meshName = m_config->mesh;
        OutputDebugStringA(("[SKYBOX] Usando mesh del config: " + meshName + "\n").c_str());
    }
    else
    {
        OutputDebugStringA("[SKYBOX] WARNING: Config mesh name empty, using default 'ProcSkyboxMesh'\n");
    }

    // Try to get the mesh from AssetLocator
    try
    {
        OutputDebugStringA(("[SKYBOX] Intentando obtener mesh '" + meshName + "' desde AssetLocator\n").c_str());
        mesh = AssetLocator::GetAsset<MeshAsset>(meshName);

        if (!mesh)
        {
            OutputDebugStringA(("[SKYBOX] ERROR: AssetLocator retornó nullptr para mesh '" + meshName + "'\n").c_str());
            OutputDebugStringA("[SKYBOX] → Intentando crear mesh temporalmente\n");

            // Fallback: create mesh directly as before (temporary)
            mesh = std::make_shared<MeshAsset>();
            if (!mesh)
            {
                OutputDebugStringA("[SKYBOX] ❌ ERROR: Failed to create fallback mesh\n");
                return E_FAIL;
            }

            HRESULT hr = mesh->Init();
            if (FAILED(hr))
            {
                OutputDebugStringA("[SKYBOX] ❌ ERROR: Failed to initialize fallback mesh\n");
                return hr;
            }

            OutputDebugStringA("[SKYBOX] ⚠️ WARNING: Using fallback mesh (may not have geometry)\n");
        }
        else
        {
            OutputDebugStringA("[SKYBOX] OK: Mesh obtenido correctamente desde AssetLocator\n");
        }

        // CRÍTICO: Llamar a Init() para que el mesh cargue su geometría desde el archivo .obj
        OutputDebugStringA("[SKYBOX] → Llamando Init() en el mesh para cargar geometría\n");
        HRESULT hr = mesh->Init();
        if (FAILED(hr))
        {
            OutputDebugStringA("[SKYBOX] ERROR: Failed to initialize mesh (geometry not loaded)\n");
            return hr;
        }

        // Verify the mesh has data after Init()
        int readIndex = mesh->GetReadIndex();
        UINT vertexCount = mesh->GetVertexCount(readIndex);
        UINT indexCount = mesh->GetIndexCount(readIndex);

        OutputDebugStringA(("[SKYBOX] Mesh stats: readIndex=" + std::to_string(readIndex) +
                            ", vertices=" + std::to_string(vertexCount) +
                            ", indices=" + std::to_string(indexCount) + "\n")
                               .c_str());

        if (vertexCount == 0 || indexCount == 0)
        {
            OutputDebugStringA("[SKYBOX] WARNING: Mesh has no geometry data after Init()\n");
            return E_FAIL;
        }

        OutputDebugStringA("[SKYBOX] OK: Mesh inicializado correctamente con geometria\n");
        return S_OK;
    }
    catch (const std::exception &e)
    {
        OutputDebugStringA("[SKYBOX] ❌ ERROR: Exception loading mesh: ");
        OutputDebugStringA(e.what());
        OutputDebugStringA("\n");
        return E_FAIL;
    }
    catch (...)
    {
        OutputDebugStringA("[SKYBOX] ❌ ERROR: Unknown exception loading mesh\n");
        return E_FAIL;
    }
}

HRESULT Skybox::Init()
{
    HRESULT hr = InitConfig();
    if (FAILED(hr))
    {
        return hr;
    }

    hr = InitManagers();
    if (FAILED(hr))
    {
        return hr;
    }

    hr = InitMesh();
    if (FAILED(hr))
    {
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
    // Update sun position and rotation matrix
    UpdateSunPosition(deltaTime);
}

void Skybox::Render()
{
    // The skybox is now rendered through the pipeline system
    // This method should add the skybox mesh to the render queue
    // Instead of directly calling mesh->Render()

    if (!mesh)
    {
        return;
    }

    // The actual rendering is handled by SceneManager through PipelineResourcesManager
    // We just need to make sure our mesh is registered and updated
    OutputDebugStringA("[SKYBOX] Skybox render requested (handled by pipeline)\n");
}

XMFLOAT4 Skybox::GetSunColor() const
{
    return m_sunColor;
}
XMFLOAT4 Skybox::GetSkyColor() const
{
    return m_skyColor;
}

void Skybox::Shutdown()
{
}
