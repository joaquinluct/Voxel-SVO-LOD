// MeshAsset.cpp
#include "MeshAsset.h"
#include <type_traits>
#include <DDSTextureLoader.h> // Para CreateDDSTextureFromFile
#include <windows.h> // Para OutputDebugStringA
#include <Assets/Base/VertexAsset.h>
#include <Assets/Base/ShaderAsset.h>
#include <AssetLocator/AssetLocator.h>
#include <DefineLocator/DefineLocator.h>
#include <ManagerLocator/ManagerLocator.h>
#include <REGISTER_ASSET_MACRO.h>
#include <Util/Text/Text.h>
#include <Assets/Base/TextureAsset.h>
//#define TINYOBJLOADER_IMPLEMENTATION
#include <TinyObjLoader/tiny_obj_loader.h>
#include <Assets/Base/ObjFormat/ObjUtil.h>
#include <Defines/Mesh.h>
#include <Defines/Vector.h>

REGISTER_ASSET_TYPE(MeshAsset, "MeshAsset")

MeshAsset::MeshAsset()
    : m_meshConfig(nullptr)
    , m_indexBuffer(nullptr)
    , m_vertexBuffer(nullptr)
    , m_vertexCount(0)
    , m_deviceManager(nullptr)
    , m_cameraManager(nullptr)
    , m_shaderManager(nullptr)
    , m_material(nullptr)
	, m_textureAsset(nullptr)
{}

MeshAsset::~MeshAsset() {}

HRESULT MeshAsset::InitManagers() {
    m_deviceManager = ManagerLocator::GetDeviceManager();
    if (!m_deviceManager) {
        OutputDebugStringA("MeshAsset::Init - ERROR: DeviceManager not found.\n");
        return E_FAIL;
    }
    m_cameraManager = ManagerLocator::GetCameraManager();
    if (!m_cameraManager) {
        OutputDebugStringA("MeshAsset::Init - ERROR: CameraManager not found.\n");
        return E_FAIL;
    }
    m_shaderManager = ManagerLocator::GetShaderManager();
    if (!m_shaderManager) {
        OutputDebugStringA("MeshAsset::Init - ERROR: ShaderManager not found.\n");
        return E_FAIL;
    }	
	m_uiManager = ManagerLocator::GetManager<UIManager>();
    if (!m_uiManager) {
        OutputDebugStringA("MeshAsset::Init - ERROR: UIManager not found.\n");
        return E_FAIL;
	}
    return S_OK;
}

HRESULT MeshAsset::InitConfig() {
    // Obtener el vertexDefinition del shader
    m_name = m_meshConfig->name;
    m_shaderAssetName = m_meshConfig->shader;
    m_meshType = static_cast<Mesh::Type>(m_meshConfig->meshType);

    m_shaderAsset = AssetLocator::GetShaderAsset(m_shaderAssetName);
    if (!m_shaderAsset) {
        OutputDebugStringA(("MeshAsset::Init - ERROR: Failed to get ShaderAsset for mesh '" + m_name + "'.\n").c_str());
        return E_FAIL;
    }

    m_vertexDef = m_shaderAsset->GetConfig()->vertex_def;
    if (m_vertexDef.empty()) {
        OutputDebugStringA(("MeshAsset::Init - ERROR: No vertex definition found for mesh '" + m_name + "'.\n").c_str());
        return E_FAIL;
	}
    return S_OK;
}

