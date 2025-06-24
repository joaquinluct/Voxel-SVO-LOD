// MeshAsset.cpp
#include <string>
#include <DDSTextureLoader.h> // Para CreateDDSTextureFromFile
#include "MeshAsset.h"
#include <windows.h> // Para OutputDebugStringA
#include <REGISTER_ASSET_MACRO.h>
#include <Mesh/SkyboxCube.h> 
#include <ManagerLocator/ManagerLocator.h>
#include <Text/Text.h>
#include <../Resources/resource.h>

REGISTER_ASSET_TYPE(MeshAsset, "MeshAsset")

MeshAsset::MeshAsset() :
    m_vertexBuffer(nullptr),
    m_indexBuffer(nullptr),
    m_vertexCount(0),
    m_indexCount(0),
	m_config(nullptr),
	m_inputLayout(nullptr),
    m_material(nullptr),
    m_shaderManager(nullptr),
    m_deviceManager(nullptr),
    m_cameraManager(nullptr),
    m_wMatrixManager(nullptr),
    m_vertexShader(nullptr),
    m_pixelShader(nullptr)
    //m_axis(nullptr)
{
    // Constructor
}

MeshAsset::~MeshAsset() {
    Shutdown();
}

HRESULT MeshAsset::Init() {
	m_shaderManager = ManagerLocator::GetManager<ShaderManager>();
    m_deviceManager = ManagerLocator::GetManager<DeviceManager>();
    m_cameraManager = ManagerLocator::GetManager<CameraManager>();
    m_wMatrixManager = ManagerLocator::GetManager<WorldMatrixManager>();
	m_keyboardrManager = ManagerLocator::GetManager<KeyboardManager>();
    m_deviceManager->InitRasterizedState();
    m_material = new Material(SHADER_TEXTURE_BASE);
	m_material->Init(m_deviceManager->GetDevice());
    Material* m_material2 = new Material(SHADER_BASE);
    m_material2->Init(m_deviceManager->GetDevice());
    //m_axis = new Axis(m_material2);
	//m_axis->Init(m_deviceManager->GetDevice());


    RECT clientRect;
    GetClientRect(*m_deviceManager->GetHwnd(), &clientRect);
    float width = static_cast<float>(clientRect.right - clientRect.left);
    float height = static_cast<float>(clientRect.bottom - clientRect.top);
    float aspectRatio = width / height;

    const std::shared_ptr<ICamera> camera = m_cameraManager->GetCurrentCamera();

	camera->SetPosition(50.0f, 50.0f, 50.0f);
    camera->SetLookAt(0.0f, 0.0f, 0.0f); // Mira hacia el centro del mundo
	camera->SetProjectionParams(XM_PIDIV4, aspectRatio, 0.1f, 6000.0f);

	//m_cameraManager->AddCamera("FPC", m_camera);

    return S_OK;
}

void MeshAsset::SetType(const std::string& type) {
    if (type == "SkyboxCube") {
        m_vertextType = "SimpleVertex";
        SkyboxCubeConfig::Values<VertexDefinition::SimpleVertex>* config = new SkyboxCubeConfig::Values<VertexDefinition::SimpleVertex>();
        config->SetVertex(SkyboxCube::GetVertex());
        config->SetIndex(SkyboxCube::GetIndex());
        m_config = config;
		InitD3D11Resources<VertexDefinition::SimpleVertex>(m_deviceManager->GetDevice(), config->GetVertex(), config->GetIndex());
        std::wstring wShaderName(m_shaderName.begin(), m_shaderName.end());
        m_vertexShader = m_shaderManager->GetVertexShader(wShaderName);
        m_pixelShader = m_shaderManager->GetPixelShader(wShaderName);
	}
}

void MeshAsset::Shutdown() {
    // ComPtr maneja la liberación. Solo para explicitar.
    m_vertexBuffer.Reset();
    m_indexBuffer.Reset();
    //OutputDebugStringA(("MeshAsset '" + m_name + "' released.\n").c_str());
}

void MeshAsset::SetTexture(const std::string& texture) {
    ID3D11Device* device = m_deviceManager->GetDevice();
    ID3D11DeviceContext* context = m_deviceManager->GetContext();

    ID3D11ShaderResourceView* m_skyboxSRV = nullptr;
    std::wstring wTexture = std::wstring(texture.begin(), texture.end());
    HRESULT hr = DirectX::CreateDDSTextureFromFile(device, wTexture.c_str(), nullptr, &m_skyboxSRV);
    if (FAILED(hr)) {
        // Manejar error
    }

    ID3D11SamplerState* m_samplerState{nullptr};
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    device->CreateSamplerState(&sampDesc, &m_samplerState);

    context->PSSetShaderResources(0, 1, &m_skyboxSRV);
    context->PSSetSamplers(0, 1, &m_samplerState);

	context->IASetInputLayout(m_inputLayout);

    XMMATRIX viewMatrix = m_cameraManager->GetCurrentViewMatrix();
    XMMATRIX proyectionMatrix = m_cameraManager->GetCurrentProjectionMatrix();

	ID3D11Buffer* buffer = m_wMatrixManager->GetMatrixBuffer();
        
    XMMATRIX viewProj = XMMatrixMultiply(viewMatrix, proyectionMatrix);
    context->UpdateSubresource(buffer, 0, nullptr, &viewProj, 0, 0);
    context->VSSetConstantBuffers(0, 1, &buffer);

}

