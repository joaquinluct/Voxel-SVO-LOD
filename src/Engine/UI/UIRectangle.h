// UIRectangle.h
#pragma once
#include "UIElement.h"

class UIRectangle : public UIElement
{
public:
    UIRectangle();
    UIRectangle(const XMFLOAT2& position, const XMFLOAT2& size, const XMFLOAT4& color);
    ~UIRectangle();

    HRESULT Init() override;
    //void Render() override {};
    void Render(const XMMATRIX& orthoMatrix);
    void Release();

    void SetColor(const XMFLOAT4& color) { m_color = color; }

private:
    XMFLOAT4 m_color;
    // Recursos Direct3D para dibujar rectángulos
};
