#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "../Line/Line.h"
#include "../UIBox/UIBox.h"

using namespace DirectX;

class Axis : public iDrawable {
public:
    Axis(Material* material);
    ~Axis();

    HRESULT Init(ID3D11Device* device) override;
    void Render(ID3D11DeviceContext* context) override;
    void Release() override;
    ID3D11Buffer* GetVertexBuffer() override { return nullptr; }
	//void SetKeyboardManager(KeyboardManager* keyboardManager) override {}

private:
    Material* m_material;
    std::vector<Line*> m_lines; // Usar un vector para almacenar las líneas
    std::vector <UIBox*> boxes;
};