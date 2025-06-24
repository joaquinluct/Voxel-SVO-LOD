#include "Line.h"

Line::Line(Material* material, const XMFLOAT3& start, const XMFLOAT3& end, const XMFLOAT4& color)
    : m_material(material), m_vertexBuffer(nullptr), m_numVertices(2) {
    m_vertices[0] = { start, color };
    m_vertices[1] = { end, color };
}

Line::~Line() {
    Release();
}

HRESULT Line::Init(ID3D11Device* device) {
    // Crear descripción del buffer de vértices
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    bufferDesc.ByteWidth = sizeof(Vertex) * m_numVertices;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    assert(bufferDesc.ByteWidth >= sizeof(Vertex) * m_numVertices);

    // Definir datos de inicialización
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = m_vertices;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    // Crear buffer en GPU
    HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &m_vertexBuffer);
    if (FAILED(hr)) return hr;

    return S_OK;
}

void Line::Render(ID3D11DeviceContext* context) {
    // Aplicar shaders y estados del material
    m_material->Apply(context);

    // Configurar el Input Layout (si no lo hace el material)
    //context->IASetInputLayout(m_material->GetInputLayout());

    // Renderizar la línea
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    context->Draw(m_numVertices, 0);
}

void Line::Release() {
    if (m_vertexBuffer) m_vertexBuffer->Release();
    m_vertexBuffer = nullptr;
}