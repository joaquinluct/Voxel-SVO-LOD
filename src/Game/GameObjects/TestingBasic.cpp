#include "TestingBasic.h"
#include <REGISTER_SERVICE_MACRO.h>
#include <ManagerLocator/ManagerLocator.h>
#include <d3dcompiler.h>

using namespace Microsoft::WRL;
using namespace DirectX;

REGISTER_SERVICE_TYPE(TestingBasic, "TestingBasic");

struct VertexBasic {
    XMFLOAT3 position;
};

TestingBasic::TestingBasic() {}
TestingBasic::~TestingBasic() {}

HRESULT TestingBasic::Init() {
    m_deviceManager = ManagerLocator::GetManager<DeviceManager>();
    std::shared_ptr<ID3D11Device> device = m_deviceManager->GetDevice();

    // 🎯 Triángulo simple
    VertexBasic vertices[] = {
        { XMFLOAT3(0.0f,  0.5f, 0.0f) },
        { XMFLOAT3(0.5f, -0.5f, 0.0f) },
        { XMFLOAT3(-0.5f, -0.5f, 0.0f) }
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

    // 🎯 Shaders embebidos (minimalistas)
    const char* vsCode = R"(
        struct VSInput { float3 pos : POSITION; };
        struct VSOutput { float4 pos : SV_POSITION; };

        VSOutput VSMain(VSInput input) {
            VSOutput output;
            output.pos = float4(input.pos, 1.0f);
            return output;
        }
    )";

    const char* psCode = R"(
        float4 PSMain() : SV_TARGET {
            return float4(1, 0, 0, 1); // rojo
        }
    )";

    Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, psBlob, errorBlob;
    hr = D3DCompile(vsCode, strlen(vsCode), nullptr, nullptr, nullptr, "VSMain", "vs_4_0", 0, 0, &vsBlob, &errorBlob);
    if (FAILED(hr)) return hr;

    hr = D3DCompile(psCode, strlen(psCode), nullptr, nullptr, nullptr, "PSMain", "ps_4_0", 0, 0, &psBlob, &errorBlob);
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
    return hr;
}

void TestingBasic::Render() {

    m_deviceManager->InitRasterizedState();

    ID3D11DeviceContext* context = m_deviceManager->GetContext();
    UINT stride = sizeof(VertexBasic);
    UINT offset = 0;

    context->IASetInputLayout(m_inputLayout.Get());
    context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
    context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->Draw(3, 0);
}

void TestingBasic::Shutdown() {
    // Todos los ComPtrs se limpian automáticamente
}