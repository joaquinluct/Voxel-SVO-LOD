#include "Line.h"

Line::Line(Material* material, const XMFLOAT3& start, const XMFLOAT3& end, const XMFLOAT4& color)
    : m_material(material), m_vertexBuffer(nullptr), m_numVertices(2) {
    m_vertices[0] = { start, color };
    m_vertices[1] = { end, color };
}

Line::~Line() {
    Release();
}

void Line::SetPosition(const XMFLOAT3& start, const XMFLOAT3& end) {
    m_vertices[0] = { start, m_vertices[0].Color };
    m_vertices[1] = { end, m_vertices[1].Color};
}

HRESULT Line::InitManagers() {
    m_deviceManager = ManagerLocator::GetDeviceManager();
    if (!m_deviceManager) {
        return E_FAIL;
    }
	m_device = m_deviceManager->GetDevice();
    if (!m_device) {
        return E_FAIL;
	}
	m_context = m_deviceManager->GetContext();
    if (!m_context) {
        return E_FAIL;
	}
    m_cameraManager = ManagerLocator::GetCameraManager();
    if (!m_cameraManager) {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT Line::Init() {

	HRESULT hr = InitManagers();
	if (FAILED(hr)) return hr;

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
    hr = m_device->CreateBuffer(&bufferDesc, &initData, &m_vertexBuffer);
    if (FAILED(hr)) return hr;


    return S_OK;
}

void Line::Render() {
    // Aplicar shaders y estados del material
    //m_material->Apply(m_context);

	/*XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = m_cameraManager->GetCurrentViewMatrix();
    XMMATRIX projectionMatrix = m_cameraManager->GetCurrentProjectionMatrix();*/
	
	//m_material->SetShaderParameters<Vertex>(context, worldMatrix, viewMatrix, projectionMatrix);

    // Configurar el Input Layout (si no lo hace el material)
    //context->IASetInputLayout(m_material->GetInputLayout());

    // Renderizar la línea
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    m_context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    m_context->Draw(m_numVertices, 0);
}

void Line::Release() {
    if (m_vertexBuffer) m_vertexBuffer->Release();
    m_vertexBuffer = nullptr;
}