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
#include <Defines/VertexDefinition.h>
#include <wrl/client.h>
#include <ManagerLocator/ManagerLocator.h>
#include <Managers/UIManager.h>
#include <Assets/Base/MeshAsset.h>
#include <unordered_map>

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
        m_constantBuffer.Attach(cb);
        ID3D11Buffer* cbRaw = m_constantBuffer.Get();
        m_context->VSSetConstantBuffers(13, 1, &cbRaw);
    }

    return S_OK;
}

void UIRenderer::ExecuteUICommands(const RenderCommandPacket& packet) {
    if (!m_context) return;

    // Prefer to render UIText meshes if UIManager provides them
    auto uiManager = ManagerLocator::GetManager<UIManager>();
    if (uiManager) {
        const auto& texts = uiManager->GetTextElements();

        // Group meshes by their first texture SRV to minimize PS state changes
        std::unordered_map<ID3D11ShaderResourceView*, std::vector<std::shared_ptr<MeshAsset>>> groups;
        for (const auto& kv : texts) {
            UIText* txt = kv.second;
            if (!txt) continue;
            auto mesh = txt->GetMesh();
            if (!mesh) continue;
            const Material* mat = mesh->GetMaterial();
            ID3D11ShaderResourceView* key = nullptr;
            if (mat) {
                auto texs = mat->GetTextures();
                if (!texs.empty()) key = texs[0];
            }
            groups[key].push_back(mesh);
        }

        // Bind shader and input layout once for all UI meshes
        if (m_vertexShader) m_context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
        if (m_pixelShader) m_context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
        if (m_inputLayout) m_context->IASetInputLayout(m_inputLayout.Get());

        // Update ortho matrix constant once
        XMMATRIX ortho = uiManager->GetOrthoMatrix();
        XMMATRIX orthoT = DirectX::XMMatrixTranspose(ortho);
        if (m_constantBuffer) {
            m_context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &orthoT, 0, 0);
            ID3D11Buffer* cb = m_constantBuffer.Get();
            m_context->VSSetConstantBuffers(13, 1, &cb);
        }

        // Iterate groups and bind textures per-group
        for (auto& pair : groups) {
            ID3D11ShaderResourceView* srv = pair.first;
            if (srv) {
                m_context->PSSetShaderResources(0, 1, &srv);
            } else {
                // Unbind texture slot 0
                ID3D11ShaderResourceView* nullSrv[1] = { nullptr };
                m_context->PSSetShaderResources(0, 1, nullSrv);
            }

            for (auto& mesh : pair.second) {
                if (!mesh) continue;
                ID3D11Buffer* vb = mesh->GetVertexBuffer(mesh->GetReadIndex()).Get();
                ID3D11Buffer* ib = mesh->GetIndexBuffer(mesh->GetReadIndex()).Get();
                if (vb) {
                    UINT stride = mesh->GetVertexTypeSize();
                    UINT offset = 0;
                    m_context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
                }
                if (ib) {
                    m_context->IASetIndexBuffer(ib, DXGI_FORMAT_R16_UINT, 0);
                }
                m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                UINT indexCount = mesh->GetIndexCount(mesh->GetReadIndex());
                if (indexCount > 0) m_context->DrawIndexed(indexCount, 0, 0);
            }
        }
        return;
    }
}

void UIRenderer::Shutdown() {
    if (m_vertexBuffer) m_vertexBuffer.Reset();
    if (m_inputLayout) m_inputLayout.Reset();
    if (m_vertexShader) m_vertexShader.Reset();
    if (m_pixelShader) m_pixelShader.Reset();
}
