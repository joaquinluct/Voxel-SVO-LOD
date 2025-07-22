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
    return S_OK;
}

HRESULT MeshAsset::InitTexture() {
    std::string shaderAssetName = m_meshConfig->shader;
    std::string meshObj = m_meshConfig->mesh_path;
    std::string textureAssetName = m_meshConfig->texture;

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

    HRESULT hr = InitD3D11ResourcesVertex(m_deviceManager->GetDevice(), vertex, indexes);

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

    ID3D11DeviceContext* context = m_deviceManager->GetContext();
    m_deviceManager->SetRasterizerState();

    m_material->Render();

    UINT stride = m_vertexTypeSize;
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
 	context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->DrawIndexed(m_indexCount, 0, 0);
}

HRESULT MeshAsset::CreateVertexBuffer(std::shared_ptr<ID3D11Device> pDevice, const std::vector<std::shared_ptr<VertexDefinition::VertexVariant>> vertex) {
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

//HRESULT CreateVertexBuffer(ID3D11Device* pDevice, const std::vector<std::shared_ptr<VertexDefinition::VertexVariant>> vertex) {
//    if (vertex.empty()) {
//        std::cerr << "MeshAsset::CreateBuffers: El vector de variantes está vacío.\n";
//        return E_INVALIDARG;
//    }
//
//    // --- 1. Determinar el Stride y el ByteWidth total ---
//    // Necesitas el stride (tamaño de un solo vértice)
//    // std::visit es útil aquí para obtener el tamaño del primer elemento.
//    // Asumimos que todos los vértices en el vector son del mismo tipo concreto para el stride.
//    m_vertexTypeSize = 0;
//    std::visit([&](auto& currentVertex) {
//        // currentVertex es el tipo concreto (SimpleVertex, TextureBasicVertex, etc.)
//        // Necesitamos que tus structs de vértice tengan un método GetSize()
//        // o un sizeof() para obtener su tamaño.
//        m_vertexTypeSize = static_cast<UINT>(sizeof(&currentVertex));
//        // O si tus structs concretos tienen un GetSize() virtual:
//        // m_vertexStride = currentVertex.GetSize(); // ¡Solo si el variant contiene un puntero!
//        // Si el variant contiene el objeto por valor, sizeof(currentVertex) es la opción.
//
//        // Si el variant contiene punteros a IVertex, como en tu DefineLocator anterior:
//        // std::shared_ptr<IVertex> ptr_to_base_vertex = currentVertex; // Si el variant contiene ptrs
//        // m_vertexStride = static_cast<UINT>(ptr_to_base_vertex->GetSize());
//
//        // **IMPORTANTE**: Asegúrate de que el tamaño sea el correcto para TU struct de vértice.
//        // Para la mayoría de los casos de uso con std::variant, el variant contiene el objeto por valor.
//        // Así que 'sizeof(currentVertex)' suele ser lo correcto.
//        }, *vertex[0]); // Visita el primer elemento para obtener el stride
//
//    if (m_vertexTypeSize == 0) {
//        std::cerr << "MeshAsset::CreateBuffers: No se pudo determinar el stride del vértice.\n";
//        return E_FAIL;
//    }
//
//    UINT totalByteWidth = m_vertexTypeSize * static_cast<UINT>(vertex.size());
//
//    // --- 2. Crear un Buffer Contiguo de Datos Raw ---
//    std::vector<uint8_t> rawVertexData(totalByteWidth);
//
//    // Copiar los datos de cada vértice a este buffer contiguo
//    size_t currentOffset = 0;
//    for (const auto& v_shared_ptr : vertex) {
//        std::visit([&](auto& currentVertex) {
//            // 'currentVertex' es el objeto de vértice concreto (ej., SimpleVertex)
//            // Usamos GetRawData() para obtener un puntero a sus datos.
//            const void* source_data = currentVertex.GetRawData(); // Asumo que GetRawData() existe en tus structs de vértice
//
//            // Copia los bytes del vértice actual al buffer contiguo
//            std::memcpy(rawVertexData.data() + currentOffset, &currentVertex, m_vertexTypeSize);
//            currentOffset += m_vertexTypeSize;
//            }, *v_shared_ptr); // Desreferencia el shared_ptr para acceder al VertexVariant
//    }
//
//    // --- 3. Rellenar D3D11_BUFFER_DESC y D3D11_SUBRESOURCE_DATA ---
//    D3D11_BUFFER_DESC vbDesc = {};
//    vbDesc.Usage = D3D11_USAGE_DEFAULT;
//    vbDesc.ByteWidth = totalByteWidth; // ¡Este es el tamaño total correcto!
//    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//    vbDesc.CPUAccessFlags = 0;
//    vbDesc.MiscFlags = 0;
//    vbDesc.StructureByteStride = 0; // Correcto para un vertex buffer normal
//
//    D3D11_SUBRESOURCE_DATA vbInitData = {};
//    vbInitData.pSysMem = rawVertexData.data(); // ¡Aquí apuntas al inicio del buffer contiguo!
//    vbInitData.SysMemPitch = 0;       // No aplicable para vertex buffers
//    vbInitData.SysMemSlicePitch = 0;  // No aplicable para vertex buffers
//
//    // --- 4. Crear el Vertex Buffer de DirectX ---
//    HRESULT hr = pDevice->CreateBuffer(&vbDesc, &vbInitData, &m_vertexBuffer);
//    if (FAILED(hr)) {
//        std::cerr << "Error al crear el Vertex Buffer: " << std::hex << hr << std::endl;
//        return hr;
//    }
//
//    m_vertexCount = static_cast<UINT>(vertex.size());
//    return hr;
//}