#include "Triangulo.h"

Triangulo::Triangulo(Material* material) : m_vertexBuffer(nullptr), m_material(material) {}

HRESULT Triangulo::Init(std::shared_ptr<ID3D11Device> device) {
    // Definir vértices
    vertices = {
        { 0.0f, 0.5f, 0.0f },  // Vértice superior
        { -0.5f, -0.5f, 0.0f }, // Vértice inferior izquierdo
        { 0.5f, -0.5f, 0.0f }   // Vértice inferior derecho
    };

    // Crear descripción del buffer
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = static_cast<UINT>(sizeof(XMFLOAT3) * vertices.size());
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;

    // Definir datos de inicialización
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices.data();

    // Crear buffer en GPU
    HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &m_vertexBuffer);
    if (FAILED(hr)) return hr;

    return S_OK;
}

void Triangulo::Render(ID3D11DeviceContext* context) {
    // Aplicar shaders
    m_material->Apply(context);

    // Renderizar el triángulo
    UINT stride = sizeof(XMFLOAT3);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    context->Draw(static_cast<UINT>(vertices.size()), 0);
}

void Triangulo::Release() {
    if (m_vertexBuffer) m_vertexBuffer->Release();
    m_vertexBuffer = nullptr;
}
