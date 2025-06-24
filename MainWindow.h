#pragma once
// ¡IMPORTANTE! Define NOMINMAX antes de cualquier include de Windows o DirectX.
#define NOMINMAX 
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "Resources/resource.h"
#include "MainController.h"
#include "Utils.h"
#include "Devices/Mouse.h"


#define WM_GAME_UPDATE (WM_USER + 1)

class MainWindow
{
public:
	MainWindow(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow);
	~MainWindow();

	HRESULT Init( HINSTANCE hInstance, int nCmdShow, int width, int height);
	int Create(int width, int height);
	void ToggleFullscreen();

	Mouse* GetMouse() const { return mouse; }
	void SetKeyboard(std::shared_ptr<Keyboard> keyboard) { g_keyboard = keyboard; }
	std::shared_ptr<Keyboard> GetKeyboard() { return g_keyboard; }

	MainController*	g_controller;
	HWND			g_hWnd;

private:
	Mouse*							mouse;
	std::shared_ptr<Keyboard>		g_keyboard;
	HINSTANCE						g_hInstance;
	HINSTANCE						g_hPrevInstance;
	LPWSTR							g_lpCmdLine;
	int								g_nCmdShow;
};
