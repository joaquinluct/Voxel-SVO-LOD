#include "TestBasicTexture.h"
#include <REGISTER_SERVICE_MACRO.h>
#include <ManagerLocator/ManagerLocator.h>
#include <d3dcompiler.h>

using namespace Microsoft::WRL;
using namespace DirectX;

REGISTER_SERVICE_TYPE(TestBasicTexture, "TestBasicTexture");

struct VertexBasic2 {
    XMFLOAT3 position;
};

TestBasicTexture::TestBasicTexture() : m_cameraManager(), m_deviceManager(), m_inputLayout(), m_matrixBuffer(), m_pixelShader(), m_vertexBuffer(), m_vertexShader(), m_viewMatrix() {}
TestBasicTexture::~TestBasicTexture() {}

HRESULT TestBasicTexture::Init() {
    m_cameraManager = ManagerLocator::GetManager<CameraManager>();
    m_deviceManager = ManagerLocator::GetManager<DeviceManager>();
    std::shared_ptr<ID3D11Device> device = m_deviceManager->GetDevice();

    m_deviceManager->SetRasterizerState();

    // 🎯 Triángulo simple
    VertexBasic2 vertices[] = {
        { XMFLOAT3(0.0f,  10.5f, 0.0f) },
        { XMFLOAT3(10.5f, -10.5f, 0.0f) },
        { XMFLOAT3(-10.5f, -10.5f, 0.0f) }
    };

    // Vertex Buffer
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.ByteWidth = sizeof(vertices);
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA vbData = {};
    vbData.pSysMem = vertices;

    HRESULT hr = device->CreateBuffer(&vbDesc, &vbData, &m_vertexBuffer);
    if (FAILED(hr)) return hr;

    D3D11_BUFFER_DESC cbd = {};
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.ByteWidth = sizeof(MatrixBufferType);
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    device->CreateBuffer(&cbd, nullptr, &m_matrixBuffer);

    // 🎯 Shaders embebidos (minimalistas)
    const char* vsCodeTest = R"(
        cbuffer MatrixBuffer : register(b0)
        {
            matrix world;
            matrix view;
            matrix projection;
        };

        struct VSInput {
            float3 pos : POSITION;
        };

        struct VSOutput {
            float4 pos : SV_POSITION;
            float4 color : COLOR;
        };

        VSOutput VSMain(VSInput input) {
            VSOutput output;
            float4 worldPos = mul(float4(input.pos, 1.0f), world);
            float4 viewPos = mul(worldPos, view);
            output.pos = mul(viewPos, projection);

            output.color = float4(worldPos.xyz / 300.0f + 0.5f, 1.0f);
            return output;
        }
    )";

    const char* psCodeTest = R"(
         float4 PSMain() : SV_TARGET {
            // return color;
            return float4(1, 0, 0, 1); // rojo
        }
    )";

    const char* vs = vsCodeTest;
    const char* ps = psCodeTest;


    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, psBlob, errorBlob;
    hr = D3DCompile(vs, strlen(vs), nullptr, nullptr, nullptr, "VSMain", "vs_4_0", 0, 0, &vsBlob, &errorBlob);
    if (errorBlob) {
        OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    if (FAILED(hr)) return hr;

    hr = D3DCompile(ps, strlen(ps), nullptr, nullptr, nullptr, "PSMain", "ps_4_0", 0, 0, &psBlob, &errorBlob);
    if (errorBlob) {
        OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    if (FAILED(hr)) return hr;

    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vertexShader);
    if (FAILED(hr)) return hr;

    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_pixelShader);
    if (FAILED(hr)) return hr;

    // 🎯 Input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    hr = device->CreateInputLayout(layout, 1, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_inputLayout);


    //XMVECTOR eye = XMVectorSet(100.0f, 100.0f, 100.0f, 1.0f);
    //XMVECTOR target = XMVectorZero(); // (0, 0, 0)
    //XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // dirección "arriba" del mundo

    //m_viewMatrix = XMMatrixLookAtLH(eye, target, up);


    //m_cameraManager->GetCurrentCamera()->SetPosition(100.0f, 100.0f, 100.0f);
    //m_cameraManager->GetCurrentCamera()->SetLookAt(0.0f, 0.0f, 0.0f);

    return hr;
}

void TestBasicTexture::Render() {
    ID3D11DeviceContext* context = m_deviceManager->GetContext();
    UINT stride = sizeof(VertexBasic2);
    UINT offset = 0;

    MatrixBufferType matrices = {};
    matrices.world = XMMatrixIdentity();
    matrices.view = XMMatrixTranspose(m_cameraManager->GetCurrentViewMatrix());
    matrices.projection = XMMatrixTranspose(m_cameraManager->GetCurrentProjectionMatrix());


    /*matrices.world = XMMatrixIdentity();
    matrices.view = XMMatrixTranspose(m_viewMatrix);
    matrices.projection = XMMatrixTranspose(m_cameraManager->GetCurrentProjectionMatrix());*/

    D3D11_MAPPED_SUBRESOURCE mapped = {};
    context->Map(m_matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, &matrices, sizeof(MatrixBufferType));
    context->Unmap(m_matrixBuffer.Get(), 0);
    context->VSSetConstantBuffers(0, 1, m_matrixBuffer.GetAddressOf());

    context->IASetInputLayout(m_inputLayout.Get());
    //context->RSSetState(m_rasterState.Get());
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->Draw(3, 0);
}

//void TestBasicTexture::Render() {
//    ID3D11DeviceContext* context = m_deviceManager->GetContext();
//    UINT stride = sizeof(Vertex);
//    UINT offset = 0;
//
//    MatrixBufferType matrices = {};
//    matrices.world = XMMatrixTranspose(XMMatrixIdentity());
//    matrices.view = XMMatrixTranspose(m_cameraManager->GetCurrentViewMatrix());
//    matrices.projection = XMMatrixTranspose(m_cameraManager->GetCurrentProjectionMatrix());
//
//    D3D11_MAPPED_SUBRESOURCE mapped = {};
//    context->Map(m_matrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
//    memcpy(mapped.pData, &matrices, sizeof(MatrixBufferType));
//    context->Unmap(m_matrixBuffer.Get(), 0);
//    context->VSSetConstantBuffers(0, 1, m_matrixBuffer.GetAddressOf());
//
//    context->IASetInputLayout(m_inputLayout.Get());
//    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
//    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
//    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
//    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//    context->Draw(3, 0);
//}

void TestBasicTexture::Shutdown() {
    // Todos los ComPtrs se limpian automáticamente
}