#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "../../Material/Material.h"
#include "../../Interfaces/iDrawable.h"

using namespace DirectX;

class Line : public iDrawable {
public:
    Line(Material* material, const XMFLOAT3& start, const XMFLOAT3& end, const XMFLOAT4& color);
    ~Line();

    HRESULT Init(ID3D11Device* device) override;
    void Render(ID3D11DeviceContext* context) override;
    void Release() override;
    ID3D11Buffer* GetVertexBuffer() override { return m_vertexBuffer; }
    //void SetKeyboardManager(KeyboardManager* keyboardManager) override {}
private:
    struct Vertex {
        XMFLOAT3 Position;
        XMFLOAT4 Color;
    };

    Material* m_material;
    ID3D11Buffer* m_vertexBuffer;
    UINT m_numVertices;
    Vertex m_vertices[2]; // Almacena los dos vértices de la línea
};
