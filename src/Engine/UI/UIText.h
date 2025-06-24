#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <memory> // Para std::shared_ptr
#include <UI/UIElement.h>

// Forward declarations para managers que UIText utiliza
class DeviceManager;
class Material; // Asumiendo que tienes una clase Material
class WorldMatrixManager;

// Estructura de vértice (asegúrate de que coincide con tu shader)
struct UIVertex {
    DirectX::XMFLOAT3 Position;
    DirectX::XMFLOAT2 TexCoord;
    DirectX::XMFLOAT4 Color;
};

class UIText : public UIElement {
public:
    // Constructor de UIText, recibe dependencias
    UIText(std::shared_ptr<DeviceManager> deviceManager,
        std::shared_ptr<WorldMatrixManager> worldMatrixManager,
        Material* material);
    ~UIText();

    HRESULT Init() { return S_OK; };

    // Setters para configurar el texto y su apariencia
    void SetText(const std::string& text);
    void SetPosition(float x, float y); // Asume Z=0 para UI
    void SetColor(float r, float g, float b, float a);
    void SetFontSize(float size);

    // Render ahora solo toma la matriz ortográfica global (o específica para esta capa de UI)
    // Usará los miembros internos (m_text, m_position, etc.) para dibujar.
    void Render(const DirectX::XMMATRIX& orthoMatrix);

private:
    std::string m_text;
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT4 m_color;
    float m_fontSize;

    std::shared_ptr<DeviceManager> m_device;
    std::shared_ptr<WorldMatrixManager> m_worldMatrixManager;
    std::shared_ptr<Material> m_material;

    ID3D11Buffer* m_vertexBuffer; // Gestionado dinámicamente

    UINT CreateVertexBuffer();
    void SetVertexBuffer(ID3D11DeviceContext* context);
    void ApplyMaterial(ID3D11DeviceContext* context, std::shared_ptr<Material> m_material, XMMATRIX orthoMatrix);

    // Método privado para liberar recursos
    void Release();
};