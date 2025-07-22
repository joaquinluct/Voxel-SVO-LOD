// MeshAsset.cpp
#include "MeshAsset.h"
#include <string>
#include <DDSTextureLoader.h> // Para CreateDDSTextureFromFile
#include <windows.h> // Para OutputDebugStringA
#include <Mesh/SkyboxCube.h> 
#include <ManagerLocator/ManagerLocator.h>
#include <Text/Text.h>
#include "../../../../resources/resource.h"
#include <REGISTER_ASSET_MACRO.h>

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
	//m_meshAsset(nullptr)
    //m_axis(nullptr)
{
    // Constructor
}

MeshAsset::~MeshAsset() {}

//HRESULT MeshAsset::Init() {
//    m_deviceManager = ManagerLocator::GetDeviceManager();
//    if (!m_deviceManager) {
//        OutputDebugStringA("MeshAsset::Init - ERROR: DeviceManager not found.\n");
//        return E_FAIL;
//	}
//    return S_OK;
//}

HRESULT MeshAsset::Init() {
	m_shaderManager = ManagerLocator::GetManager<ShaderManager>();
    if (!m_shaderManager) {
        OutputDebugStringA("MeshAsset::Init - ERROR: ShaderManager not found.\n");
        return E_FAIL;
	}
    m_deviceManager = ManagerLocator::GetManager<DeviceManager>();
    if (!m_deviceManager) {
        OutputDebugStringA("MeshAsset::Init - ERROR: DeviceManager not found.\n");
        return E_FAIL;
    }
    m_cameraManager = ManagerLocator::GetManager<CameraManager>();
    if (!m_cameraManager) {
        OutputDebugStringA("MeshAsset::Init - ERROR: CameraManager not found.\n");
        return E_FAIL;
	}
    m_wMatrixManager = ManagerLocator::GetManager<WorldMatrixManager>();
    if (!m_wMatrixManager) {
        OutputDebugStringA("MeshAsset::Init - ERROR: WorldMatrixManager not found.\n");
		return E_FAIL;
	}

    m_deviceManager->InitRasterizedState();
    m_material = new Material(SHADER_SKYBOX);
	m_material->Init(m_deviceManager->GetDevice());
    Material* m_material2 = new Material(SHADER_BASE);
    m_material2->Init(m_deviceManager->GetDevice());
    
    RECT clientRect;
    GetClientRect(*m_deviceManager->GetHwnd(), &clientRect);
    float width = static_cast<float>(clientRect.right - clientRect.left);
    float height = static_cast<float>(clientRect.bottom - clientRect.top);
    float aspectRatio = width / height;

    return S_OK;
}

void MeshAsset::SetType(const std::string& type) {
   /* if (type == "SkyboxCube") {
        m_meshAsset = new SkyboxCube{};
        InitD3D11ResourcesSimpleVertex(m_deviceManager->GetDevice());
    }*/
}

void MeshAsset::Shutdown() {
    // ComPtr maneja la liberación. Solo para explicitar.
    //m_vertexBuffer.Reset();
    if (m_vertexBuffer) {
        m_vertexBuffer->Release();
        m_vertexBuffer = nullptr;
	}
    if (m_indexBuffer) {
        m_indexBuffer->Release();
    }
    //OutputDebugStringA(("MeshAsset '" + m_name + "' released.\n").c_str());
}

void MeshAsset::SetTexture(ID3D11ShaderResourceView* textureResource) {

	m_material->SetTexture(textureResource);

    ID3D11Device* device = m_deviceManager->GetDevice();
    ID3D11DeviceContext* context = m_deviceManager->GetContext();

    ID3D11SamplerState* m_samplerState{ nullptr };
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    device->CreateSamplerState(&sampDesc, &m_samplerState);

    context->PSSetShaderResources(0, 1, &textureResource);
    context->PSSetSamplers(0, 1, &m_samplerState);

    context->IASetInputLayout(m_inputLayout);

    XMMATRIX viewMatrix = m_cameraManager->GetCurrentViewMatrix();
    XMMATRIX proyectionMatrix = m_cameraManager->GetCurrentProjectionMatrix();

    ID3D11Buffer* buffer = m_wMatrixManager->GetMatrixBuffer();

    XMMATRIX viewProj = XMMatrixMultiply(viewMatrix, proyectionMatrix);
    context->UpdateSubresource(buffer, 0, nullptr, &viewProj, 0, 0);
    context->VSSetConstantBuffers(0, 1, &buffer);
}

void MeshAsset::SetTexture(const std::string& texture) {
    ID3D11ShaderResourceView* m_skyboxSRV = nullptr;
    SetTexture(m_skyboxSRV);
}

//template<typename TVertex>
//HRESULT MeshAsset::InitD3D11Resources(ID3D11Device* pDevice) {
//    if (!m_meshAsset) {
//        OutputDebugStringA("ERROR: MeshAsset is not set.\n");
//        return E_FAIL;
//    }
//    const std::vector<TVertex>& vertices = m_meshAsset->GetVertex();
//    const std::vector<WORD>& indices = m_meshAsset->GetIndex();
//
//    return InitD3D11Resources(pDevice, vertices, indices);
//}

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
            //SkyboxCubeConfig::Values<VertexDefinition::SimpleVertex>* config = new SkyboxCubeConfig::Values<VertexDefinition::SimpleVertex>();
            //config->SetVertex(SkyboxCube::GetVertex());
            //config->SetIndex(SkyboxCube::GetIndex());
            //InitD3D11Resources<VertexDefinition::SimpleVertex>(m_deviceManager->GetDevice(), config->GetVertex(), config->GetIndex());
        }
        //OutputDebugStringA(("ERROR: MeshAsset '" + m_name + "' has not been initialized properly.\n").c_str());
        //return;
    }

    m_deviceManager->SetRasterizerState();

    XMMATRIX viewMatrix = m_cameraManager->GetCurrentViewMatrix();
    XMMATRIX proyectionMatrix = m_cameraManager->GetCurrentProjectionMatrix();
    
    //ID3D11Buffer* buffer = m_wMatrixManager->GetMatrixBuffer();

    float scaleFactor = 20.0f;
    DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);
	XMMATRIX worldMatrix = XMMatrixIdentity() * scaleMatrix; // Aplicar escala al mundo
    //XMMATRIX viewProj = XMMatrixMultiply(viewMatrix, proyectionMatrix);

	//m_wMatrixManager->SetGlobalMatrices(XMMatrixIdentity(), viewMatrix, proyectionMatrix);
	m_material->SetContantsBuffers(context, worldMatrix, viewMatrix, proyectionMatrix);

	m_material->Apply(context);

    /*context->VSSetShader(m_vertexShader, nullptr, 0);
    context->PSSetShader(m_pixelShader, nullptr, 0);*/

    if (m_vertextType == "SimpleVertex") {
        UINT stride = sizeof(VertexDefinition::SkyboxVertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, GetVertexBuffer(), &stride, &offset);
		context->IASetIndexBuffer(GetIndexBuffer(), DXGI_FORMAT_R16_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->DrawIndexed(m_indexCount, 0, 0);
	}

    //m_axis->Render(context);
    
}