#include "Material.h"
#include <memory>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <ShaderManager.h>
#include <DirectX/DirectXTex/DirectXTex.h>
#include <DirectX/WICTextureLoader/WICTextureLoader11.h>
#include <ManagerLocator/ManagerLocator.h>
#include <ConfigLocator/ConfigLocator.h>
#include <Defines/Texture.h>
#include <Defines/MatrixDefinition.h>
#include <Defines/Light.h>
#include <Util/DirectXUtils.h>
#include <REGISTER_SERVICE_MACRO.h>

REGISTER_SERVICE_TYPE(Material, "Material")

Material::Material() 
    : m_shaderName(L""),
    m_texture_albedo(nullptr),
    m_texture_normal(nullptr),
    m_texture_roughness(nullptr),
    m_texture_metallic(nullptr),
    m_texture_ao(nullptr),
    m_texture(nullptr),
    m_samplerState(nullptr),
    m_matrixBuffer(nullptr),
    m_deviceManager(nullptr),
    m_cameraManager(nullptr),
    m_shaderManager(nullptr),
    m_renderManager(nullptr),
	m_lighting(nullptr),
    m_shadows(nullptr),
    vertexShader(nullptr),
    pixelShader(nullptr),
	inputLayout(nullptr)
{
}

HRESULT Material::InitManagers() {
    m_deviceManager = ManagerLocator::GetDeviceManager();
    if (m_deviceManager == nullptr) {
        OutputDebugStringA("Error: DeviceManager no inicializado.\n");
        return E_FAIL;
    }

    m_cameraManager = ManagerLocator::GetCameraManager();
    if (m_cameraManager == nullptr) {
        OutputDebugStringA("Error: CameraManager no inicializado.\n");
        return E_FAIL;
    }

    m_shaderManager = ManagerLocator::GetShaderManager();
    if (m_shaderManager == nullptr) {
        OutputDebugStringA("Error: ShaderManager no inicializado.\n");
        return E_FAIL;
    }

	m_renderManager = ManagerLocator::GetManager<RenderManager>();
    if (m_renderManager == nullptr) {
        OutputDebugStringA("Error: RenderManager no inicializado.\n");
        return E_FAIL;
	}

	m_lighting = ServiceLocator::GetService<Lighting>();
    if (m_lighting == nullptr) {
        OutputDebugStringA("Error: Lighting no inicializado.\n");
        return E_FAIL;
	}

	m_shadows = ServiceLocator::GetService<Shadows>();
    if (m_shadows == nullptr) {
        OutputDebugStringA("Error: Shadows no inicializado.\n");
		return E_FAIL;
	}

    return S_OK;
}

HRESULT Material::InitPixelAndVertexShaders() {
    vertexShader = m_shaderManager->GetVertexShader(m_shaderName);
    pixelShader = m_shaderManager->GetPixelShader(m_shaderName);

    if (!vertexShader || !pixelShader) {
        OutputDebugStringA("Error: No se pudieron cargar los shaders.\n");
    }
    return S_OK;
}

HRESULT Material::InitSampleState() {
    m_shaderManager = ManagerLocator::GetShaderManager();
    if (m_shaderManager == nullptr) {
        OutputDebugStringA("Error: ShaderManager no inicializado.\n");
        return E_FAIL;
    }

    Microsoft::WRL::ComPtr<ID3D11Device> device = m_deviceManager->GetDevice();

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // Tipo de filtrado (bilineal, trilineal, etc.)
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;    // Comportamiento de "envoltura" para coordenadas U
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;    // Comportamiento de "envoltura" para coordenadas V
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;    // Comportamiento de "envoltura" para coordenadas W (para 3D/cubemaps)
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1; // O más si usas filtrado anisotrópico
    samplerDesc.BorderColor[0] = 0; samplerDesc.BorderColor[1] = 0; samplerDesc.BorderColor[2] = 0; samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // Usar todos los mipmaps

    HRESULT hr = device->CreateSamplerState(&samplerDesc, &m_samplerState);
    if (FAILED(hr)) {
        // Manejar error
        return hr;
    }
    return hr;
}
HRESULT Material::InitMatrixBuffer() {

	m_shaderManager = ManagerLocator::GetShaderManager();
    if (m_shaderManager == nullptr) {
        OutputDebugStringA("Error: ShaderManager no inicializado.\n");
        return E_FAIL;
	}
    
	std::map<std::string, std::pair<int, std::unique_ptr<MatrixDefinition::AnyMatrixBuffer>>>& matrixBuffers = m_shaderManager->GetMatrixBuffers(m_shaderName);

    if (matrixBuffers.empty()) {
        OutputDebugStringA("Error: No se encontraron Matrix Buffers para el shader.\n");
        return E_FAIL;
	}

    for (const auto& [matrixName, matrix] : matrixBuffers) {

        UINT buffer_byte_width = 0;

        std::visit([&](auto& currentMatrixStruct) {
            using CurrentStructType = std::decay_t<decltype(currentMatrixStruct)>;
            buffer_byte_width = currentMatrixStruct.Size();
            }, * matrix.second);

        // Si no es un múltiplo de 16, redondear hacia arriba
        if (buffer_byte_width % 16 != 0) {
            buffer_byte_width = (buffer_byte_width / 16 + 1) * 16;
        }

        D3D11_BUFFER_DESC cbd = {};
        cbd.Usage = D3D11_USAGE_DYNAMIC;
        cbd.ByteWidth = buffer_byte_width;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        Microsoft::WRL::ComPtr<ID3D11Device> device = m_deviceManager->GetDevice();

        m_constantBuffers[matrixName] = nullptr;

        HRESULT hr = device->CreateBuffer(&cbd, nullptr, &m_constantBuffers[matrixName]);
        if (FAILED(hr)) {
            OutputDebugStringA("Error al crear el Matrix Buffer.\n");
            return hr;
        }
    }
    return S_OK;
}

