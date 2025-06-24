#pragma once
#include <d3d11.h>
#include <vector>
#include <directxmath.h>
#include "Material/Material.h"
#include "Utils.h"
using namespace DirectX;

class iDrawable {
public:
	virtual ~iDrawable() = default; // Destructor virtual para permitir la limpieza de recursos derivados
    virtual HRESULT Init(ID3D11Device* device) = 0;         // Inicialización con acceso a DirectX
    virtual void Render(ID3D11DeviceContext* context) = 0;  // Dibujar el objeto    
    virtual ID3D11Buffer* GetVertexBuffer() = 0;            // Obtener el buffer de vértices
    void SetPosition(const XMFLOAT3& position) {
        m_position = position;
    };
    void SetMaterial(Material* material) {
        m_material = material;
	};
    virtual void Release() {
		SafeDelete(vertexBuffer);
		SafeDelete(m_material);
    }
	XMMATRIX GetWorldMatrix() const {
		return XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	}

protected:
    std::vector<XMFLOAT3>   vertices;                   // Lista de vértices
    ID3D11Buffer*           vertexBuffer    = nullptr;  // Buffer de vértices en GPU
	ID3D11Buffer*           m_indexBuffer   = nullptr;  // Buffer de índices en GPU
    Material*               m_material      = nullptr;  // Referencia al material
    XMFLOAT3                m_position      = { 0,0,0 };  // Posición del objeto
};
