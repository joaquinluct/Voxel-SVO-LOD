// MeshAsset.cpp
#include "MeshAsset.h"
#include <string>
#include <DDSTextureLoader.h> // Para CreateDDSTextureFromFile
#include <windows.h> // Para OutputDebugStringA
#include <Assets/Base/VertexAsset.h>
#include <AssetLocator/AssetLocator.h>
#include <DefineLocator/DefineLocator.h>
#include <ManagerLocator/ManagerLocator.h>
#include <REGISTER_ASSET_MACRO.h>
#include <Util/Text/Text.h>
#include <Assets/Base/TextureAsset.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include <Assets/Base/ObjFormat/ObjUtil.h>

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
	m_renderManager = ManagerLocator::GetManager<RenderManager>();
    if (!m_renderManager) {
        OutputDebugStringA("MeshAsset::Init - ERROR: RenderManager not found.\n");
        return E_FAIL;
	}
    return S_OK;
}

HRESULT MeshAsset::InitTexture() {
    std::string shaderAssetName = m_meshConfig->shader;
    std::string meshObj = m_meshConfig->mesh_path;
    std::string textureAssetName = m_meshConfig->texture;
    bool castShadows = m_meshConfig->cast_shadows;

    m_material = new Material();
    if (!m_material) {
        OutputDebugStringA(("MeshAsset::Init - ERROR: Failed to create Material resource for mesh '" + meshObj + "'.\n").c_str());
        return E_FAIL;
    }

    m_material->SetShaderName(StringToWstring(shaderAssetName));
    
    HRESULT hr = m_material->Init();
    if (FAILED(hr)) {
        OutputDebugStringA(("MeshAsset::Init - ERROR: Failed to initialize Material resource for mesh '" + meshObj + "'.\n").c_str());
        return E_FAIL;
    }

    if (!textureAssetName.empty()) {
        m_textureAsset = AssetLocator::GetTextureAsset(textureAssetName);
        m_textureAsset->SetTextureView(m_material);
    }

    return hr;
}

HRESULT MeshAsset::InitMesh() {

    // Obtener el vertexDefinition del shader
    std::string meshObj = m_meshConfig->mesh_path;
    std::string shaderAssetName = m_meshConfig->shader;

    std::shared_ptr<ShaderAsset> shaderAsset = AssetLocator::GetShaderAsset(shaderAssetName);
    if (!shaderAsset) {
        OutputDebugStringA(("MeshAsset::Init - ERROR: Failed to get ShaderAsset for mesh '" + meshObj + "'.\n").c_str());
        return E_FAIL;
    }

    // Crear el array de vértices e índices
    std::string vertexDef = shaderAsset->GetConfig()->vertex_def;
    auto vertex = DefineLocator::GetVertexDefineAsVector(vertexDef);
    std::vector<uint16_t> indexes = {};

    // Cargar el .obj
    bool result = ObjUtil::LoadObj(meshObj, vertex, indexes);

    if (!result) {
        OutputDebugStringA(("MeshAsset::Init - ERROR: Failed to load mesh file '" + meshObj + "'.\n").c_str());
        return E_FAIL;
    }

    if (vertex.empty()) {
        OutputDebugStringA(("MeshAsset::Init - ERROR: No vertex data found in mesh file '" + meshObj + "'.\n").c_str());
        return E_FAIL;
    }

    HRESULT hr = InitD3D11ResourcesVertex(m_deviceManager->GetDevice().Get(), vertex, indexes);

    if (FAILED(hr)) {
        OutputDebugStringA(("MeshAsset::Init - ERROR: Failed to initialize D3D11 resources for mesh '" + meshObj + "'.\n").c_str());
        return hr;
    }

    return hr;
}

HRESULT MeshAsset::Init() {
    if (m_meshConfig == nullptr) {
        return S_OK;
    }

    HRESULT hr = InitManagers();
    if (FAILED(hr)) {
        OutputDebugStringA("MeshAsset::Init - ERROR: Managers init.\n");
        return E_FAIL;
    }

    hr = InitTexture();
    if (FAILED(hr)) {
        OutputDebugStringA("MeshAsset::Init - ERROR: Texture init.\n");
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
}

void MeshAsset::Render() {

    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = m_deviceManager->GetContext();

    if (m_renderManager->IsRenderColourPassActive()) {
        m_material->Render();
    }

    UINT stride = m_vertexTypeSize;
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
 	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->DrawIndexed(m_indexCount, 0, 0);
}

HRESULT MeshAsset::CreateVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, const std::vector<std::shared_ptr<VertexDefinition::VertexVariant>> vertex) {
    if (m_vertexBuffer) {
        m_vertexBuffer->Release();
        m_vertexBuffer = nullptr;
    }
    m_vertexCount = 0;
    m_vertexTypeSize = 0; // Se inicializa aquí para asegurar que siempre está limpia

    // --- 1. Validar parámetros de entrada ---
    if (!pDevice || vertex.empty()) {
        std::cerr << "CreateVertexBuffer: Parámetros de entrada inválidos o vector de vértices vacío.\n";
        return E_INVALIDARG;
    }

    // --- 2. Determinar el tamaño del tipo de vértice (Stride) ---
    // Asumimos que todos los vértices en el vector son del mismo tipo concreto
    // para este búfer.
    std::visit([&](auto& currentVertex) {
        // 'currentVertex' es la instancia del tipo de vértice concreto (ej., SimpleVertex).
        // Llamamos a GetSize() en esa instancia para obtener su tamaño.
		m_vertexTypeSize = currentVertex.Size(); // Asegúrate de que GetSize() esté implementado en tus structs de vértice
        }, *vertex[0]); // Visita el primer elemento del vector de variants

    if (m_vertexTypeSize == 0) {
        std::cerr << "CreateVertexBuffer: No se pudo determinar el tamaño del tipo de vértice (es 0).\n";
        return E_FAIL;
    }

    // --- 3. Calcular el tamaño total del búfer de vértices ---
    m_vertexCount = static_cast<UINT>(vertex.size());
    UINT totalByteWidth = m_vertexTypeSize * m_vertexCount;

    // --- 4. Crear un búfer contiguo de datos raw en la CPU ---
    // Este vector temporal almacenará todos los datos de tus vértices
    // en un formato lineal, listo para la GPU.
    std::vector<uint8_t> rawVertexData(totalByteWidth);

    // --- 5. Iterar sobre cada VertexVariant y copiar sus datos raw al búfer contiguo ---
    size_t currentOffset = 0;
    for (const auto& v_shared_ptr : vertex) {
        if (!v_shared_ptr) {
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

            // Copiamos los bytes de la instancia actual al búfer contiguo.
            std::memcpy(rawVertexData.data() + currentOffset, source_data, m_vertexTypeSize);
            currentOffset += m_vertexTypeSize;
            }, *v_shared_ptr); // Desreferencia el shared_ptr para acceder al VertexVariant por valor
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