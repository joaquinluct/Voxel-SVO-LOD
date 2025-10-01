// MeshAsset.cpp
//#define TINYOBJLOADER_IMPLEMENTATION
//#include <TinyObjLoader/tiny_obj_loader.h>

#include "MeshAsset.h"
#include <Assets/Base/ObjFormat/ObjUtil.h>
#include <cstdint>
#include <debugapi.h>
#include <DefineLocator/DefineLocator.h>
#include <Defines/Mesh.h>
#include <iostream>
#include <REGISTER_ASSET_MACRO.h>
#include <string>
#include <type_traits>
#include <vector>
#include <Windows.h>

REGISTER_ASSET_TYPE(MeshAsset, "MeshAsset")

MeshAsset::MeshAsset() {}
MeshAsset::~MeshAsset() {}

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

    HRESULT hr = MeshAssetBase::Init();
    if (FAILED(hr)) {
        OutputDebugStringA("MeshAsset::Init - ERROR: MeshAssetBase initialization error.\n");
        return E_FAIL;
    }

    if (m_name.empty()) {
        return S_OK;
    }

    hr = InitMesh();
    if (FAILED(hr)) {
        OutputDebugStringA("MeshAsset::Init - ERROR: Mesh init.\n");
        return E_FAIL;
    }

    return hr;
}

void MeshAsset::Shutdown() {
    MeshAssetBase::Shutdown();
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
    int index = m_writeIndex.load();
    if (m_vertexBuffer[index]) {
        m_vertexBuffer[index]->Release();
        //m_vertexBuffer = nullptr;
    }
    m_vertexCount[index] = 0;
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
        }, *vertices[0]); // Visita el primer elemento del vector de variants

    if (m_vertexTypeSize == 0) {
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
    for (const auto& vertex : vertices) {
        if (!vertex) {
            std::cerr << "CreateVertexBuffer: Se encontró un shared_ptr nulo en el vector de variants.\n";
            // Limpiar recursos antes de salir por error
            if (m_vertexBuffer[index]) { m_vertexBuffer[index]->Release(); m_vertexBuffer[index] = nullptr; }
            m_vertexCount[index] = 0;
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
            }, *vertex); // Desreferencia el shared_ptr para acceder al VertexVariant por valor
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
    HRESULT hr = pDevice->CreateBuffer(&vbDesc, &vbInitData, &m_vertexBuffer[index]);
    if (FAILED(hr)) {
        std::cerr << "CreateVertexBuffer: Error al crear el ID3D11Buffer: " << std::hex << hr << std::endl;
        // Limpiar variables de estado en caso de fallo
        m_vertexCount[index] = 0;
        m_vertexTypeSize = 0;
        if (m_vertexBuffer[index]) {
            m_vertexBuffer[index]->Release();
            m_vertexBuffer[index] = nullptr;
        }
    }

    return hr;
}

HRESULT MeshAsset::CreateIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Device> pDevice, const std::vector<uint16_t>& indexes) {
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
    if (FAILED(hr)) {
        OutputDebugStringA(("ERROR: Failed to create index buffer for mesh '" + m_meshConfig->name + "'. HRESULT: " + std::to_string(hr) + "\n").c_str());
        // Liberar vertex buffer si el index buffer falla
        //m_vertexBuffer.Reset();
        m_vertexBuffer[index]->Release();
        return hr;
    }
    m_indexCount[index] = static_cast<UINT>(indexes.size());
    return hr;
}
