#include "Material.h"
#include <d3dcompiler.h> // Si usas D3DReadFileToBlob para cargar shaders
#include <DirectXMath.h> // Necesario para XMMATRIX, XMMatrixTranspose, etc.

Material::Material(ShaderManager* shaderManager, std::wstring shaderName) : m_texture(nullptr), m_samplerState(nullptr), m_matrixBuffer(nullptr) {
    vertexShader = shaderManager->GetVertexShader(shaderName);
    pixelShader = shaderManager->GetPixelShader(shaderName);

    if (!vertexShader || !pixelShader) {
        OutputDebugStringA("Error: No se pudieron cargar los shaders.\n");
    }

    inputLayout = shaderManager->GetInputLayout(shaderName);
}

Material::~Material()
{
    Release();
}

HRESULT Material::Init(ID3D11Device* device) {
    HRESULT hr = S_OK;

    // --- Descripci�n del Sampler State ---
    D3D11_SAMPLER_DESC samplerDesc;
    ZeroMemory(&samplerDesc, sizeof(samplerDesc));

    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0.0f;
    samplerDesc.BorderColor[1] = 0.0f;
    samplerDesc.BorderColor[2] = 0.0f;
    samplerDesc.BorderColor[3] = 0.0f;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

    // Crea el Sampler State
    hr = device->CreateSamplerState(&samplerDesc, &m_samplerState);
    if (FAILED(hr)) {
        OutputDebugStringA("Error al crear el Sampler State.\n");
        return hr;
    }

    // Crear el buffer de constantes para las matrices
    D3D11_BUFFER_DESC matrixBufferDesc;
    ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType); // Usa la estructura de Utils.h
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    hr = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if (FAILED(hr)) {
        OutputDebugStringA("Error al crear el Matrix Buffer.\n");
        return hr;
    }

    return S_OK;
}

void Material::SetTexture(ID3D11ShaderResourceView* texture) {
    m_texture = texture;
}

// Implementaci�n actualizada para enviar World, View, Projection por separado
void Material::SetShaderParameters(ID3D11DeviceContext* context, const DirectX::XMMATRIX& worldMatrix, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix) {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;

    // Transponer las matrices antes de copiarlas al buffer.
    DirectX::XMMATRIX transposedWorld = DirectX::XMMatrixTranspose(worldMatrix);
    DirectX::XMMATRIX transposedView = DirectX::XMMatrixTranspose(viewMatrix);
    DirectX::XMMATRIX transposedProjection = DirectX::XMMatrixTranspose(projectionMatrix);

    // Bloquear el buffer de constantes para escribir en �l.
    HRESULT hr = context->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr)) {
        OutputDebugStringA("Error al mapear el Matrix Buffer.\n");
        return;
    }

    // Obtener un puntero a los datos del buffer y copiar las matrices.
    dataPtr = (MatrixBufferType*)mappedResource.pData;
    dataPtr->worldMatrix = transposedWorld;
    dataPtr->viewMatrix = transposedView;
    dataPtr->projectionMatrix = transposedProjection;

    // Desbloquear el buffer.
    context->Unmap(m_matrixBuffer, 0);

    // Establecer el buffer de constantes en el pipeline del v�rtice shader.
    context->VSSetConstantBuffers(0, 1, &m_matrixBuffer); // Slot 0 (b0)
}


void Material::Apply(ID3D11DeviceContext* context) {
    // Establecer shaders
    if (!vertexShader || !pixelShader) {
        OutputDebugStringA("Error: Shaders no inicializados correctamente.\n");
        return;
    }
    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);

    if (inputLayout) {
        context->IASetInputLayout(inputLayout);
    }
    else {
        OutputDebugStringA("Error: Input Layout no inicializado.\n");
    }

    // Log: Aplicando material, textura y sampler
    // char logMsg[128];
    // sprintf_s(logMsg, sizeof(logMsg), "[Material::Apply] m_texture=%p, m_samplerState=%p\n", m_texture, m_samplerState);
    // OutputDebugStringA(logMsg);

    // Establecer textura (si hay)
    if (m_texture) {
        context->PSSetShaderResources(0, 1, &m_texture); // Slot 0 para la textura
    }
    // Siempre bindea el sampler, aunque no haya textura
    context->PSSetSamplers(0, 1, &m_samplerState);
}

void Material::Release() {
    SafeRelease(m_texture);
    SafeRelease(m_samplerState);
    SafeRelease(m_matrixBuffer); // Liberar el buffer de matrices
    // Los shaders y el inputLayout no se liberan aqu� si son gestionados por ShaderManager
}