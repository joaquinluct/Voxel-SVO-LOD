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
#include <Managers/AssetManager.h>
#include <fstream>
#include <sstream>

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

    // Try to obtain UIAtlas from AssetManager (registered via YAML)
    auto assetManager = ManagerLocator::GetManager<AssetManager>();
    if (assetManager) {
        ID3D11ShaderResourceView* atlasSrv = assetManager->LoadTexture("UIAtlas");
        if (atlasSrv) {
            m_uiAtlasSRV.Attach(atlasSrv);
            OutputDebugStringA("UIRenderer: Loaded UIAtlas via AssetManager.\n");
        }
        else {
            OutputDebugStringA("UIRenderer: UIAtlas asset not found via AssetManager; using POC atlas.\n");
        }
    }

    // Try to load atlas metrics JSON (optional) with a tiny parser
    std::ifstream jf("Assets/Textures/UI/atlas_ui.json");
    if (jf.good()) {
        std::string line;
        std::string content;
        while (std::getline(jf, line)) content += line + "\n";
        jf.close();
        // crude parse: extract textureSize and cellSize
        auto findInt = [&](const std::string& key, int fallback) {
            auto pos = content.find('"' + key + '"');
            if (pos == std::string::npos) return fallback;
            auto colon = content.find(':', pos);
            if (colon == std::string::npos) return fallback;
            auto end = content.find_first_of(",}\n", colon+1);
            std::string num = content.substr(colon+1, end-colon-1);
            return atoi(num.c_str());
        };
        m_atlasTextureSize = findInt("textureSize", m_atlasTextureSize);
        m_atlasCellSize = findInt("cellSize", m_atlasCellSize);
        // parse glyphs entries: look for "glyphs": { ... }
        auto gpos = content.find("\"glyphs\"");
        if (gpos != std::string::npos) {
            auto brace = content.find('{', gpos);
            auto endb = content.find('}', brace+1);
            if (brace != std::string::npos && endb != std::string::npos) {
                std::string block = content.substr(brace+1, endb-brace-1);
                std::istringstream iss(block);
                std::string entry;
                while (std::getline(iss, entry, '}')) {
                    auto q = entry.find('"');
                    if (q == std::string::npos) continue;
                    auto q2 = entry.find('"', q+1);
                    if (q2 == std::string::npos) continue;
                    std::string key = entry.substr(q+1, q2-q-1);
                    int code = atoi(key.c_str());
                    GlyphMetric gm = {0,0,m_atlasCellSize,m_atlasCellSize,m_atlasCellSize};
                    // find numbers inside entry
                    auto px = entry.find("\"x\"");
                    if (px != std::string::npos) gm.x = findInt("x", 0);
                    auto py = entry.find("\"y\"");
                    if (py != std::string::npos) gm.y = findInt("y", 0);
                    auto pw = entry.find("\"w\"");
                    if (pw != std::string::npos) gm.w = findInt("w", gm.w);
                    auto ph = entry.find("\"h\"");
                    if (ph != std::string::npos) gm.h = findInt("h", gm.h);
                    auto pa = entry.find("\"advance\"");
                    if (pa != std::string::npos) gm.advance = findInt("advance", gm.w);
                    m_glyphs[code] = gm;
                }
            }
        }
        OutputDebugStringA("UIRenderer: Parsed atlas_ui.json (heuristic parser)\n");
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

            // If we have an atlas and glyph metrics, render per-character using dynamic VB
            if (m_uiAtlasSRV && !m_glyphs.empty()) {
                struct UIVertex { DirectX::XMFLOAT3 pos; DirectX::XMFLOAT2 uv; DirectX::XMFLOAT4 color; };
                std::vector<UIVertex> vertices;
                vertices.reserve(4096);

                // Collect text from UIManager
                for (auto& mesh : pair.second) {
                    if (!mesh) continue;
                    // Try to find owning UIText by searching UIManager entries
                    UIText* owner = nullptr;
                    auto uiMgr = ManagerLocator::GetManager<UIManager>();
                    if (uiMgr) {
                        for (auto& kv : uiMgr->GetTextElements()) {
                            if (kv.second && kv.second->GetMesh() == mesh) {
                                owner = kv.second;
                                break;
                            }
                        }
                    }
                    if (!owner) continue;

                    std::string text = owner->GetText();
                    float x = roundf(owner->GetPosition().x);
                    float y = roundf(owner->GetPosition().y);
                    float fontSize = owner->GetFontSize();
                    DirectX::XMFLOAT4 color = owner->GetColor();

                    // Convert from top-left pixel coords to centered orthographic coords used by UI shaders
                    std::shared_ptr<DeviceManager> dev = ManagerLocator::GetDeviceManager();
                    if (dev) {
                        float sw = static_cast<float>(dev->GetWidth());
                        float sh = static_cast<float>(dev->GetHeight());
                        // Owner positions are expected in pixels from top-left; convert to centered origin (X right, Y down -> Y up)
                        x = x - (sw * 0.5f);
                        y = (sh * 0.5f) - y;
                    }

                    for (char c : text) {
                        int code = static_cast<unsigned char>(c);
                        auto it = m_glyphs.find(code);
                        int gx = 0, gy = 0, gw = m_atlasCellSize, gh = m_atlasCellSize, adv = gw;
                        if (it != m_glyphs.end()) {
                            gx = it->second.x; gy = it->second.y; gw = it->second.w; gh = it->second.h; adv = it->second.advance;
                        }
                        float u0 = (float)gx / (float)m_atlasTextureSize;
                        float v0 = (float)gy / (float)m_atlasTextureSize;
                        float u1 = (float)(gx + gw) / (float)m_atlasTextureSize;
                        float v1 = (float)(gy + gh) / (float)m_atlasTextureSize;

                        float w = (float)gw * (fontSize / (float)m_atlasCellSize);
                        float h = (float)gh * (fontSize / (float)m_atlasCellSize);

                        DirectX::XMFLOAT3 p1 = { x, y, 0.0f };
                        DirectX::XMFLOAT3 p2 = { x + w, y, 0.0f };
                        DirectX::XMFLOAT3 p3 = { x, y + h, 0.0f };

                        vertices.push_back({ p1, {u0, v0}, color });
                        vertices.push_back({ p2, {u1, v0}, color });
                        vertices.push_back({ p3, {u0, v1}, color });
                        vertices.push_back({ {x, y + h, 0.0f}, {u0, v1}, color });
                        vertices.push_back({ {x + w, y, 0.0f}, {u1, v0}, color });
                        vertices.push_back({ {x + w, y + h, 0.0f}, {u1, v1}, color });

                        x += adv * (fontSize / (float)m_atlasCellSize);
                    }
                }

                if (!vertices.empty()) {
                    // Ensure vertex buffer capacity
                    size_t vbNeeded = vertices.size();
                    if (vbNeeded > m_vbCapacityVertices) {
                        D3D11_BUFFER_DESC vbNew = {};
                        vbNew.Usage = D3D11_USAGE_DYNAMIC;
                        vbNew.ByteWidth = static_cast<UINT>(sizeof(UIVertex) * vbNeeded);
                        vbNew.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                        vbNew.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
                        vbNew.MiscFlags = 0;
                        ID3D11Buffer* newBuf = nullptr;
                        m_device->CreateBuffer(&vbNew, nullptr, &newBuf);
                        m_vertexBuffer.Attach(newBuf);
                        m_vbCapacityVertices = vbNeeded;
                    }

                    // Map and upload
                    D3D11_MAPPED_SUBRESOURCE mapped = {};
                    HRESULT mr = m_context->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
                    if (SUCCEEDED(mr)) {
                        memcpy(mapped.pData, vertices.data(), sizeof(UIVertex) * vertices.size());
                        m_context->Unmap(m_vertexBuffer.Get(), 0);

                        UINT stride = sizeof(UIVertex);
                        UINT offset = 0;
                        ID3D11Buffer* vbRaw = m_vertexBuffer.Get();
                        m_context->IASetVertexBuffers(0, 1, &vbRaw, &stride, &offset);
                        m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                        // Bind atlas
                        ID3D11ShaderResourceView* atlas = m_uiAtlasSRV.Get();
                        m_context->PSSetShaderResources(0, 1, &atlas);
                        ID3D11SamplerState* s = m_samplerState.Get();
                        m_context->PSSetSamplers(0, 1, &s);

                        m_context->Draw(static_cast<UINT>(vertices.size()), 0);
                    }
                }
            } else {
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
