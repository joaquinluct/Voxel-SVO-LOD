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

    // Create a 1x1 white texture SRV for UI fallback
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = 1;
    texDesc.Height = 1;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;

    UINT32 whitePixel[1] = { 0xFFFFFFFF };
    D3D11_SUBRESOURCE_DATA texInit = {};
    texInit.pSysMem = whitePixel;
    texInit.SysMemPitch = 4;

    ID3D11Texture2D* tex = nullptr;
    hr = m_device->CreateTexture2D(&texDesc, &texInit, &tex);
    if (SUCCEEDED(hr) && tex) {
        ID3D11ShaderResourceView* srv = nullptr;
        hr = m_device->CreateShaderResourceView(tex, nullptr, &srv);
        if (SUCCEEDED(hr)) {
            m_whiteTextureSRV.Attach(srv);
        }
        tex->Release();
    }

    // Create a simple UI atlas (256x256 white) as POC
    D3D11_TEXTURE2D_DESC atlasDesc = {};
    atlasDesc.Width = 256;
    atlasDesc.Height = 256;
    atlasDesc.MipLevels = 1;
    atlasDesc.ArraySize = 1;
    atlasDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    atlasDesc.SampleDesc.Count = 1;
    atlasDesc.Usage = D3D11_USAGE_DEFAULT;
    atlasDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    atlasDesc.CPUAccessFlags = 0;

    std::vector<UINT32> atlasData(256 * 256, 0xFFFFFFFF);
    D3D11_SUBRESOURCE_DATA atlasInit = {};
    atlasInit.pSysMem = atlasData.data();
    atlasInit.SysMemPitch = 256 * 4;
    ID3D11Texture2D* atlasTex = nullptr;
    hr = m_device->CreateTexture2D(&atlasDesc, &atlasInit, &atlasTex);
    if (SUCCEEDED(hr) && atlasTex) {
        ID3D11ShaderResourceView* atlasSrv = nullptr;
        hr = m_device->CreateShaderResourceView(atlasTex, nullptr, &atlasSrv);
        if (SUCCEEDED(hr)) {
            m_uiAtlasSRV.Attach(atlasSrv);
        }
        atlasTex->Release();
    }

    // If a real atlas file exists in Assets, notify (automatic WIC loader not available here)
    std::wstring atlasFile = L"Assets\\Textures\\UI\\atlas_ui.png";
    DWORD attrs = GetFileAttributesW(atlasFile.c_str());
    if (attrs != INVALID_FILE_ATTRIBUTES) {
        OutputDebugStringA("UIRenderer: Found Assets/Textures/UI/atlas_ui.png — loading via AssetManager recommended. Using POC atlas for now.\n");
    }

    // Create simple point sampler for UI
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    ID3D11SamplerState* sampler = nullptr;
    hr = m_device->CreateSamplerState(&sampDesc, &sampler);
    if (SUCCEEDED(hr)) {
        m_samplerState.Attach(sampler);
        ID3D11SamplerState* s = m_samplerState.Get();
        m_context->PSSetSamplers(0, 1, &s);
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
            // Prefer UI atlas if available
            if (m_uiAtlasSRV) {
                ID3D11ShaderResourceView* a = m_uiAtlasSRV.Get();
                m_context->PSSetShaderResources(0, 1, &a);
            } else if (srv) {
                m_context->PSSetShaderResources(0, 1, &srv);
            } else if (m_whiteTextureSRV) {
                ID3D11ShaderResourceView* w = m_whiteTextureSRV.Get();
                m_context->PSSetShaderResources(0, 1, &w);
            } else {
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
    if (m_constantBuffer) m_constantBuffer.Reset();
    if (m_whiteTextureSRV) m_whiteTextureSRV.Reset();
    if (m_samplerState) m_samplerState.Reset();
}
