#include "UIRenderer.h"
#include <ManagerLocator/ManagerLocator.h>
#include <Managers/ShaderManager.h>
#include <Defines/VertexDefinition.h>
#include <Assets/Base/ShaderAsset.h>
#include <AssetLocator/AssetLocator.h>
#include <Util/DirectXUtils.h>
#include <DirectXMath.h>
#include <Managers/DeviceManager.h>
#include <Engine/Rendering/RenderCommand.h>

HRESULT UIRenderer::Init(ID3D11Device* device, ID3D11DeviceContext* context) {
    if (!device || !context) return E_FAIL;
    m_device = device;
    m_context = context;

    // Load shader via ShaderManager
    auto shaderManager = ManagerLocator::GetManager<ShaderManager>();
    if (!shaderManager) {
        OutputDebugStringA("UIRenderer: ShaderManager not available\n");
        return E_FAIL;
    }

    std::wstring name = L"ShaderTextUI";
    std::shared_ptr<ShaderAsset> shaderAsset = shaderManager->LoadShaderByName(name);
    if (!shaderAsset) {
        OutputDebugStringA("UIRenderer: Failed to load ShaderTextUI via ShaderManager\n");
        // Not fatal for now
    }

    // Create input layout using VertexDefinition::TextVertex
    VertexDefinition::TextVertex tmp;
    unsigned int numElems = 0;
    D3D11_INPUT_ELEMENT_DESC* layout = tmp.GetInputLayout(numElems);
    if (!layout || numElems == 0) {
        OutputDebugStringA("UIRenderer: TextVertex layout not available\n");
        return S_OK; // still allow engine to run; UI will be no-op
    }

    // Try to get VS bytecode and shader objects from ShaderManager
    ID3DBlob* vsBlob = shaderManager->GetVertexShaderBytecode(name);
    UINT vsSize = shaderManager->GetVertexShaderBytecodeLength(name);
    auto vsObj = shaderManager->GetVertexShader(name);
    auto psObj = shaderManager->GetPixelShader(name);
    if (vsObj) m_vertexShader = vsObj;
    if (psObj) m_pixelShader = psObj;

    if (vsBlob && vsSize > 0) {
        ID3D11InputLayout* inputLayout = nullptr;
        HRESULT hr = m_device->CreateInputLayout(layout, numElems, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout);
        if (SUCCEEDED(hr)) {
            m_inputLayout.Attach(inputLayout);
        } else {
            OutputDebugStringA("UIRenderer: Failed to create input layout from VS blob\n");
        }
    } else {
        OutputDebugStringA("UIRenderer: VS bytecode not available for ShaderTextUI; skipping input layout creation\n");
    }

    // free layout array allocated by GetInputLayout
    delete[] layout;

    // Create a dynamic vertex buffer for UI (TextVertex)
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DYNAMIC;
    vbDesc.ByteWidth = sizeof(VertexDefinition::TextVertex) * 6 * 16; // space for 16 quads
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbDesc.MiscFlags = 0;
    ID3D11Buffer* vb = nullptr;
    HRESULT hr = m_device->CreateBuffer(&vbDesc, nullptr, &vb);
    if (SUCCEEDED(hr)) {
        m_vertexBuffer.Attach(vb);
    }

    // Create a simple constant buffer for projection (identity by default)
    D3D11_BUFFER_DESC cbDesc = {};
    cbDesc.Usage = D3D11_USAGE_DEFAULT;
    cbDesc.ByteWidth = sizeof(DirectX::XMMATRIX);
    cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbDesc.CPUAccessFlags = 0;
    DirectX::XMMATRIX identity = DirectX::XMMatrixIdentity();
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &identity;
    ID3D11Buffer* cb = nullptr;
    hr = m_device->CreateBuffer(&cbDesc, &initData, &cb);
    if (SUCCEEDED(hr)) {
        // Bind to slot b13 as TextShader expects
        m_context->VSSetConstantBuffers(13, 1, &cb);
        // store local ref so Shutdown can release
        Microsoft::WRL::ComPtr<ID3D11Buffer> cbPtr(cb);
        // keep a reference via m_vertexBuffer as placeholder (no member for cb)
    }

    return S_OK;
}

void UIRenderer::ExecuteUICommands(const RenderCommandPacket& packet) {
    if (!m_context) return;

    // Bind UI shaders/layout if available
    if (m_inputLayout) {
        m_context->IASetInputLayout(m_inputLayout.Get());
    }

    for (const auto& cmd : packet.commands) {
        // For now, commands Execute() are no-op placeholders; call them to keep pipeline
        if (cmd) cmd->Execute(m_context);
    }
}

void UIRenderer::Shutdown() {
    if (m_vertexBuffer) m_vertexBuffer.Reset();
    if (m_inputLayout) m_inputLayout.Reset();
    if (m_vertexShader) m_vertexShader.Reset();
    if (m_pixelShader) m_pixelShader.Reset();
}
