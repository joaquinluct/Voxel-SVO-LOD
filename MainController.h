#pragma once
#include <windows.h>
#include "Managers/DeviceManager.h"
#include "Managers/RenderTargetManager.h"
#include "Camera/Camera.h"
#include "Managers/ShaderManager.h"
#include "Material/Material.h"
#include "RenderObjects/Triangulo.h"
#include "RenderObjects/Axis/Axis.h"


class MainController {
public:
    MainController();
    HRESULT Init(HWND hwnd);
    void Cleanup();
    void Render();
    struct MatrixBufferType {
        XMMATRIX worldMatrix;
        XMMATRIX viewMatrix;
        XMMATRIX projectionMatrix;
    };

private:
    HWND g_hwnd;
    DeviceManager* g_device;
    RenderTargetManager* g_renderTarget;
    Camera* g_camera;
    ShaderManager* g_shaderManager;
    Material* g_material;
    Triangulo* g_triangulo;
    ID3D11Buffer* g_matrixBuffer;
	Axis* g_axis;
};
