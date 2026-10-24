// MeshAsset.cpp
// #define TINYOBJLOADER_IMPLEMENTATION
// #include <TinyObjLoader/tiny_obj_loader.h>

#include "MeshAsset.h"

#include <Assets/Base/ObjFormat/ObjUtil.h>
#include <DefineLocator/DefineLocator.h>
#include <Defines/Mesh.h>
#include <ManagerLocator/ManagerLocator.h>
#include <REGISTER_ASSET_MACRO.h>
#include <Windows.h>
#include <cstdint>
#include <debugapi.h>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

REGISTER_ASSET_TYPE(MeshAsset, "MeshAsset")

MeshAsset::MeshAsset()
{
}

MeshAsset::~MeshAsset()
{
}

HRESULT MeshAsset::InitMesh()
{
    OutputDebugStringA("[MeshAsset::InitMesh] ? Iniciando carga de geometría\n");

    // Crear el array de v?rtices e ?ndices
    std::vector<uint16_t> indexes = {};
    HRESULT hr = S_OK;
    
    OutputDebugStringA(("[MeshAsset::InitMesh] ? m_vertexDef = '" + m_vertexDef + "'\n").c_str());
    OutputDebugStringA(("[MeshAsset::InitMesh] ? m_meshType = " + std::to_string(static_cast<int>(m_meshType)) + "\n").c_str());
    
    auto vertex = DefineLocator::GetVertexDefinitionAsVector(m_vertexDef);
    OutputDebugStringA(("[MeshAsset::InitMesh] ? DefineLocator returned " + std::to_string(vertex.size()) + " vertices (should be 0 before loading)\n").c_str());

    // PROCESASR EL ARCHIVO OBJ O TEXTO
    if (m_meshType == Mesh::Type::File_Obj)
    {
        if (!m_meshConfig) {
            OutputDebugStringA("[MeshAsset::InitMesh] ? ERROR: m_meshConfig is NULL\n");
            return E_FAIL;
        }
        
        std::string meshObj = m_meshConfig->mesh_path;
        OutputDebugStringA(("[MeshAsset::InitMesh] ? Cargando OBJ desde: " + meshObj + "\n").c_str());
        
        bool loadResult = ObjUtil::LoadObj(meshObj, vertex, indexes);
        OutputDebugStringA(("[MeshAsset::InitMesh] ? ObjUtil::LoadObj() returned: " + std::string(loadResult ? "TRUE" : "FALSE") + "\n").c_str());
        
        hr = loadResult ? S_OK : E_FAIL;
        
        if (vertex.empty())
        {
            OutputDebugStringA(
                ("MeshAsset::InitMesh - ? ERROR: No vertex data found in mesh file '" + meshObj + "'.\n").c_str());
            return E_FAIL;
        }
        OutputDebugStringA(("[MeshAsset::InitMesh] ? OBJ cargado: " + std::to_string(vertex.size()) + 
                            " vertices, " + std::to_string(indexes.size()) + " indices\n").c_str());
    }
    // PROCESAR TIPO TEXTO
    else if (m_meshType == Mesh::Type::Text)
    {
        OutputDebugStringA("[MeshAsset::InitMesh] ? Procesando tipo Text\n");
        m_uiText = new UIText(std::make_shared<MeshAsset>(*this));
        m_uiText->CreateMesh(vertex);
        if (vertex.empty())
        {
            OutputDebugStringA(
                ("MeshAsset::InitMesh - ? ERROR: No vertex data found in mesh file '" + m_name + "'.\n").c_str());
            return E_FAIL;
        }
    }
    // PROCESAR TIPO TERRENO
    else if (m_meshType == Mesh::Type::Terrain)
    {
        OutputDebugStringA("[MeshAsset::InitMesh] ? Tipo Terrain - skipping geometry load\n");
        return S_OK;
    }
    else
    {
        OutputDebugStringA(("[MeshAsset::InitMesh] ?? WARNING: Unknown mesh type: " + 
                            std::to_string(static_cast<int>(m_meshType)) + "\n").c_str());
    }

    // Crear el vetex & index buffer
    OutputDebugStringA("[MeshAsset::InitMesh] ? Creando D3D11 buffers\n");
    hr = InitD3D11ResourcesVertex(m_deviceManager->GetDevice().Get(), vertex, indexes);

    if (FAILED(hr))
    {
        OutputDebugStringA(
            ("MeshAsset::InitMesh - ? ERROR: Failed to initialize D3D11 resources for mesh '" + m_name + "'.\n").c_str());
        return hr;
    }

    // CRÍTICO: Hacer swap de buffers para que los datos sean visibles en readIndex
    OutputDebugStringA("[MeshAsset::InitMesh] ? Ejecutando SwapBuffer() para hacer visibles los datos\n");
    SwapBuffer();
    
    // Verificar que el swap funcionó
    int newReadIndex = GetReadIndex();
    int newWriteIndex = GetWriteIndex();
    UINT readVertexCount = GetVertexCount(newReadIndex);
    UINT readIndexCount = GetIndexCount(newReadIndex);
    OutputDebugStringA(("[MeshAsset::InitMesh] ? Buffer swap completed: readIndex=" + std::to_string(newReadIndex) + 
                        ", writeIndex=" + std::to_string(newWriteIndex) + 
                        ", vertices=" + std::to_string(readVertexCount) + 
                        ", indices=" + std::to_string(readIndexCount) + "\n").c_str());

    OutputDebugStringA("[MeshAsset::InitMesh] ? D3D11 resources created successfully\n");
    return hr;
}

