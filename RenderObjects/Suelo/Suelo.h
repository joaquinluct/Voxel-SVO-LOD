#pragma once
#include <d3d11.h>
#include <vector>
#include <directxmath.h>
#include "iDrawable.h"
#include "Utils.h"

using namespace DirectX;

class Suelo : public iDrawable {
public:
    Suelo(Material* material);
	~Suelo();

    HRESULT Init(std::shared_ptr<ID3D11Device> device) override;
    void Render(ID3D11DeviceContext* context) override;
    void Release() override;
	ID3D11Buffer* GetVertexBuffer() override { return m_vertexBuffer; }
    //void SetKeyboardManager(KeyboardManager* keyboardManager) override {}
private:
    Vertex          m_vertices[4];
    UINT            m_index[6];
    ID3D11Buffer*   m_vertexBuffer;
    ID3D11Buffer*   m_indexBuffer;
    Material*       m_material ;
};