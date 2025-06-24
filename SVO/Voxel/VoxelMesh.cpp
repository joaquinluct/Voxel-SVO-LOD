// VoxelMesh.cpp
#include "VoxelMesh.h"
#include <iostream>

VoxelMesh::VoxelMesh(DeviceManager* deviceManager, Material* material) :
    m_vertexBuffer(nullptr),
    m_indexBuffer(nullptr),
    m_vertexCount(0),
    m_indexCount(0),
    m_material(material),
	m_deviceManager(deviceManager)
	//m_chunkKey(0, 0, 0) // Inicializar chunkKey a (0, 0, 0)
{
}

VoxelMesh::~VoxelMesh() {
    Release();
}

// Implementaci�n de Init que toma MarchingCubesMesh
HRESULT VoxelMesh::Init(ID3D11Device* device, const MarchingCubesMesh& mcMesh) {
    HRESULT hr = S_OK;

    // Liberar buffers existentes si esta funci�n es llamada para actualizar la malla
    if (m_vertexBuffer) {
        m_vertexBuffer->Release();
        m_vertexBuffer = nullptr;
    }
    if (m_indexBuffer) {
        m_indexBuffer->Release();
        m_indexBuffer = nullptr;
    }

    // Inicializar conteos de vértices e índices
    m_vertexCount = static_cast<int>(mcMesh.vertices.size());
    m_indexCount = static_cast<int>(mcMesh.indices.size());

    // Si no hay v�rtices o �ndices, no hay nada que crear
    if (m_vertexCount == 0 || m_indexCount == 0) {
        return S_OK; // No se crean buffers si la malla est� vac�a
    }

    // ** 1. Crear Vertex Buffer **
    D3D11_BUFFER_DESC vertexBufferDesc;
    D3D11_SUBRESOURCE_DATA vertexData;
    ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
    ZeroMemory(&vertexData, sizeof(vertexData));

    //vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(MarchingCubesVertex) * m_vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    //vertexData.pSysMem = mcMesh.vertices.data();
    vertexData.pSysMem = mcMesh.vertices.data();
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
    if (FAILED(hr)) {
        std::cerr << "Failed to create VoxelMesh vertex buffer. HRESULT: " << hr << std::endl;
        return hr;
    }

    // ** 2. Crear Index Buffer **
    D3D11_BUFFER_DESC indexBufferDesc;
    D3D11_SUBRESOURCE_DATA indexData;
    ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
    ZeroMemory(&indexData, sizeof(indexData));

    // indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned int) * m_indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    indexData.pSysMem = mcMesh.indices.data();

    hr = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
    if (FAILED(hr)) {
        std::cerr << "Failed to create VoxelMesh index buffer. HRESULT: " << hr << std::endl;
        if (m_vertexBuffer) {
            m_vertexBuffer->Release();
            m_vertexBuffer = nullptr;
        }
        return hr;
    }

    return hr;
}

bool VoxelMesh::isValidMesh() {
	return m_vertexBuffer != nullptr && m_indexBuffer != nullptr && m_material != nullptr && m_indexCount > 0;
}

void VoxelMesh::Render(ID3D11DeviceContext* context, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix) {
    if (!isValidMesh()) {
        return;
    }

    // Asegura el estado de rasterizado correcto antes de dibujar
    m_deviceManager->SetRasterizerState();

    // 1. Establecer los par�metros del shader (matrices) a trav�s del material
    m_material->SetShaderParameters(context, worldMatrix, viewMatrix, projectionMatrix);

    // 2. Establecer el material (shaders e input layout)
    m_material->Apply(context);

    // 3. Configurar el Input Assembler (IA)
    UINT stride = sizeof(MarchingCubesVertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // 4. Establecer el buffer de �ndices
    context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0); // DXGI_FORMAT_R32_UINT para unsigned int

    // 5. Establecer la topolog�a de primitivas (tri�ngulos)
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 6. Dibujar los tri�ngulos usando DrawIndexed
    //m_deviceManager->InitRasterizedState();
    context->DrawIndexed(m_indexCount, 0, 0); // Dibuja usando los �ndices
    //m_deviceManager->ResetContextState();
}

std::vector<Util::Triangle> VoxelMesh::GetTriangles(std::vector<unsigned int> indices)  {
    std::vector<Util::Triangle> triangles;

    // Verificar que la malla sea válida
    if (!isValidMesh()) {
        return triangles; // Devolver un vector vacío si la malla no es válida
    }

    // Obtener un puntero a los vértices
    MarchingCubesVertex* vertices = nullptr;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = m_deviceManager->GetContext()->Map(m_vertexBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);
    if (FAILED(hr)) {
        std::cerr << "Error al mapear el vertex buffer para lectura.\n";
        return triangles;
    }
    vertices = static_cast<MarchingCubesVertex*>(mappedResource.pData);

    // Iterar a través de los índices, de tres en tres (cada tres índices forman un triángulo)
    for (size_t i = 0; i < m_indexCount; i += 3) {
        // Asegurarse de no exceder el tamaño del vector de índices
        if (i + 2 >= m_indexCount) {
            break; // Salir del bucle si no hay suficientes índices para formar un triángulo
        }

        // Obtener los índices de los vértices del triángulo
        unsigned int index0 = indices[i];
        unsigned int index1 = indices[i + 1];
        unsigned int index2 = indices[i + 2];

        // Asegurarse de que los índices son válidos
        if (index0 >= m_vertexCount || index1 >= m_vertexCount || index2 >= m_vertexCount) {
            std::cerr << "Índice fuera de rango: " << index0 << ", " << index1 << ", " << index2 << std::endl;
            continue; // Saltar este triángulo si algún índice está fuera de rango
        }

        // Crear el triángulo usando los vértices correspondientes
        Util::Triangle* triangle = new Util::Triangle(vertices[index0].Position, vertices[index1].Position, vertices[index2].Position);

        // Añadir el triángulo al vector
        triangles.push_back(*triangle);
    }

    // Desmapear el vertex buffer
    m_deviceManager->GetContext()->Unmap(m_vertexBuffer, 0);

    return triangles;
}

void VoxelMesh::Release() {
    if (m_vertexBuffer) {
        m_vertexBuffer->Release();
        m_vertexBuffer = nullptr;
    }
    if (m_indexBuffer) {
        m_indexBuffer->Release();
        m_indexBuffer = nullptr;
    }
    // El material no se libera aqu�
}