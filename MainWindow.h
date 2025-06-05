#pragma once
#include <windows.h>
#include "MainGame.h"
#include "Util/Utils.h"
#include <Windows.h>

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
	Keyboard* GetKeyboard() const { return keyboard; }

	MainGame*		g_controller;
	HWND			g_hWnd;

private:
	Mouse*			mouse;
	Keyboard*		keyboard;
	HINSTANCE		g_hInstance;
	HINSTANCE		g_hPrevInstance;
	LPWSTR			g_lpCmdLine;
	int				g_nCmdShow;
};
