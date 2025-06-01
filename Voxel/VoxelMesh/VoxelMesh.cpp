// VoxelMesh.cpp
#include "VoxelMesh.h"
#include <iostream>

VoxelMesh::VoxelMesh(DeviceManager* deviceManager, Material* material) :
    m_vertexBuffer(nullptr),
    m_indexBuffer(nullptr),
    m_vertexCount(0),
    m_indexCount(0),
    m_material(material),
	m_deviceManager(deviceManager),
	m_chunkKey(0, 0, 0) // Inicializar chunkKey a (0, 0, 0)
{
}

VoxelMesh::~VoxelMesh() {
    Release();
}

// Implementación de Init que toma MarchingCubesMesh
HRESULT VoxelMesh::Init(ID3D11Device* device, const MarchingCubesMesh& mcMesh) {
    HRESULT hr = S_OK;

    // Liberar buffers existentes si esta función es llamada para actualizar la malla
    if (m_vertexBuffer) {
        m_vertexBuffer->Release();
        m_vertexBuffer = nullptr;
    }
    if (m_indexBuffer) {
        m_indexBuffer->Release();
        m_indexBuffer = nullptr;
    }

    m_vertexCount = static_cast<int>(mcMesh.vertices.size());
    m_indexCount = static_cast<int>(mcMesh.indices.size());

    // Si no hay vértices o índices, no hay nada que crear
    if (m_vertexCount == 0 || m_indexCount == 0) {
        return S_OK; // No se crean buffers si la malla está vacía
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

    // 1. Establecer los parámetros del shader (matrices) a través del material
    m_material->SetShaderParameters(context, worldMatrix, viewMatrix, projectionMatrix);

    // 2. Establecer el material (shaders e input layout)
    m_material->Apply(context);

    // 3. Configurar el Input Assembler (IA)
    UINT stride = sizeof(MarchingCubesVertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // 4. Establecer el buffer de índices
    context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0); // DXGI_FORMAT_R32_UINT para unsigned int

    // 5. Establecer la topología de primitivas (triángulos)
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 6. Dibujar los triángulos usando DrawIndexed
    //m_deviceManager->InitRasterizedState();
    context->DrawIndexed(m_indexCount, 0, 0); // Dibuja usando los índices
    //m_deviceManager->ResetContextState();
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
    // El material no se libera aquí
}