void MeshAsset::UpdateTextMesh(UIText *uiText, std::string text)
{
    std::vector<uint16_t> indexes = {};
    auto vertex = DefineLocator::GetVertexDefinitionAsVector(m_vertexDef);
    uiText->CreateMesh(vertex);

    // Crear el vetex & index buffer
    HRESULT hr = InitD3D11ResourcesVertex(m_deviceManager->GetDevice(), vertex, indexes);

    if (FAILED(hr))
    {
        OutputDebugStringA(
            ("MeshAsset::Init - ERROR: Failed to initialize D3D11 resources for mesh '" + m_name + "'.\n").c_str());
        return;
    }

    return;
}

HRESULT MeshAsset::Init()
{
    OutputDebugStringA("[MeshAsset::Init] ? Iniciando inicialización\n");
    
    // First, initialize managers even if m_meshConfig is null
    if (m_deviceManager == nullptr)
    {
        OutputDebugStringA("[MeshAsset::Init] ? Obteniendo DeviceManager\n");
        m_deviceManager = ManagerLocator::GetDeviceManager();
        if (!m_deviceManager)
        {
            OutputDebugStringA("[MeshAsset::Init] ? ERROR: DeviceManager not found.\n");
            return E_FAIL;
        }
    }

    OutputDebugStringA("[MeshAsset::Init] ? Llamando MeshAssetBase::Init()\n");
    HRESULT hr = MeshAssetBase::Init();
    if (FAILED(hr))
    {
        OutputDebugStringA("[MeshAsset::Init] ? ERROR: MeshAssetBase initialization error.\n");
        return E_FAIL;
    }

    OutputDebugStringA(("[MeshAsset::Init] ? m_name = '" + m_name + "'\n").c_str());
    OutputDebugStringA(("[MeshAsset::Init] ? m_meshConfig = " + std::string(m_meshConfig ? "SET" : "NULL") + "\n").c_str());

    if (m_name.empty())
    {
        OutputDebugStringA("[MeshAsset::Init] ?? WARNING: m_name is empty, skipping InitMesh()\n");
        return S_OK;
    }

    OutputDebugStringA("[MeshAsset::Init] ? Llamando InitMesh() para cargar geometría\n");
    hr = InitMesh();
    if (FAILED(hr))
    {
        OutputDebugStringA("[MeshAsset::Init] ? ERROR: Mesh init failed.\n");
        return E_FAIL;
    }

    OutputDebugStringA("[MeshAsset::Init] ? Inicialización completada exitosamente\n");
    return hr;
}

void MeshAsset::Shutdown()
{
    MeshAssetBase::Shutdown();
    if (m_uiText)
    {
        SafeShutDown(m_uiText);
    }
}

void MeshAsset::Render()
{

    //   Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = m_deviceManager->GetContext();

    //   //if (m_renderManager->IsRenderColourPassActive()) {
    //       m_material->Render();
    //   //}

    //   UINT stride = m_vertexTypeSize;
    //   UINT offset = 0;
    // ID3D11Buffer* vertexBuffers[] = { m_vertexBuffer.Get() };
    //   context->IASetVertexBuffers(0, 1, vertexBuffers, &stride, &offset);
    //	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    //   context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //   context->DrawIndexed(m_indexCount, 0, 0);
}

