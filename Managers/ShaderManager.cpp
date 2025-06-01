#include "ShaderManager.h"
#include <d3dcompiler.h> // Necesario para la compilación de shaders
#include <d3d11.h>       // Asegúrate de incluirlo aquí también
#include "../Resources/resource.h"


ShaderManager::ShaderManager() {
	// Inicializar mapas
	vertexShaders.clear();
	pixelShaders.clear();
	inputLayouts.clear();
	vertexShaderBlobs.clear();
	pixelShaderBlobs.clear();
}

ShaderManager::~ShaderManager() {
	Release();
}

HRESULT ShaderManager::LoadShader(ID3D11Device* device, std::wstring shaderName, std::wstring vsPath, std::wstring psPath, D3D11_INPUT_ELEMENT_DESC layoutDesc[], UINT numElements) {
    // Verificar si el shader ya está cargado
    if (vertexShaders.find(shaderName) != vertexShaders.end()) {
        return S_OK;
    }

    ID3DBlob* vsBlob = nullptr;
    ID3DBlob* psBlob = nullptr;
    ID3DBlob* errorBlob = nullptr;
    ID3D11VertexShader* vs = nullptr;
    ID3D11PixelShader* ps = nullptr;
    ID3D11InputLayout* layout = nullptr;

    // Compilar Vertex Shader
    HRESULT hr = D3DCompileFromFile(vsPath.c_str(), nullptr, nullptr, "VSMain", "vs_5_0", 0, 0, &vsBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        return hr;
    }

    // Compilar Pixel Shader
    hr = D3DCompileFromFile(psPath.c_str(), nullptr, nullptr, "PSMain", "ps_5_0", 0, 0, &psBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob) {
            OutputDebugStringA((char*)errorBlob->GetBufferPointer());
            errorBlob->Release();
        }
        vsBlob->Release();
        return hr;
    }

    // Crear Vertex Shader
    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vs);
    if (FAILED(hr)) {
        vsBlob->Release();
        psBlob->Release();
        return hr;
    }

    // Crear Pixel Shader
    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &ps);
    if (FAILED(hr)) {
        vsBlob->Release();
        psBlob->Release();
        vs->Release();
        return hr;
    }

    // Crear Input Layout
    hr = device->CreateInputLayout(layoutDesc, numElements, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &layout);
    if (FAILED(hr)) {
        vsBlob->Release();
        psBlob->Release();
        vs->Release();
        ps->Release();
        return hr;
    }

    // Guardar shaders y layout en los mapas
    vertexShaders[shaderName] = vs;
    pixelShaders[shaderName] = ps;
    inputLayouts[shaderName] = layout;
    vertexShaderBlobs[shaderName] = vsBlob; // Guardar el blob del VS
    pixelShaderBlobs[shaderName] = psBlob;   // Guardar el blob del PS

    // Los blobs ahora se liberan al destruir el ShaderManager o cuando se recargan los shaders.
    // No los liberamos aquí para poder usarlos para crear el Input Layout si es necesario.

    return S_OK;
}

ID3D11VertexShader* ShaderManager::GetVertexShader(std::wstring shaderName) {
    auto it = vertexShaders.find(shaderName);
    return (it != vertexShaders.end()) ? it->second : nullptr;
}

ID3D11PixelShader* ShaderManager::GetPixelShader(std::wstring shaderName) {
    auto it = pixelShaders.find(shaderName);
    return (it != pixelShaders.end()) ? it->second : nullptr;
}

ID3D11InputLayout* ShaderManager::GetInputLayout(std::wstring shaderName) {
    auto it = inputLayouts.find(shaderName);
    return (it != inputLayouts.end()) ? it->second : nullptr;
}

ID3DBlob* ShaderManager::GetVertexShaderBytecode(std::wstring shaderName) {
    auto it = vertexShaderBlobs.find(shaderName);
    return (it != vertexShaderBlobs.end()) ? it->second : nullptr;
}

UINT ShaderManager::GetVertexShaderBytecodeLength(std::wstring shaderName) {
    auto it = vertexShaderBlobs.find(shaderName);
    if (it != vertexShaderBlobs.end() && it->second != nullptr) {
        return static_cast<UINT>(it->second->GetBufferSize());
    }
    return 0; // Or another appropriate value to indicate "not found" or error
}

HRESULT ShaderManager::Init(DeviceManager* device) {
	// Aquí puedes inicializar cualquier recurso adicional que necesites
    // Cargar shaders


    // SHADER 1: SUELO
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0} // Ajuste correcto de offset
    };
	UINT numElements = ARRAYSIZE(layoutDesc);
    HRESULT result = LoadShader(device->GetDevice(),
        SHADER_BASE,
        L"Resources/Shaders/BasicShader.hlsl",
		L"Resources/Shaders/BasicShader.hlsl", layoutDesc, numElements);

	// SHADER 2: VOXEL
    D3D11_INPUT_ELEMENT_DESC layoutVoxelDesc[] = {
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}, // Nueva entrada para la normal
    {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0} // Color va después de la normal
    };
    UINT numVoxelElements = ARRAYSIZE(layoutVoxelDesc);
    result = LoadShader(device->GetDevice(),
        SHADER_VOXEL,
        L"Resources/Shaders/VoxelShader.hlsl", // Ruta del Vertex Shader
        L"Resources/Shaders/VoxelShader.hlsl", // Ruta del Pixel Shader (asumiendo el mismo archivo)
        layoutVoxelDesc, numVoxelElements);

	// SHADER 3: TEXT
    D3D11_INPUT_ELEMENT_DESC layoutText[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
	UINT numElementsText = ARRAYSIZE(layoutText);
    result = LoadShader(device->GetDevice(),
        SHADER_TEXT,
        L"Resources/Shaders/TextShader.hlsl",
        L"Resources/Shaders/TextShader.hlsl", layoutText, numElementsText);

    if (FAILED(result)) {
        OutputDebugStringA("Error en la carga de shaders.\n");
        return result;
    }
	return result;
}

HRESULT ShaderManager::Init() {
	// Aquí puedes inicializar cualquier recurso adicional que necesites
	return S_OK;
}

HRESULT ShaderManager::Release() {
	// Liberar todos los shaders y layouts
	for (auto& pair : vertexShaders) {
		if (pair.second) pair.second->Release();
	}
	for (auto& pair : pixelShaders) {
		if (pair.second) pair.second->Release();
	}
	for (auto& pair : inputLayouts) {
		if (pair.second) pair.second->Release();
	}
	for (auto& pair : vertexShaderBlobs) {
		if (pair.second) pair.second->Release();
	}
	for (auto& pair : pixelShaderBlobs) {
		if (pair.second) pair.second->Release();
	}

	vertexShaders.clear();
	pixelShaders.clear();
	inputLayouts.clear();
	vertexShaderBlobs.clear();
	pixelShaderBlobs.clear();

	return S_OK;
}