HRESULT Material::Init() {
    HRESULT hr = S_OK;

    if (m_shaderName.empty()) {
        return S_OK;
	}

    Shutdown(); // Asegurarse de liberar recursos previos

    hr = InitManagers();
    if (FAILED(hr)) {
        OutputDebugStringA("Material Service Error: Managers.\n");
        return hr;
    }
    	
    hr = InitPixelAndVertexShaders();
    if (FAILED(hr)) {
        OutputDebugStringA("Material Service Error: Pixel/Vertex shader.\n");
        return hr;
    }

    inputLayout = m_shaderManager->GetInputLayout(m_shaderName);
    if (!inputLayout) {
        OutputDebugStringA("Material Service Error: Imput Layout fail.\n");
        return hr;
    }

    hr = InitSampleState();
    if (FAILED(hr)) {
        OutputDebugStringA("Material Service Error: Sampler state fail.\n");
        return hr;
    }

    hr = InitMatrixBuffer();
    if (FAILED(hr)) {
        OutputDebugStringA("Material Service Error: Matrix buffer fail.\n");
        return hr;
    }

    return hr;
}

void Material::Render() {

    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = m_deviceManager->GetContext();

    XMMATRIX worldMatrix = XMMatrixIdentity();
    XMMATRIX viewMatrix = XMMatrixTranspose(m_cameraManager->GetCurrentViewMatrix());
    XMMATRIX projectionMatrix = XMMatrixTranspose(m_cameraManager->GetCurrentProjectionMatrix());

    MatrixDefinitionBase::MatrixParams matrixParams{};
    if (m_renderManager->IsRenderColourPassActive()) {
        matrixParams.worldMatrix = worldMatrix;
        matrixParams.viewMatrix = viewMatrix;
        matrixParams.projectionMatrix = projectionMatrix;
        matrixParams.cameraPosition = m_cameraManager->GetCurrentCameraPosition();
        matrixParams.lightDirection = m_lighting->GetLightDirection();
        matrixParams.lightColor = m_lighting->GetLightColor();
        matrixParams.materialAO = 0.4f;
    } else if (m_renderManager->IsRenderShadowsPassActive()) {
        matrixParams.worldMatrix = worldMatrix;
        matrixParams.lightViewProjectionMatrix = m_shadows->GetLightViewProjectionMatrix();
    } 
    
	SetConstantBuffers(context, matrixParams);

    Apply(context);
}

void Material::SetTexture(ID3D11ShaderResourceView* texture, std::string textureMap) {
    if (textureMap == TEXTURE_MAP_ALBEDO.data()) m_texture_albedo = texture;
    if (textureMap == TEXTURE_MAP_NORMAL.data()) m_texture_normal = texture;
    if (textureMap == TEXTURE_MAP_ROUGHNESS.data()) m_texture_roughness = texture;
    if (textureMap == TEXTURE_MAP_METALLIC.data()) m_texture_metallic = texture;
    if (textureMap == TEXTURE_MAP_AO.data()) m_texture_ao = texture;
}

void Material::SetConstantBuffers(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, MatrixDefinitionBase::MatrixParams matrixParams, int slot) {
    m_shaderManager->SetConstantsBuffers(m_shaderName, matrixParams, m_constantBuffers, context);
}

