// UIRectangle.cpp
#include "UIRectangle.h"
// Asumimos que tenemos una función DrawRectangle2D
// que maneja el renderizado real del rectángulo con Direct3D
//void DrawRectangle2D(float x, float y, float width, float height, const XMFLOAT4& color, const XMMATRIX& orthoMatrix);

UIRectangle::UIRectangle() : m_color(1.0f, 1.0f, 1.0f, 1.0f) {}

UIRectangle::UIRectangle(const XMFLOAT2& position, const XMFLOAT2& size, const XMFLOAT4& color)
    : m_color(color)
{
    SetPosition(position);
    SetSize(size);
}

UIRectangle::~UIRectangle()
{
    Release();
}

HRESULT UIRectangle::Init()
{
    // Inicializar recursos para dibujar rectángulos
    return S_OK;
}

void UIRectangle::Render(DeviceManager* deviceManager, ShaderManager* m_shaderManager, WorldMatrixManager* worldMatrix, const XMMATRIX& orthoMatrix) // Implementación de Render()
{
    //DrawRectangle2D(m_position.x, m_position.y, m_size.x, m_size.y, m_color, orthoMatrix);
}

void UIRectangle::Release()
{
    // Liberar recursos de rectángulo
}