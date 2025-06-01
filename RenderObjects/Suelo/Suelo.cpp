#include "Suelo.h"

Suelo::Suelo(Material* material) : m_material(material), m_vertexBuffer(nullptr), m_indexBuffer(nullptr), m_index{0,1,2,2,1,3}, m_vertices{
		{{-1250.0f, 0.0f, -1250.0f}, {1.0f, 0.0f, 0.0f, 1.0f}}, // Rojo
		{{-1250.0f, 0.0f,  1250.0f}, {0.0f, 1.0f, 0.0f, 1.0f}}, // Verde
		{{ 1250.0f, 0.0f, -1250.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}, // Azul
		{{ 1250.0f, 0.0f,  1250.0f}, {1.0f, 1.0f, 0.0f, 1.0f}}  // Amarillo
} {}

Suelo::~Suelo() {
	Release();
}

HRESULT Suelo::Init(ID3D11Device* device) {
	// Crear descripción del buffer
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(m_vertices);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_BUFFER_DESC indexBufferDesc = {};
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(UINT) * ARRAYSIZE(m_index);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	// Definir datos de inicialización
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = m_vertices;
	initData.SysMemPitch = 0;
	initData.SysMemSlicePitch = 0;

	// Crear buffer en GPU
	HRESULT hr = device->CreateBuffer(&bufferDesc, &initData, &m_vertexBuffer);
	if (FAILED(hr)) return hr;

	// Crear buffer de índice
	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = m_index;

	// Crear buffer de índice en GPU
	hr = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(hr)) return hr;

	SetPosition(XMFLOAT3(0.0f, 0.0f, 500.0f)); // Establecer posición inicial

	return S_OK;
}

void Suelo::Render(ID3D11DeviceContext* context) {
	// Aplicar shaders y estados del material
	m_material->Apply(context);

	// Configurar el Input Layout (si no lo hace el material)
	//context->IASetInputLayout(m_material->GetInputLayout());

	// Renderizar el suelo
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	context->Draw(ARRAYSIZE(m_vertices), 0);
}


void Suelo::Release() {
	if (m_vertexBuffer) m_vertexBuffer->Release();
	m_vertexBuffer = nullptr;
	if (m_indexBuffer) m_indexBuffer->Release();
	m_indexBuffer = nullptr;
}