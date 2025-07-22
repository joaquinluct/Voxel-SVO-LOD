#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "../../src/Engine/Core/Material/Material.h"
#include "../../RenderObjects/UIBox/UIBox.h"
#include "../../RenderObjects/Line/Line.h"

//class Line; // Forward declaration
//class UIBox; // Forward declaration


class Axis {
public:
    Axis(Material* material);
    ~Axis();

    HRESULT Init(ID3D11Device* device);
    void Render(ID3D11DeviceContext* context);
    void Release();
    ID3D11Buffer* GetVertexBuffer() { return nullptr; }
	//void SetKeyboardManager(KeyboardManager* keyboardManager) override {}

private:
    Material* m_material;
    std::vector<Line*> m_lines; // Usar un vector para almacenar las líneas
    std::vector<UIBox*> boxes;
};