HRESULT MeshAsset::CreateVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice,
                                      const std::vector<std::shared_ptr<VertexDefinition::VertexVariant>> vertices)
{
    int index = m_writeIndex.load();
    if (m_vertexBuffer.size() && m_vertexBuffer[index])
    {
        m_vertexBuffer[index]->Release();
        // m_vertexBuffer = nullptr;
    }
    m_vertexCount[index] = 0;
    m_vertexTypeSize = 0; // Se inicializa aquí para asegurar que siempre está limpia

    // --- 1. Validar parámetros de entrada ---
    if (!pDevice || vertices.empty())
    {
        std::cerr << "CreateVertexBuffer: Parámetros de entrada inválidos o vector de vértices vacío.\n";
        return E_INVALIDARG;
    }

    // --- 2. Determinar el tamaño del tipo de vértice (Stride) ---
    // Asumimos que todos los vértices en el vector son del mismo tipo concreto
    // para este búfer.
    std::visit(
        [&](auto &currentVertex) {
            using T = std::decay_t<decltype(currentVertex)>;
            // if inline constexpr (is_vector_v<T>) {
            m_vertexTypeSize = currentVertex.Size();
            //}
        },
        *vertices[0]); // Visita el primer elemento del vector de variants

    if (m_vertexTypeSize == 0)
    {
        std::cerr << "CreateVertexBuffer: No se pudo determinar el tamaño del tipo de vértice (es 0).\n";
        return E_FAIL;
    }

    // --- 3. Calcular el tamaño total del búfer de vértices ---
    m_vertexCount[index] = static_cast<UINT>(vertices.size());
    UINT totalByteWidth = m_vertexTypeSize * m_vertexCount[index];

    // --- 4. Crear un búfer contiguo de datos raw en la CPU ---
    // Este vector temporal almacenará todos los datos de tus vértices
    // en un formato lineal, listo para la GPU.
    std::vector<uint8_t> rawVertexData(totalByteWidth);

    // --- 5. Iterar sobre cada VertexVariant y copiar sus datos raw al búfer contiguo ---
    size_t currentOffset = 0;
    for (const auto &vertex : vertices)
    {
        if (!vertex)
        {
            std::cerr << "CreateVertexBuffer: Se encontró un shared_ptr nulo en el vector de variants.\n";
            // Limpiar recursos antes de salir por error
            if (m_vertexBuffer[index])
            {
                m_vertexBuffer[index]->Release();
                m_vertexBuffer[index] = nullptr;
            }
            m_vertexCount[index] = 0;
            m_vertexTypeSize = 0;
            return E_INVALIDARG;
        }

        // Usamos std::visit para acceder al tipo concreto dentro del std::variant.
        std::visit(
            [&](auto &currentVertex) {
                // 'currentVertex' es una referencia al objeto concreto (ej., SimpleVertex).
                // Llamamos a GetRawData() en esa instancia para obtener el puntero a sus datos.
                const void *source_data = currentVertex.GetRawData();
                std::memcpy(rawVertexData.data() + currentOffset, source_data, m_vertexTypeSize);
                // Copiamos los bytes de la instancia actual al búfer contiguo.
                currentOffset += m_vertexTypeSize;
            },
            *vertex); // Desreferencia el shared_ptr para acceder al VertexVariant por valor
    }

    // --- 6. Configurar D3D11_BUFFER_DESC ---
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;          // Uso predeterminado para datos estáticos
    vbDesc.ByteWidth = totalByteWidth;           // Tamaño total del búfer en bytes
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // Se usará como búfer de vértices
    vbDesc.CPUAccessFlags = 0;                   // No necesita acceso de CPU después de la creación
    vbDesc.MiscFlags = 0;                        // Sin banderas misceláneas
    vbDesc.StructureByteStride = 0;              // No es un Structured Buffer

    // --- 7. Configurar D3D11_SUBRESOURCE_DATA ---
    D3D11_SUBRESOURCE_DATA vbInitData = {};
    vbInitData.pSysMem = rawVertexData.data(); // ¡Aquí apuntamos al inicio del búfer contiguo!
    vbInitData.SysMemPitch = 0;                // No aplicable para vertex buffers
    vbInitData.SysMemSlicePitch = 0;           // No aplicable para vertex buffers

    // --- 8. Crear el ID3D11Buffer ---
    HRESULT hr = pDevice->CreateBuffer(&vbDesc, &vbInitData, &m_vertexBuffer[index]);
    if (FAILED(hr))
    {
        std::cerr << "CreateVertexBuffer: Error al crear el ID3D11Buffer: " << std::hex << hr << std::endl;
        // Limpiar variables de estado en caso de fallo
        m_vertexCount[index] = 0;
        m_vertexTypeSize = 0;
        if (m_vertexBuffer[index])
        {
            m_vertexBuffer[index]->Release();
            m_vertexBuffer[index] = nullptr;
        }
    }

    return hr;
}

HRESULT MeshAsset::CreateIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, const std::vector<uint16_t> &indexes)
{
    int index = m_writeIndex.load();
    // Crear Vertex Buffer
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = sizeof(WORD) * static_cast<uint16_t>(indexes.size());
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = 0;
    ibDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA ibInitData = {};
    ibInitData.pSysMem = indexes.data();
    HRESULT hr = pDevice->CreateBuffer(&ibDesc, &ibInitData, &m_indexBuffer[index]);
    if (FAILED(hr))
    {
        OutputDebugStringA(("ERROR: Failed to create index buffer for mesh '" + m_meshConfig->name +
                            "'. HRESULT: " + std::to_string(hr) + "\n")
                               .c_str());
        // Liberar vertex buffer si el index buffer falla
        // m_vertexBuffer.Reset();
        m_vertexBuffer[index]->Release();
        return hr;
    }
    m_indexCount[index] = static_cast<UINT>(indexes.size());
    return hr;
}
