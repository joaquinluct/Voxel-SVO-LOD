// UIButton.h
#pragma once
#include "UIRectangle.h"
#include "UIText.h"
#include <functional> // Para std::function (callbacks)

using namespace DirectX;

class UIButton : public UIRectangle
{
public:
    UIButton();
    UIButton(const XMFLOAT2& position, const XMFLOAT2& size, const std::string& text, const XMFLOAT4& color, std::function<void()> onClick);
    ~UIButton();

    HRESULT Init() override;
    void Release() override;
    void Render(const XMMATRIX& orthoMatrix);

    void SetText(const std::string& text) { m_text->SetText(text); }
    void SetOnClick(std::function<void()> onClick) { m_onClick = onClick; }

    bool IsClicked(const XMFLOAT2& mousePos); // Check if mouse is over and clicked

    void HandleClick(); // Call the callback function

private:
    UIText* m_text;
    std::function<void()> m_onClick;
    // Estados del botón (normal, hover, pressed)
};
