#pragma once

#include <Engine/Rendering/RenderCommand.h>
#include <wrl/client.h>
#include <d3d11.h>

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
};
