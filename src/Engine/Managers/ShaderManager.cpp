#include "ShaderManager.h"
#include "REGISTER_MANAGER_MACRO.h"
#include <d3dcompiler.h>
#include <d3d11.h>
#include <vector>
#include <variant>
#include "../Resources/resource.h"
#include <ManagerLocator/ManagerLocator.h>
#include <AssetLocator/AssetLocator.h>
#include <DefineLocator/DefineLocator.h>
#include <Assets/Base/ShaderAsset.h>
#include <Util/Text/Text.h>
#include <Defines/MatrixDefinitionBase.h>

REGISTER_MANAGER_TYPE(ShaderManager, "ShaderManager")

ShaderManager::ShaderManager() {
	// Inicializar mapas
	vertexShaders.clear();
	pixelShaders.clear();
	inputLayouts.clear();
	vertexShaderBlobs.clear();
	pixelShaderBlobs.clear();
    matrixShaders.clear();
}

ShaderManager::~ShaderManager() {}

HRESULT ShaderManager::LoadShader(Microsoft::WRL::ComPtr<ID3D11Device> device, std::wstring shaderName, std::wstring vsPath, std::wstring psPath, D3D11_INPUT_ELEMENT_DESC layoutDesc[], UINT numElements) {
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

Microsoft::WRL::ComPtr<ID3D11VertexShader> ShaderManager::GetVertexShader(std::wstring shaderName) {
    auto it = vertexShaders.find(shaderName);
    return (it != vertexShaders.end()) ? it->second : nullptr;
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> ShaderManager::GetPixelShader(std::wstring shaderName) {
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

std::map<std::string, std::pair<int, std::unique_ptr<MatrixDefinition::AnyMatrixBuffer>>>& ShaderManager::GetMatrixBuffers(std::wstring shaderName) {
    auto it = matrixShaders.find(shaderName);
    if (it == matrixShaders.end()) {
        std::string msg = "Error: No se encontraron buffers de matrices para el shader: " + WstringToString(shaderName) + ".\n";
        OutputDebugStringA(msg.c_str());
        // Lanzar una excepción es la forma idiomática de indicar un fallo en este caso
        throw std::runtime_error(msg);
    }
    return it->second; // Devuelve una referencia constante al mapa interno
}

void ShaderManager::SetConstantsBuffers(std::wstring shaderName, const MatrixDefinitionBase::MatrixParams& matrixParams, std::map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>>& constantBuffers, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) {

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    // Accedemos al mapa de matrices específico para este shader
    std::map<std::string, std::pair<int, std::unique_ptr<MatrixDefinition::AnyMatrixBuffer>>>& matrices = matrixShaders[shaderName];

    // Iteramos sobre cada tipo de buffer de constante que este shader requiere
    for (const auto& [matrixName, matrix] : matrices) {
        // Buscamos el ID3D11Buffer correspondiente en nuestro mapa de buffers globales
        auto bufferComPtrIt = constantBuffers.find(matrixName);
        if (bufferComPtrIt == constantBuffers.end()) {
            // Si no se encuentra el buffer (por ejemplo, no se creó durante la inicialización),
            // lo saltamos y continuamos con el siguiente.
            continue;
        }

        // 'nSlot' es el registro (ej. b0, b1, b2) que el shader espera para este buffer
        int nSlot = matrix.first;
        // Obtenemos el puntero raw del ComPtr para usarlo con los métodos de DirectX
        ID3D11Buffer* pBuffer = bufferComPtrIt->second.Get();

        // Mapeamos el buffer de constante para escritura.
        // D3D11_MAP_WRITE_DISCARD es eficiente si el buffer se actualiza cada frame.
        HRESULT hr = context->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        if (FAILED(hr)) {
            // Manejar error de mapeo, quizás con un log o assert.
            continue;
        }

        std::string matrixType = "";

        // Usamos std::visit para aplicar el método SetMatrixData correcto
        // a la estructura de buffer de constante específica (MatrixBufferType, DirectionalLight, etc.).
        std::visit([&](auto& currentMatrixStruct) {
            // Llamamos a SetMatrixData para rellenar la estructura con los datos actuales
            // de MatrixParams. Cada estructura sabe qué datos de MatrixParams necesita.
            currentMatrixStruct.SetMatrixData(matrixParams);
            matrixType = currentMatrixStruct.MatrixType();
            size_t size = currentMatrixStruct.Size();
            //size_t size = sizeof(currentMatrixStruct);
            // Copiamos los datos de nuestra estructura C++ a la memoria mapeada de la GPU.
            // Asegúrate de que el tamaño de la estructura coincida con el tamaño del buffer en la GPU.
            //memcpy(mapped.pData, &currentMatrixStruct, sizeof(std::decay_t<decltype(currentMatrixStruct)>));
            memcpy(mapped.pData, &currentMatrixStruct, size);
            }, *matrix.second); // Accedemos al contenido del unique_ptr<AnyMatrixBuffer>

        // Desmapeamos el buffer para que la GPU pueda acceder a los datos actualizados.
        context->Unmap(pBuffer, 0);

        // --- ENLACE DE LOS CONSTANT BUFFERS A LOS SHADERS ---
        // Aquí decidimos a qué estadio del pipeline se enlaza cada buffer.
        // Lo más común es:
        // - Matrices de transformación (World, View, Projection): Vertex Shader.
        // - Datos de cámara, luz, material: Pixel Shader (para cálculos de iluminación).

        if (matrixType == MATRIX_TYPE_VERTEX.data() || matrixType == MATRIX_TYPE_MIXED.data()) {
            // Estos buffers contienen matrices de transformación que suelen usarse en el Vertex Shader.
            context->VSSetConstantBuffers(nSlot, 1, &pBuffer);
        }

        if (matrixType == MATRIX_TYPE_PIXEL.data() || matrixType == MATRIX_TYPE_MIXED.data()) {
            // Estos buffers contienen datos que son cruciales para los cálculos de iluminación
            // y propiedades de superficie, que se realizan en el Pixel Shader.
            context->PSSetConstantBuffers(nSlot, 1, &pBuffer);
        }
        // Nota: Si un buffer como "CameraData" también se necesitara en el Vertex Shader (ej. para billboarding),
        // podrías añadir otra línea: context->VSSetConstantBuffers(nSlot, 1, &pBuffer);
    }
}

HRESULT ShaderManager::Init() {
	OutputDebugStringA("Inicializando ShaderManager...\n");

    deviceManager = ManagerLocator::GetManager<DeviceManager>();
    if (!deviceManager) {
        OutputDebugStringA("Error: DeviceManager no encontrado.\n");
        return E_FAIL;
	}

    std::vector<std::shared_ptr<ShaderAsset>> shaders = AssetLocator::GetShaders();
    if (shaders.empty()) {
        OutputDebugStringA("No se encontraron shaders para cargar.\n");
        return S_OK; // No hay shaders que cargar, pero no es un error.
	}

    for(const auto& shader : shaders) {
        OutputDebugStringA(("** Shader: " + shader->GetStaticAssetName() + ".\n").c_str());
        std::shared_ptr<IAssetShaderConfig> config =  shader->GetConfig();
        if (!config || config->vertex_def.empty()) {
            OutputDebugStringA("Error: Configuración del shader no encontrada.\n");
            continue; // Saltar este shader si no tiene configuración
		}

        std::string vertexDef = config->vertex_def;
		std::vector<std::string> matrixDef = config->matrix_slots;

        //std::unique_ptr<VertexDefinition::VertexVariant> vertex = VertexDefinition::Factory().createVertex(vertexDef);
        std::shared_ptr<IVertex> vertex = DefineLocator::GetVertexDefine(vertexDef);
        if (!vertex) {
            OutputDebugStringA("Error: Configuración del shader: Definición de vértices no encontrada.\n");
            continue;
        }

        unsigned int numItems = 0;
        //D3D11_INPUT_ELEMENT_DESC* layout;// = vertex->GetInputLayout(numItems);
        D3D11_INPUT_ELEMENT_DESC* layout = vertex->GetInputLayout(numItems);

        if (!layout) {
            OutputDebugStringA("Error: Layout de entrada no encontrado.\n");
            continue; // Saltar este shader si no tiene layout
        }

		std::wstring name = StringToWstring(shader->GetAssetName());
		std::wstring vsPath = StringToWstring(config->shader_path);

        HRESULT result = LoadShader(deviceManager->GetDevice(), name, vsPath, vsPath, layout, numItems);

        if (SUCCEEDED(result)) {
            int idx = 0;
            for (const std::string& matrixName : matrixDef) {
    			MatrixDefinition::AnyMatrixBuffer matrix = MatrixDefinition::Get(matrixName);
                const std::string matrixSlotName = ParseInt(idx) + matrixName;
                matrixShaders[name][matrixSlotName] = { idx, (std::make_unique<MatrixDefinition::AnyMatrixBuffer>(std::move(matrix))) };
				idx++;
			}
		}
	}

    return S_OK;
}

void ShaderManager::Shutdown() {
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
}