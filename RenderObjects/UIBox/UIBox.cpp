// UIBox.cpp
#include "UIBox.h"

// Constructor
UIBox::UIBox(Material* material, const DirectX::XMFLOAT3& origin, float sizeX, float sizeY, float sizeZ, const DirectX::XMFLOAT4& color)
    : m_material(material),
    m_origin(origin),
    m_sizeX(sizeX),
    m_sizeY(sizeY),
    m_sizeZ(sizeZ),
    m_color(color)
{
    // Las l�neas se crear�n en CreateLines()
}

// Destructor: Libera las l�neas
UIBox::~UIBox() {
    Release(); // Llama a Release para liberar los recursos de D3D
}

// M�todo para crear las 12 l�neas de la caja
void UIBox::CreateLines() {
    // Definir las 8 esquinas de la caja relativas al origen
    // Los puntos se calculan desde m_origin y los tama�os m_sizeX, m_sizeY, m_sizeZ

    // Esquinas de la base "inferior" (Z m�nima)
    DirectX::XMFLOAT3 p0 = m_origin;                                     // (minX, minY, minZ)
    DirectX::XMFLOAT3 p1 = { m_origin.x + m_sizeX, m_origin.y, m_origin.z }; // (maxX, minY, minZ)
    DirectX::XMFLOAT3 p2 = { m_origin.x, m_origin.y + m_sizeY, m_origin.z }; // (minX, maxY, minZ)
    DirectX::XMFLOAT3 p3 = { m_origin.x + m_sizeX, m_origin.y + m_sizeY, m_origin.z }; // (maxX, maxY, minZ)

    // Esquinas de la base "superior" (Z m�xima)
    DirectX::XMFLOAT3 p4 = { m_origin.x, m_origin.y, m_origin.z + m_sizeZ }; // (minX, minY, maxZ)
    DirectX::XMFLOAT3 p5 = { m_origin.x + m_sizeX, m_origin.y, m_origin.z + m_sizeZ }; // (maxX, minY, maxZ)
    DirectX::XMFLOAT3 p6 = { m_origin.x, m_origin.y + m_sizeY, m_origin.z + m_sizeZ }; // (minX, maxY, maxZ)
    DirectX::XMFLOAT3 p7 = { m_origin.x + m_sizeX, m_origin.y + m_sizeY, m_origin.z + m_sizeZ }; // (maxX, maxY, maxZ)

    // Las 12 aristas de la caja
    // Base Inferior (Z m�nima)
    m_lines.push_back(new Line(m_material, p0, p1, m_color));
    m_lines.push_back(new Line(m_material, p1, p3, m_color));
    m_lines.push_back(new Line(m_material, p3, p2, m_color));
    m_lines.push_back(new Line(m_material, p2, p0, m_color));

    // Base Superior (Z m�xima)
    m_lines.push_back(new Line(m_material, p4, p5, m_color));
    m_lines.push_back(new Line(m_material, p5, p7, m_color));
    m_lines.push_back(new Line(m_material, p7, p6, m_color));
    m_lines.push_back(new Line(m_material, p6, p4, m_color));

    // Aristas Verticales (Conectando bases)
    m_lines.push_back(new Line(m_material, p0, p4, m_color));
    m_lines.push_back(new Line(m_material, p1, p5, m_color));
    m_lines.push_back(new Line(m_material, p2, p6, m_color));
    m_lines.push_back(new Line(m_material, p3, p7, m_color));
}

// Inicializaci�n: Llama a Init para cada l�nea
HRESULT UIBox::Init(ID3D11Device* device) {
    CreateLines(); // Primero, crea las instancias de Line
    for (Line* line : m_lines) {
        if (FAILED(line->Init(device))) {
            // Manejar error: liberar lo que ya se inicializ� y retornar
            return E_FAIL;
        }
    }
    return S_OK;
}

// Renderizado: Llama a Render para cada l�nea
void UIBox::Render(ID3D11DeviceContext* context) {
    for (Line* line : m_lines) {
        line->Render(context);
    }
}

// Liberaci�n de recursos: Libera cada l�nea y limpia el vector
void UIBox::Release() {
    for (Line* line : m_lines) {
        line->Release(); // Libera los recursos de D3D de la l�nea
        delete line;     // Elimina la instancia de Line
    }
    m_lines.clear(); // Limpia el vector
}