void Material::Apply(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) {
    // Establecer shaders
    if (!vertexShader || !pixelShader) {
        OutputDebugStringA("Error: Shaders no inicializados correctamente.\n");
        return;
    }
    context->VSSetShader(vertexShader.Get(), nullptr, 0);
    context->PSSetShader(pixelShader.Get(), nullptr, 0);

    if (inputLayout) {
        context->IASetInputLayout(inputLayout.Get());
    }
    else {
        OutputDebugStringA("Error: Input Layout no inicializado.\n");
    }

    // Log: Aplicando material, textura y sampler
    // char logMsg[128];
    // sprintf_s(logMsg, sizeof(logMsg), "[Material::Apply] m_texture=%p, m_samplerState=%p\n", m_texture, m_samplerState);
    // OutputDebugStringA(logMsg);

    // Establecer textura (si hay)
    // Array para almacenar todas las vistas de recursos del shader
    const int mapSize = 5;
    ID3D11ShaderResourceView* texturesToBind[mapSize] = { nullptr, nullptr, nullptr, nullptr, nullptr }; // Inicializa todos los elementos a nullptr

    // Asigna tus texturas a las posiciones correctas en el array.
    // Asegúrate de que las variables miembro m_texture_albedo, m_texture_normal, etc.,
    // sean punteros a ID3D11ShaderResourceView*.

    // Slot 0: Albedo
    if (m_texture_albedo) {
        texturesToBind[0] = m_texture_albedo;
    }

    // Slot 1: Normal
    if (m_texture_normal) {
        texturesToBind[1] = m_texture_normal;
    }

    // Slot 2: Roughness
    // *** IMPORTANTE: Tu código original comprobaba 'if (m_texture_normal)'.
    // *** Deberías comprobar 'if (m_texture_roughness)' para asegurarte de que el mapa de rugosidad existe.
    if (m_texture_roughness) {
        texturesToBind[2] = m_texture_roughness;
    }

    if (m_texture_metallic) {
        texturesToBind[3] = m_texture_metallic;
    }

    // Slot 3: Ambient Occlusion (AO)
    // *** IMPORTANTE: Similar al caso anterior, tu código original comprobaba 'if (m_texture_normal)'.
    // *** Deberías comprobar 'if (m_texture_ao)' para el mapa de oclusión ambiental.
    if (m_texture_ao) {
        texturesToBind[4] = m_texture_ao;
    }

    // Finalmente, vincula todas las texturas en una sola llamada.
    // 'StartSlot' es 0, y 'NumViews' es 4 porque queremos vincular 4 texturas desde el slot 0.
    context->PSSetShaderResources(0, mapSize, texturesToBind);

    // Siempre bindea el sampler, aunque no haya textura
    context->PSSetSamplers(0, 1, &m_samplerState);
}

ID3D11ShaderResourceView* Material::LoadTextureFromFile(std::shared_ptr<ID3D11Device> device, const std::wstring& filename) {
    ID3D11ShaderResourceView* texture = nullptr;
    HRESULT hr = DirectX::CreateWICTextureFromFile(device.get(), filename.c_str(), nullptr, &texture);
    if (FAILED(hr)) {
        //std::cerr << "Failed to load texture from file: " << filename.c_str() << " HRESULT: " << hr << std::endl;
        return nullptr;
    }
    return texture;
}

float av = 10.0f;

void Material::Update(float deltatime) {
    m_keyboard = ManagerLocator::GetKeyboardManager();

    if (m_keyboard == nullptr) {
        return;
	}

    if (m_keyboard->IsKeyDown('T')) {
		debug_lightDirection.x += deltatime * av; // Incrementar la dirección de la luz hacia arriba
    }
    if (m_keyboard->IsKeyDown('G')) {
        debug_lightDirection.x -= deltatime * av; // Incrementar la dirección de la luz hacia arriba
    }
    if (m_keyboard->IsKeyDown('Y')) {
        debug_lightDirection.y += deltatime * av; // Incrementar la dirección de la luz hacia arriba
    }
    if (m_keyboard->IsKeyDown('H')) {
        debug_lightDirection.y -= deltatime * av; // Incrementar la dirección de la luz hacia arriba
    }
    if (m_keyboard->IsKeyDown('U')) {
        debug_lightDirection.z += deltatime * av; // Incrementar la dirección de la luz hacia arriba
    }
    if (m_keyboard->IsKeyDown('J')) {
        debug_lightDirection.z -= deltatime * av; // Incrementar la dirección de la luz hacia arriba
    }
}

void Material::Shutdown() {
    SafeRelease(m_texture);
    SafeRelease(m_matrixBuffer); // Liberar el buffer de matrices
    // Los shaders y el inputLayout no se liberan aqu� si son gestionados por ShaderManager
}