#pragma once
#include "iDrawable.h"
#include "Services/Material.h"
#include <d3d11.h>
#include <vector>
#include <directxmath.h>

using namespace DirectX;

class Triangulo : public iDrawable {
public:
    Triangulo(Material* material);
    HRESULT Init(std::shared_ptr<ID3D11Device> device) override;
    void Render(ID3D11DeviceContext* context) override;
    void Release() override;
	ID3D11Buffer* GetVertexBuffer() override { return m_vertexBuffer; } // Obtener el buffer de vértices
    //void SetKeyboardManager(KeyboardManager* keyboardManager) override {}

private:
    std::vector<XMFLOAT3> vertices;
    ID3D11Buffer* m_vertexBuffer;
    Material* m_material; // Nuevo: referencia al material
};