HRESULT MeshAsset::InitTexture() {
    std::string textureAssetName = m_meshConfig->texture;
    
    m_material = new Material();
    if (!m_material) {
        OutputDebugStringA(("MeshAsset::Init - ERROR: Failed to create Material resource for mesh '" + m_name + "'.\n").c_str());
        return E_FAIL;
    }

    m_material->SetShaderName(StringToWstring(m_shaderAssetName));
    
    HRESULT hr = m_material->Init();
    if (FAILED(hr)) {
        OutputDebugStringA(("MeshAsset::Init - ERROR: Failed to initialize Material resource for mesh '" + m_name + "'.\n").c_str());
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

HRESULT MeshAsset::InitShadows()
{
    bool castShadows = m_meshConfig->cast_shadows;
    if (!castShadows) {
	    return S_OK;
    }
	m_shadowMaterial = new Material();
    m_shadowMaterial->SetShaderName(StringToWstring(m_meshConfig->shader_shadows));
    HRESULT hr = m_shadowMaterial->Init();
    if (FAILED(hr)) {
        OutputDebugStringA("MeshAsset::Init - ERROR: Failed to initialize Material resource for shadows.\n");
        return E_FAIL;
    }
	return hr;
}

HRESULT MeshAsset::InitMesh() {

    // Crear el array de vértices e índices
    std::vector<uint16_t> indexes = {};
    HRESULT hr = S_OK;
   auto vertex = DefineLocator::GetVertexDefinitionAsVector(m_vertexDef);

	// PROCESASR EL ARCHIVO OBJ O TEXTO
    if (m_meshType == Mesh::Type::File_Obj) {
        std::string meshObj = m_meshConfig->mesh_path;
        hr = ObjUtil::LoadObj(meshObj, vertex, indexes) ? S_OK : E_FAIL;
        if (vertex.empty()) {
            OutputDebugStringA(("MeshAsset::Init - ERROR: No vertex data found in mesh file '" + meshObj + "'.\n").c_str());
            return E_FAIL;
        }
	}
    // PROCESAR TIPO TEXTO
    else if (m_meshType == Mesh::Type::Text) {
        //UIText* text = m_uiManager->InitText(std::make_shared<MeshAsset>(*this), vertex);
        m_uiText = new UIText(std::make_shared<MeshAsset>(*this));
        m_uiText->CreateMesh(vertex);
        if (vertex.empty()) {
            OutputDebugStringA(("MeshAsset::Init - ERROR: No vertex data found in mesh file '" + m_name + "'.\n").c_str());
            return E_FAIL;
        }
    }
    // PROCESAR TIPO TERRENO
    else if (m_meshType == Mesh::Type::Terrain) {
        return S_OK;
    }

    // Crear el vetex & index buffer
    hr = InitD3D11ResourcesVertex(m_deviceManager->GetDevice().Get(), vertex, indexes);

    if (FAILED(hr)) {
        OutputDebugStringA(("MeshAsset::Init - ERROR: Failed to initialize D3D11 resources for mesh '" + m_name + "'.\n").c_str());
        return hr;
    }

    return hr;
}

void MeshAsset::UpdateTextMesh(UIText* uiText, std::string text) {
    std::vector<uint16_t> indexes = {};
    auto vertex = DefineLocator::GetVertexDefinitionAsVector(m_vertexDef);
    uiText->CreateMesh(vertex);

    // Crear el vetex & index buffer
    HRESULT hr = InitD3D11ResourcesVertex(m_deviceManager->GetDevice().Get(), vertex, indexes);

    if (FAILED(hr)) {
        OutputDebugStringA(("MeshAsset::Init - ERROR: Failed to initialize D3D11 resources for mesh '" + m_name + "'.\n").c_str());
        return;
    }

    return;
}

HRESULT MeshAsset::Init() {
    if (m_meshConfig == nullptr) {
        return S_OK;
    }

	m_name = m_meshConfig->name;

    HRESULT hr = InitManagers();
    if (FAILED(hr)) {
        OutputDebugStringA("MeshAsset::Init - ERROR: Managers init.\n");
        return E_FAIL;
    }

    hr = InitConfig();
    if (FAILED(hr)) {
        OutputDebugStringA("MeshAsset::Init - ERROR: Config init.\n");
        return E_FAIL;
	}

    hr = InitTexture();
    if (FAILED(hr)) {
        OutputDebugStringA("MeshAsset::Init - ERROR: Texture init.\n");
        return E_FAIL;
    }

	hr = InitShadows();
    if (FAILED(hr)) {
        OutputDebugStringA("MeshAsset::Init - ERROR: Shadows init.\n");
        return E_FAIL;
	}

    hr = InitMesh();
    if (FAILED(hr)) {
        OutputDebugStringA("MeshAsset::Init - ERROR: Mesh init.\n");
        return E_FAIL;
    }

    return hr;
}

void MeshAsset::Shutdown() {
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
    if (m_uiText) {        
		SafeShutDown(m_uiText);
	}
}

void MeshAsset::Render() {

 //   Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = m_deviceManager->GetContext();

 //   //if (m_renderManager->IsRenderColourPassActive()) {
 //       m_material->Render();
 //   //}

 //   UINT stride = m_vertexTypeSize;
 //   UINT offset = 0;
	//ID3D11Buffer* vertexBuffers[] = { m_vertexBuffer.Get() };
 //   context->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);
 //	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
 //   context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
 //   context->DrawIndexed(m_indexCount, 0, 0);
}

HRESULT MeshAsset::CreateVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, const std::vector<std::shared_ptr<VertexDefinition::VertexVariant>> vertices) {
    if (m_vertexBuffer) {
        m_vertexBuffer->Release();
        //m_vertexBuffer = nullptr;
    }
    m_vertexCount = 0;
    m_vertexTypeSize = 0; // Se inicializa aquí para asegurar que siempre está limpia

    // --- 1. Validar parámetros de entrada ---
    if (!pDevice || vertices.empty()) {
        std::cerr << "CreateVertexBuffer: Parámetros de entrada inválidos o vector de vértices vacío.\n";
        return E_INVALIDARG;
    }

    // --- 2. Determinar el tamaño del tipo de vértice (Stride) ---
    // Asumimos que todos los vértices en el vector son del mismo tipo concreto
    // para este búfer.
    std::visit([&](auto& currentVertex) {
        using T = std::decay_t<decltype(currentVertex)>;
        //if inline constexpr (is_vector_v<T>) {
            m_vertexTypeSize = currentVertex.Size();
        //}
        }, * vertices[0]); // Visita el primer elemento del vector de variants

    if (m_vertexTypeSize == 0) {
        std::cerr << "CreateVertexBuffer: No se pudo determinar el tamaño del tipo de vértice (es 0).\n";
        return E_FAIL;
    }

    // --- 3. Calcular el tamaño total del búfer de vértices ---
    m_vertexCount = static_cast<UINT>(vertices.size());
    UINT totalByteWidth = m_vertexTypeSize * m_vertexCount;

    // --- 4. Crear un búfer contiguo de datos raw en la CPU ---
    // Este vector temporal almacenará todos los datos de tus vértices
    // en un formato lineal, listo para la GPU.
    std::vector<uint8_t> rawVertexData(totalByteWidth);

    // --- 5. Iterar sobre cada VertexVariant y copiar sus datos raw al búfer contiguo ---
    size_t currentOffset = 0;
    for (const auto& vertex : vertices) {
        if (!vertex) {
            std::cerr << "CreateVertexBuffer: Se encontró un shared_ptr nulo en el vector de variants.\n";
            // Limpiar recursos antes de salir por error
            if (m_vertexBuffer) { m_vertexBuffer->Release(); m_vertexBuffer = nullptr; }
            m_vertexCount = 0;
            m_vertexTypeSize = 0;
            return E_INVALIDARG;
        }

        // Usamos std::visit para acceder al tipo concreto dentro del std::variant.
        std::visit([&](auto& currentVertex) {
            // 'currentVertex' es una referencia al objeto concreto (ej., SimpleVertex).
            // Llamamos a GetRawData() en esa instancia para obtener el puntero a sus datos.
            const void* source_data = currentVertex.GetRawData();
            std::memcpy(rawVertexData.data() + currentOffset, source_data, m_vertexTypeSize);
            // Copiamos los bytes de la instancia actual al búfer contiguo.
            currentOffset += m_vertexTypeSize;
            }, * vertex); // Desreferencia el shared_ptr para acceder al VertexVariant por valor
    }

    // --- 6. Configurar D3D11_BUFFER_DESC ---
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;         // Uso predeterminado para datos estáticos
    vbDesc.ByteWidth = totalByteWidth;           // Tamaño total del búfer en bytes
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Se usará como búfer de vértices
    vbDesc.CPUAccessFlags = 0;                   // No necesita acceso de CPU después de la creación
    vbDesc.MiscFlags = 0;                        // Sin banderas misceláneas
    vbDesc.StructureByteStride = 0;              // No es un Structured Buffer

    // --- 7. Configurar D3D11_SUBRESOURCE_DATA ---
    D3D11_SUBRESOURCE_DATA vbInitData = {};
    vbInitData.pSysMem = rawVertexData.data();   // ¡Aquí apuntamos al inicio del búfer contiguo!
    vbInitData.SysMemPitch = 0;                  // No aplicable para vertex buffers
    vbInitData.SysMemSlicePitch = 0;             // No aplicable para vertex buffers

    // --- 8. Crear el ID3D11Buffer ---
    HRESULT hr = pDevice->CreateBuffer(&vbDesc, &vbInitData, &m_vertexBuffer);
    if (FAILED(hr)) {
        std::cerr << "CreateVertexBuffer: Error al crear el ID3D11Buffer: " << std::hex << hr << std::endl;
        // Limpiar variables de estado en caso de fallo
        m_vertexCount = 0;
        m_vertexTypeSize = 0;
        if (m_vertexBuffer) {
            m_vertexBuffer->Release();
            m_vertexBuffer = nullptr;
        }
    }

    return hr;
}