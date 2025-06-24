// VoxelBasicInitializer.cpp
#include "VoxelBasicTextureConfig.h"
#include <string> // Para OutputDebugStringA
#include <memory> // Para ReleaseComPtr si la usas, o directamente para RAII de COM objects
#include <Windows.h> // Para S_OK, FAILED, OutputDebugStringA
#include "..\TextureConfigFactory.h" // Incluye la factoría para registrarse

HRESULT VoxelBasicTextureConfig::Init(
    ID3D11Device* device,
    ShaderManager* shaderManger,
    ID3D11SamplerState** outSamplerState,
    ID3D11Buffer** outMatrixBuffer
) {
    HRESULT hr = S_OK;

    // Asegúrate de que los punteros de salida sean válidos
    if (!device || !outSamplerState || !outMatrixBuffer) {
        OutputDebugStringA("Error: Parámetros nulos en VoxelBasicInitializer::Init.\n");
        return E_INVALIDARG;
    }

    // Libera los recursos existentes si ya estaban asignados (prevención de fugas)
    if (*outSamplerState) { (*outSamplerState)->Release(); *outSamplerState = nullptr; }
    if (*outMatrixBuffer) { (*outMatrixBuffer)->Release(); *outMatrixBuffer = nullptr; }

    // --- Descripción del Sampler State ---
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
    hr = device->CreateSamplerState(&samplerDesc, outSamplerState);
    if (FAILED(hr)) {
        OutputDebugStringA("Error al crear el Sampler State para VOXEL_BASIC.\n");
        return hr;
    }

    // Crear el buffer de constantes para las matrices
    D3D11_BUFFER_DESC matrixBufferDesc;
    ZeroMemory(&matrixBufferDesc, sizeof(matrixBufferDesc));
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    hr = device->CreateBuffer(&matrixBufferDesc, NULL, outMatrixBuffer);
    if (FAILED(hr)) {
        OutputDebugStringA("Error al crear el Matrix Buffer para VOXEL_BASIC.\n");
        // Si falla el buffer, liberar el samplerState que ya se creó
        if (*outSamplerState) { (*outSamplerState)->Release(); *outSamplerState = nullptr; }
        return hr;
    }

    return S_OK;
}

// Objeto estático global para registrar la clase automáticamente al inicio del programa
namespace { // Usar un namespace anónimo para limitar la visibilidad    
    static bool registeredVoxelBasic = TextureConfigFactory::Register(
        VOXEL_BASIC,
        []() { return std::make_unique<VoxelBasicTextureConfig>(); }
    );
}