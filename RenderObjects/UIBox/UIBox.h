// UIBox.h
#pragma once

#include "../Line/Line.h"
#include <DirectXMath.h> // Para XMFLOAT3, XMFLOAT4
#include <Windows.h>
#include <string>
#include <vector>
#include <Services/Material.h>

class UIBox {
public:
    // Constructor: Recibe el material y las propiedades de la caja.
    // origin: La esquina inferior, frontal, izquierda de la caja.
    // sizeX, sizeY, sizeZ: Las dimensiones de la caja.
    // color: El color de las l�neas de la caja.
    UIBox(Material* material, const DirectX::XMFLOAT3& origin, float sizeX, float sizeY, float sizeZ, const DirectX::XMFLOAT4& color);

    ~UIBox();

    // M�todo de inicializaci�n (para crear los recursos de Direct3D para las l�neas)
    HRESULT Init(Microsoft::WRL::ComPtr<ID3D11Device> device);
    
    // M�todo de renderizado
    void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

    // M�todo para liberar recursos
    void Release();

    DirectX::XMFLOAT3 GetOrigin() const { return m_origin; };
    DirectX::XMFLOAT4 GetColor() const { return m_color; };
    DirectX::XMFLOAT3 GetSize() const { return DirectX::XMFLOAT3(m_sizeX, m_sizeY, m_sizeZ); }
    void SetColor(const DirectX::XMFLOAT4& color) { m_color = color; }

private:
    Material* m_material;
    std::vector<Line*> m_lines;
    DirectX::XMFLOAT3 m_origin;
    float m_sizeX, m_sizeY, m_sizeZ;
    DirectX::XMFLOAT4 m_color;

    // Métodos privados para calcular los puntos de las esquinas y crear las líneas
    void CreateLines();
};