//--------------------------------------------------------------------------------------
// File: Main.cpp
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "MainWindow.h"
//#include "MarchingCubes.h"
//#include "Camera.h"
//#include "Common.h"
//#include "UIManager.h"

using namespace DirectX;


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    MainWindow* wnd = new MainWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    return wnd->Create(1920, 900);
};