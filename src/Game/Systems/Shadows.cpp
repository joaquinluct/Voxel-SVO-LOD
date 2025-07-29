#include "Shadows.h"

// Para acceder al contexto del dispositivo
#include <DeviceManager.h>
#include <ManagerLocator/ManagerLocator.h>
#include <ConfigLocator/ConfigLocator.h> // Si usas configuración para SHADOW_MAP_WIDTH/HEIGHT
#include <REGISTER_SERVICE_MACRO.h> // Tu macro para registrar servicios

// Registrar el servicio
REGISTER_SERVICE_TYPE(Shadows, "Shadows")

Shadows::Shadows()
    : m_deviceManager(nullptr),
    m_shaderManager(nullptr),
    m_numViewports(1), // Generalmente solo hay un viewport, pero se guarda por si acaso
    m_oldDSV(nullptr),
    m_oldRasterizerState(nullptr),
    m_oldRTV(nullptr),
    m_oldViewport() {
    // Inicializar ComPtrs a nullptr automáticamente
    // Inicializar matrices a identidad (opcional, se recalcularán)
    m_lightViewMatrix = DirectX::XMMatrixIdentity();
    m_lightProjectionMatrix = DirectX::XMMatrixIdentity();
    m_lightViewProjectionMatrix = DirectX::XMMatrixIdentity();
    m_lightSpaceData.worldMatrix = DirectX::XMMatrixIdentity();
    m_lightSpaceData.lightViewProjectionMatrix = DirectX::XMMatrixIdentity();
}

Shadows::~Shadows() {
    Shutdown();
}

HRESULT Shadows::Init() {
    // 1. Obtener maangers
    m_deviceManager = ManagerLocator::GetDeviceManager();
    if (!m_deviceManager) {
        OutputDebugStringA("Error: DeviceManager no inicializado en Shadows.\n");
        return E_FAIL;
    }
    m_shaderManager = ManagerLocator::GetManager<ShaderManager>(); // O ManagerLocator si es Manager
    if (!m_shaderManager) {
        OutputDebugStringA("Error: ShaderManager no inicializado en Shadows.\n");
        return E_FAIL;
    }

    Microsoft::WRL::ComPtr<ID3D11Device> device = m_deviceManager->GetDevice();
    if (!device) return E_FAIL; // Error check

    return S_OK;
}

void Shadows::Shutdown() {
    // ComPtrs se liberan automáticamente cuando salen de scope o se reasignan.
    // Si usaras punteros crudos, aquí irían los SafeRelease().
    // Asegúrate de que los shaders y input layouts que obtienes del ShaderManager no los liberes aquí
    // si el ShaderManager es quien los posee. Con ComPtr, si obtienes una nueva ComPtr, la propiedad se comparte.
    // Si los obtienes como raw pointers, NO los liberes aquí.

    // Aquí, al ser ComPtr, se liberan automáticamente.
    m_shadowMapTexture2D.Reset();
    m_shadowMapDSV.Reset();
    m_shadowMapSRV.Reset();
    m_shadowMapSamplerState.Reset();

    m_depthVS.Reset();
    m_depthPS.Reset();
    m_depthInputLayout.Reset();

    m_lightSpaceMatricesCB.Reset();

    m_shadowRasterizerState.Reset();
    m_oldRasterizerState.Reset();

    // Las referencias a gestores se liberan automáticamente por shared_ptr
    m_deviceManager = nullptr;
    m_shaderManager = nullptr;
}

void Shadows::UpdateLightMatrices(const DirectX::XMFLOAT3& lightDirection, const DirectX::XMFLOAT3& sceneCenter, float sceneRadius)
{
    // Cargar vectores de DirectX::XMFLOAT3
    DirectX::XMVECTOR LightDir = DirectX::XMLoadFloat3(&lightDirection);
    DirectX::XMVECTOR LightTarget = DirectX::XMLoadFloat3(&sceneCenter);
    DirectX::XMVECTOR LightUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // Típicamente el eje Y es 'up'

    // Calcular la posición de la luz: el centro de la escena, pero alejado en la dirección opuesta a la luz
    // Multiplicar por sceneRadius * 2.0f asegura que la escena completa esté dentro de la vista ortográfica
    DirectX::XMVECTOR LightPos = LightTarget - (DirectX::XMVector3Normalize(LightDir) * sceneRadius * 2.0f);

    // Calcular las matrices de vista y proyección de la luz
    m_lightViewMatrix = DirectX::XMMatrixLookAtLH(LightPos, LightTarget, LightUp);
    // Para una proyección ortográfica, el ancho y alto son sceneRadius * 2.0f para cubrir el diámetro
    m_lightProjectionMatrix = DirectX::XMMatrixOrthographicLH(sceneRadius * 2.0f, sceneRadius * 2.0f, 0.1f, sceneRadius * 4.0f);

    // Combinar View y Projection
    m_lightViewProjectionMatrix = m_lightViewMatrix * m_lightProjectionMatrix;

    // Almacenar la matriz de ViewProjection de la luz en la copia CPU del Constant Buffer, transpuesta para HLSL
    m_lightSpaceData.lightViewProjectionMatrix = DirectX::XMMatrixTranspose(m_lightViewProjectionMatrix);
}

void Shadows::UpdateWorldMatrixInLightCB(const DirectX::XMMATRIX& worldMatrix)
{
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = m_deviceManager->GetContext();
    if (!context) return;

    // 1. Actualizar la worldMatrix en la copia CPU de los datos del Constant Buffer, transpuesta para HLSL
    m_lightSpaceData.worldMatrix = DirectX::XMMatrixTranspose(worldMatrix);

    // 2. Mapear el Constant Buffer en la GPU para escribir en él
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = context->Map(m_lightSpaceMatricesCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (SUCCEEDED(hr)) {
        // 3. Copiar SOLO la worldMatrix a la memoria mapeada.
        // Asume que worldMatrix es el primer miembro de tu struct LightSpaceMatricesData en HLSL.
        memcpy(mappedResource.pData, &m_lightSpaceData.worldMatrix, sizeof(DirectX::XMMATRIX));

        // 4. Desmapear el Constant Buffer
        context->Unmap(m_lightSpaceMatricesCB.Get(), 0);
    }
    else {
        OutputDebugStringA("Error: Fallo al mapear m_lightSpaceMatricesCB para actualizar la worldMatrix.\n");
    }
}