template<typename TVertex>
HRESULT MeshAsset::InitD3D11Resources(ID3D11Device* pDevice,
    const std::vector<TVertex>& vertices,
    const std::vector<WORD>& indices) {
    if (!pDevice) {
        OutputDebugStringA("ERROR: ID3D11Device is null when initializing mesh.\n");
        return E_INVALIDARG;
    }
    if (vertices.empty() || indices.empty()) {
        OutputDebugStringA(("ERROR: No vertex or index data provided for mesh '" + m_name + "'.\n").c_str());
        return E_INVALIDARG;
    }

    HRESULT hr = S_OK;

    // Crear Vertex Buffer
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.ByteWidth = sizeof(TVertex) * vertices.size();
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = 0;
    vbDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vbInitData = {};
    vbInitData.pSysMem = vertices.data();
    hr = pDevice->CreateBuffer(&vbDesc, &vbInitData, &m_vertexBuffer);
    if (FAILED(hr)) {
        OutputDebugStringA(("ERROR: Failed to create vertex buffer for mesh '" + m_name + "'. HRESULT: " + std::to_string(hr) + "\n").c_str());
        return hr;
    }
    m_vertexCount = vertices.size();

    // Crear Index Buffer
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DEFAULT;
    ibDesc.ByteWidth = sizeof(WORD) * indices.size();
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = 0;
    ibDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA ibInitData = {};
    ibInitData.pSysMem = indices.data();
    hr = pDevice->CreateBuffer(&ibDesc, &ibInitData, &m_indexBuffer);
    if (FAILED(hr)) {
        OutputDebugStringA(("ERROR: Failed to create index buffer for mesh '" + m_name + "'. HRESULT: " + std::to_string(hr) + "\n").c_str());
        // Liberar vertex buffer si el index buffer falla
        m_vertexBuffer.Reset();
        return hr;
    }
    m_indexCount = indices.size();

    //OutputDebugStringA(("MeshAsset '" + m_name + "' D3D11 resources initialized.\n").c_str());
    return S_OK;
}

void MeshAsset::PrepareViewMatrix(ID3D11DeviceContext* context) {
	//using namespace DirectX;
 //   XMMATRIX viewMatrix = m_cameraManager->GetCurrentViewMatrix();
 //   XMMATRIX view = XMLoadFloat4x4(&viewMatrix);
 //   view.r[3] = XMVectorSet(0, 0, 0, 1); // Eliminar traslación
 //   XMMATRIX proj = XMLoadFloat4x4(&projectionMatrix);
 //   XMMATRIX viewProj = XMMatrixMultiply(view, proj);
 //   context->UpdateSubresource(m_constantBuffer, 0, nullptr, &viewProj, 0, 0);
}

void MeshAsset::Render() {

	ID3D11DeviceContext* context = m_deviceManager->GetContext();
    
    if (!m_vertexBuffer || !m_indexBuffer || !context) {
        if (m_vertextType == "SimpleVertex") {
            SkyboxCubeConfig::Values<VertexDefinition::SimpleVertex>* config = new SkyboxCubeConfig::Values<VertexDefinition::SimpleVertex>();
            config->SetVertex(SkyboxCube::GetVertex());
            config->SetIndex(SkyboxCube::GetIndex());
            InitD3D11Resources<VertexDefinition::SimpleVertex>(m_deviceManager->GetDevice(), config->GetVertex(), config->GetIndex());
        }
        //OutputDebugStringA(("ERROR: MeshAsset '" + m_name + "' has not been initialized properly.\n").c_str());
        //return;
    }

	PrepareViewMatrix(context);

    m_deviceManager->SetRasterizerState();

    XMMATRIX viewMatrix = m_cameraManager->GetCurrentViewMatrix();
    XMMATRIX proyectionMatrix = m_cameraManager->GetCurrentProjectionMatrix();
    
    ID3D11Buffer* buffer = m_wMatrixManager->GetMatrixBuffer();

    XMMATRIX viewProj = XMMatrixMultiply(viewMatrix, proyectionMatrix);

	m_material->SetShaderParameters(context, XMMatrixIdentity(), viewMatrix, viewProj);
	m_material->Apply(context);

    /*context->VSSetShader(m_vertexShader, nullptr, 0);
    context->PSSetShader(m_pixelShader, nullptr, 0);*/

    if (m_vertextType == "SimpleVertex") {
        UINT stride = sizeof(VertexDefinition::SimpleVertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(GetIndexBuffer(), DXGI_FORMAT_R16_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->DrawIndexed(m_indexCount, 0, 0);
	}

    //m_axis->Render(context);
    
}