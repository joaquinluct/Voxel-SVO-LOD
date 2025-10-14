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
#include "../../../nlohmann/json.hpp"

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

    // Prefer SDF/text shader if available, fall back to legacy UI text shader
    std::wstring preferredNames[] = { L"ShaderTextSDF", L"ShaderTextUI" };
    std::wstring selectedName;
    std::shared_ptr<ShaderAsset> shaderAsset = nullptr;
    for (auto& n : preferredNames) {
        shaderAsset = shaderManager->LoadShaderByName(n);
        if (shaderAsset) {
            selectedName = n;
            break;
        }
    }
    if (!shaderAsset) {
        OutputDebugStringA("UIRenderer: Failed to load ShaderTextSDF/ShaderTextUI via ShaderManager\n");
        // Not fatal for now
    } else {
        std::string msg = "UIRenderer: Loaded shader ";
        msg += std::string(WstringToString(selectedName));
        msg += " via ShaderManager\n";
        OutputDebugStringA(msg.c_str());
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
    // Use selectedName if set, otherwise default to ShaderTextUI
    std::wstring useName = selectedName.empty() ? L"ShaderTextUI" : selectedName;
    ID3DBlob* vsBlob = shaderManager->GetVertexShaderBytecode(useName);
    UINT vsSize = shaderManager->GetVertexShaderBytecodeLength(useName);
    auto vsObj = shaderManager->GetVertexShader(useName);
    auto psObj = shaderManager->GetPixelShader(useName);
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

    // Create SDF params constant buffer (b14)
    struct SDFParamsCB { float edge; float outlineWidth; float smoothness; float pad; } sdfInit;
    sdfInit.edge = m_sdfEdge;
    sdfInit.outlineWidth = m_sdfOutlineWidth;
    sdfInit.smoothness = m_sdfSmoothness;
    sdfInit.pad = 0.0f;

    D3D11_BUFFER_DESC sdfDesc = {};
    sdfDesc.Usage = D3D11_USAGE_DEFAULT;
    sdfDesc.ByteWidth = sizeof(SDFParamsCB);
    sdfDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    sdfDesc.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA sdfData = {};
    sdfData.pSysMem = &sdfInit;
    ID3D11Buffer* sdfBuf = nullptr;
    hr = m_device->CreateBuffer(&sdfDesc, &sdfData, &sdfBuf);
    if (SUCCEEDED(hr) && sdfBuf) {
        m_sdfParamsBuffer.Attach(sdfBuf);
        ID3D11Buffer* p = m_sdfParamsBuffer.Get();
        m_context->PSSetConstantBuffers(14, 1, &p);
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

    // Try to load atlas metrics JSON (optional) using nlohmann::json for robustness
    std::ifstream jf("Assets/Textures/UI/atlas_ui.json");
    if (jf.good()) {
        try {
            std::string content((std::istreambuf_iterator<char>(jf)), std::istreambuf_iterator<char>());
            jf.close();

            // strip simple '//' style comments so files with comments remain valid
            auto stripComments = [](const std::string& src) {
                std::string out;
                std::istringstream iss(src);
                std::string line;
                while (std::getline(iss, line)) {
                    size_t pos = line.find("//");
                    if (pos != std::string::npos) line.erase(pos);
                    out += line;
                    out.push_back('\n');
                }
                return out;
            };

            std::string filtered = stripComments(content);
            using nlohmann::json;
            json j = json::parse(filtered);

            if (j.contains("textureSize") && j["textureSize"].is_number_integer()) {
                m_atlasTextureSize = j["textureSize"].get<int>();
            }
            if (j.contains("cellSize") && j["cellSize"].is_number_integer()) {
                m_atlasCellSize = j["cellSize"].get<int>();
            }

            if (j.contains("glyphs") && j["glyphs"].is_object()) {
                m_glyphs.clear();
                for (const auto& kv : j["glyphs"].items()) {
                    const std::string& key = kv.first;
                    int code = atoi(key.c_str());
                    const auto& val = kv.second;
                    GlyphMetric gm = {0,0,m_atlasCellSize,m_atlasCellSize,m_atlasCellSize};
                    if (val.contains("x") && val["x"].is_number_integer()) gm.x = val["x"].get<int>();
                    if (val.contains("y") && val["y"].is_number_integer()) gm.y = val["y"].get<int>();
                    if (val.contains("w") && val["w"].is_number_integer()) gm.w = val["w"].get<int>();
                    if (val.contains("h") && val["h"].is_number_integer()) gm.h = val["h"].get<int>();
                    if (val.contains("advance") && val["advance"].is_number_integer()) gm.advance = val["advance"].get<int>();
                    m_glyphs[code] = gm;
                }
            }

            OutputDebugStringA("UIRenderer: Parsed atlas_ui.json (nlohmann::json)\n");
        }
        catch (const std::exception& e) {
            std::string msg = "UIRenderer: Failed to parse atlas_ui.json: ";
            msg += e.what();
            msg += "\n";
            OutputDebugStringA(msg.c_str());
        }
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
