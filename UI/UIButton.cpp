// UIButton.cpp
#include "UIButton.h"

UIButton::UIButton(): m_text(nullptr) {}

UIButton::UIButton(const XMFLOAT2& position, const XMFLOAT2& size, const std::string& text, const XMFLOAT4& color, std::function<void()> onClick)
    : UIRectangle(position, size, color), m_text(), m_onClick(onClick)
{
    m_text->SetText(text);
    m_text->SetPosition(position); // Centrar el texto en el botón
}

UIButton::~UIButton()
{
    Release();
}

HRESULT UIButton::Init()
{
    UIRectangle::Init();
    //m_text->Init();
    return S_OK;
}

void UIButton::Render()
{
    UIRectangle::Render();
    //m_text->Render();
}

void UIButton::Release()
{
    UIRectangle::Release();
    m_text->Release();
}

bool UIButton::IsClicked(const XMFLOAT2& mousePos)
{
    // Lógica para verificar si el ratón está dentro del botón y se ha hecho clic
    return false; // Placeholder
}

void UIButton::HandleClick()
{
    if (m_onClick)
    {
        m_onClick();
    }
}