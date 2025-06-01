#pragma once
#include "UIElement.h"
#include <string>
#include <vector>
#include <DirectXMath.h>
#include "../Util/TextureLoader/TextureLoader.h"
#include "../Material/Material.h"

class UIText : public UIElement {
public:
    UIText(Material* material, const std::string& text, const DirectX::XMFLOAT2& position, const DirectX::XMFLOAT4& color, float fontSize);
    ~UIText();
	HRESULT Init() override;
    HRESULT Init(ID3D11Device* device, int fontAtlasWidth, int fontAtlasHeight);
    void Render() override;
    void Render(DeviceManager* device, ShaderManager* m_shaderManager, WorldMatrixManager* worldMatrix, const XMMATRIX& orthoMatrix) override;
    void Release() override;

    void SetText(const std::string& text) { m_text = text; }
    void SetColor(const DirectX::XMFLOAT4& color) { m_color = color; }
    void SetFontSize(float fontSize) { m_fontSize = fontSize; }
    void SetPosition(const DirectX::XMFLOAT2& position) { m_position = position; }

private:
    struct Vertex {
        DirectX::XMFLOAT3 Position;
        DirectX::XMFLOAT2 TexCoord;
        DirectX::XMFLOAT4 Color;
    };

    std::string m_text;
    DirectX::XMFLOAT2 m_position;
    DirectX::XMFLOAT4 m_color;
    float m_fontSize;

    ID3D11Buffer* m_vertexBuffer;
    ID3D11ShaderResourceView* m_fontTextureView;
    int m_fontAtlasWidth;
    int m_fontAtlasHeight;
	Material* m_material; // Material para el texto
};