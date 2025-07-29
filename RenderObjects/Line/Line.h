#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <memory>
#include <DeviceManager.h>
#include <CameraManager.h>
#include <ManagerLocator/ManagerLocator.h>
#include <Services/Material.h>

class Line {
public:
    Line(Material* material, const XMFLOAT3& start, const XMFLOAT3& end, const XMFLOAT4& color);
    ~Line();

    HRESULT Init(Microsoft::WRL::ComPtr<ID3D11Device> device);
    void Render(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
    void Release();
    ID3D11Buffer* GetVertexBuffer() { return m_vertexBuffer; }
    //void SetKeyboardManager(KeyboardManager* keyboardManager) override {}
private:
    struct Vertex {
        XMFLOAT3 Position;
        XMFLOAT4 Color;
    };
	std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<CameraManager> m_cameraManager;
    Material* m_material;
    ID3D11Buffer* m_vertexBuffer;
    UINT m_numVertices;
    Vertex m_vertices[2]; // Almacena los dos vértices de la línea
};
