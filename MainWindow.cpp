#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "MainWindow.h"
#include "Resources/resource.h"
#include "MainGame.h"

MainWindow::MainWindow(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
	: g_hInstance(hInstance), g_hPrevInstance(hPrevInstance), g_lpCmdLine(lpCmdLine), g_nCmdShow(nCmdShow), g_hWnd(nullptr), g_controller(nullptr), mouse(nullptr), keyboard(nullptr) 
{
    g_controller = new MainGame();
}

MainWindow::~MainWindow() {
	SafeRelease(g_controller);
}

int MainWindow::Create(int width, int height)
{

    UNREFERENCED_PARAMETER(g_hPrevInstance);
    UNREFERENCED_PARAMETER(g_lpCmdLine);

    if (FAILED(Init(g_hInstance, g_nCmdShow, width, height)))
        return 0;

    keyboard = new Keyboard(); // Cambiar la inicialización para usar un puntero dinámico

    if (FAILED(g_controller->Init(g_hWnd, keyboard))) // Pasar el puntero correctamente
    {
        SafeRelease(g_controller);
        return 0;
    }

    mouse = new Mouse(g_controller->GetCamera(), g_hWnd, width, height);

        // Inicializar Mouse
    if (FAILED(mouse->Init()))
    {
        SafeRelease(g_controller);
        return 0;
    }

    keyboard->Init(); // Inicializar Keyboard

    SetTimer(g_hWnd, 1, 16, nullptr); // Configura el temporizador (16ms ~ 60 FPS)

    LARGE_INTEGER frequency;
    LARGE_INTEGER previousTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&previousTime);

    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            LARGE_INTEGER currentTime;
            QueryPerformanceCounter(&currentTime);
            float deltaTime = (float)(currentTime.QuadPart - previousTime.QuadPart) / frequency.QuadPart;
            previousTime = currentTime;

            g_controller->Update(deltaTime); // Llama a Update
            g_controller->Render();
            mouse->Render();   // Actualizar y usar la entrada del ratón
            keyboard->Render(); // Actualizar y usar la entrada del teclado
            //InvalidateRect(g_hWnd, NULL, TRUE); // Fuerza a repintar toda la ventana
            //UpdateWindow(g_hWnd); // Sincroniza el repintado
        }
    }

    mouse->Release();   // Liberar Mouse
    keyboard->Release(); // Liberar Keyboard

    SafeRelease(g_controller);

    return (int)msg.wParam;

}

//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    MainWindow* pMainWindow = (MainWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

    switch (message)
    {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        //if (pMainWindow) {
        //    // 1. Obtener el valor de pitch (asegúrate de que la cámara esté accesible aquí)
        //    int currentPitch = static_cast<int>(pMainWindow->mouse.m_pitch);

        //    // 2. Formatear el texto a dibujar
        //    std::wstring pitchText = L"Pitch: " + std::to_wstring(currentPitch);
        //    /*std::wstring pitchText = L"Test000000000000000000000000000000000000000000000000000000000000000";*/

        //    // 3. Establecer el color del texto (opcional)
        //    SetBkColor(hdc, RGB(0, 0, 255)); // Establecer el fondo a azul
        //    SetTextColor(hdc, ((COLORREF)(((BYTE)(255) | ((WORD)((BYTE)(255)) << 8)) | (((DWORD)(BYTE)(255)) << 16)))); // Blanco

        //    // 4. Establecer el fondo (opcional)
        //    //SetBkMode(hdc, TRANSPARENT); // Sin fondo
        //    SetBkMode(hdc, OPAQUE); // Con fondo

        //    // 5. Dibujar el texto
        //    TextOutW(hdc, 100, 100, pitchText.c_str(), static_cast<int>(pitchText.length())); // Posición 10, 10
        //}

        EndPaint(hWnd, &ps);
        break;

    case WM_KEYDOWN:
        if (pMainWindow) {
            pMainWindow->GetKeyboard()->Update();
            if (wParam == VK_ESCAPE) {
                PostQuitMessage(0);
            }
			if (wParam == VK_F11) {
				pMainWindow->ToggleFullscreen();
			}
        }
        break;
    case WM_KEYUP:
        if (pMainWindow) {
            pMainWindow->GetKeyboard()->Update();
        }
		break;
    case WM_MOUSEMOVE:
        /*if (pMainWindow) {
            pMainWindow->mouse.Update(hWnd);
        }*/
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
        // Note that this tutorial does not handle resizing (WM_SIZE) requests,
        // so we created the window without the resize border.

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

/*
* INICIALIZACIÓN DE LA VENTANA
*/
HRESULT MainWindow::Init( HINSTANCE hInstance, int nCmdShow, int width, int height)
{
    // Register class
    WNDCLASSEX wcex{};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"MainWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Create window
    g_hInstance = hInstance;
    RECT rc = { 0, 0, screenWidth, screenHeight };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    g_hWnd = CreateWindow(L"MainWindowClass", L"Direct3D 11 Voxelado",
        /*WS_EX_OVERLAPPEDWINDOW | WS_SYSMENU | WS_MAXIMIZE,*/
        WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
        nullptr);

    if (!g_hWnd) {
        MessageBox(nullptr, L"Error al crear la ventana", L"Error", MB_OK);
        return E_FAIL;
    }

    SetWindowLongPtr(g_hWnd, GWLP_USERDATA, (LONG_PTR)this);

    ShowWindow(g_hWnd, nCmdShow);

    return S_OK;
}

void MainWindow::ToggleFullscreen() {
    static bool fullscreen = true;
    fullscreen = !fullscreen;

    LONG style = fullscreen ? WS_POPUP : WS_OVERLAPPEDWINDOW;
    SetWindowLong(g_hWnd, GWL_STYLE, style);

    if (fullscreen) {
        int screenWidth = GetSystemMetrics(SM_CXSCREEN);
        int screenHeight = GetSystemMetrics(SM_CYSCREEN);
        SetWindowPos(g_hWnd, nullptr, 0, 0, screenWidth, screenHeight, SWP_FRAMECHANGED);
    }
    else {
        SetWindowPos(g_hWnd, nullptr, 100, 100, 1280, 720, SWP_FRAMECHANGED);
    }
}