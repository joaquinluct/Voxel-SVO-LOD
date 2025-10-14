#pragma once

#include <Engine/Rendering/RenderCommand.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <map>

// Minimal UIRenderer executed on the render thread. Implements basic
// shader loading and input-layout creation for UI TextVertex.
class UIRenderer {
public:
    UIRenderer() = default;
    ~UIRenderer() { Shutdown(); }

    HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* context);

    // Execute commands from a packet that pertains to UI
    void ExecuteUICommands(const RenderCommandPacket& packet);

    void Shutdown();

private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;

    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer; // matrix buffer (ortho)
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_whiteTextureSRV;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_uiAtlasSRV;
    // Atlas metrics
    struct GlyphMetric {
        int x; int y; int w; int h; int advance;
    };
    std::map<int, GlyphMetric> m_glyphs;
    int m_atlasTextureSize = 256;
    int m_atlasCellSize = 16;

    // Dynamic VB tracking
    size_t m_vbCapacityVertices = 0;
};
