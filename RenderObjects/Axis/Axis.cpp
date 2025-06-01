#include "Axis.h"

Axis::Axis(Material* material) : m_material(material) {}

Axis::~Axis() {
    Release();
}

HRESULT Axis::Init(ID3D11Device* device) {

	float size = 1000.0f; // Tamaño de los ejes
    float originX = 0.0f;
    float originY = 0.0f;
    float originZ = 0.0f;

    // Definir los puntos y colores para los ejes
    XMFLOAT3 origin = { originX, originY, originZ };
    XMFLOAT3 xAxisEnd = { size, originY, originZ };
    XMFLOAT3 yAxisEnd = { originX, size, originZ };
    XMFLOAT3 zAxisEnd = { originX, originY, size };
    XMFLOAT4 red = { size, 0.0f, 0.0f, size };
    XMFLOAT4 green = { 0.0f, size, 0.0f, size };
    XMFLOAT4 blue = { 0.0f, 0.0f, size, size };

    XMFLOAT3 originBoxX = { 10.0f, .0f, .0f };
    XMFLOAT3 originBoxY = { .0f, 10.0f, .0f };
    XMFLOAT3 originBoxZ = { .0f, .0f, 10.0f };

    // Crear objetos Line
    m_lines.push_back(new Line(m_material, origin, xAxisEnd, red));
    m_lines.push_back(new Line(m_material, origin, yAxisEnd, green));
    m_lines.push_back(new Line(m_material, origin, zAxisEnd, blue));

    // Inicializar las líneas
    for (Line* line : m_lines) {
        if (FAILED(line->Init(device))) {
            return E_FAIL; // Handle error appropriately
        }
    }

    boxes.push_back(new UIBox(m_material, originBoxX, 2.2f, 2.2f, 2.2f, red));
    boxes.push_back(new UIBox(m_material, originBoxY, 2.2f, 2.2f, 2.2f, green));
    boxes.push_back(new UIBox(m_material, originBoxZ, 2.2f, 2.2f, 2.2f, blue));

    for (UIBox* box : boxes) {
        box->Init(device);
    }
    
    return S_OK;
}

void Axis::Render(ID3D11DeviceContext* context) {
    // Renderizar cada línea
    for (Line* line : m_lines) {
        line->Render(context);
    }
    for (UIBox* box: boxes) {
        box->Render(context);
    }
}

void Axis::Release() {
    // Liberar y eliminar cada línea
    for (Line* line : m_lines) {
        line->Release();
        delete line;
    }
    m_lines.clear();
}