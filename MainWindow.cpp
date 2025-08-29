#include "MainWindow.h"
#include <winerror.h>
#include <Services/FrameStateService.h>

const float MAX_DELTA_TIME = 0.05f;

MainWindow::MainWindow(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
	: g_hInstance(hInstance), g_hPrevInstance(hPrevInstance), g_lpCmdLine(lpCmdLine), g_nCmdShow(nCmdShow), g_hWnd(nullptr), g_controller(nullptr), g_mouse(nullptr), g_keyboard(nullptr) 
{   
}

MainWindow::~MainWindow() {
	SafeShutDown(g_controller);
}

//void MainWindow::UpdateLoop() {
//    using namespace std::chrono;
//    const auto fixedDeltaTime = 16ms;
//    auto lastUpdateTime = high_resolution_clock::now();
//
//    // Obtener el servicio una sola vez
//    auto renderStateService = ServiceLocator::GetService<RenderStateService>();
//
//    while (isRunning) {
//        auto currentTime = high_resolution_clock::now();
//        auto deltaTime = currentTime - lastUpdateTime;
//        if (deltaTime >= fixedDeltaTime) {
//            // 1. Llama al método Update del controlador (lógica de juego)
//            float dt = static_cast<float>(duration_cast<milliseconds>(fixedDeltaTime).count()) / 1000.0f;
//            g_controller->Update(dt);
//
//            // 4. Intercambia los buffers, sincronizando ambos hilos
//            //g_renderStateService->SwapBuffers);
//
//            lastUpdateTime = currentTime;
//        }
//    }
//}

// La lógica de tu bucle de renderizado
//void MainWindow::RenderLoop() {
//    while (isRunning) {
//        // Llama al método Render del controlador, pasando el estado
//        g_controller->Render();
//    }
//}

//int MainWindow::Create(int width, int height)
//{
//    UNREFERENCED_PARAMETER(g_hPrevInstance);
//    UNREFERENCED_PARAMETER(g_lpCmdLine);
//
//    if (FAILED(Init(g_hInstance, g_nCmdShow, width, height)))
//        return 0;
//
//    g_controller = new MainController(g_hWnd, width, height);
//    g_controller->Initialize(g_hWnd, width, height);
//
//    g_keyboard = g_controller->GetKeyboard();
//    g_mouse = g_controller->GetMouse();
//
//    SetKeyboard(g_keyboard);
//    SetMouse(g_mouse);
//
//    // Inicia el hilo de Update
//    std::thread updateThread(&MainWindow::UpdateLoop, this);
//
//    // Inicia el hilo de Render
//    std::thread renderThread(&MainWindow::RenderLoop, this);
//
//    MSG msg = { 0 };
//    // El bucle del hilo principal solo procesa mensajes.
//    while (WM_QUIT != msg.message)
//    {
//        // El hilo principal solo tiene la tarea de procesar eventos de Windows
//        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
//        {
//            TranslateMessage(&msg);
//            DispatchMessage(&msg);
//        }
//        else
//        {
//            // Opcional: Puedes usar Sleep o Yield para no consumir recursos de CPU
//            // cuando no hay mensajes.
//            Sleep(1);
//        }
//    }
//
//    // Detiene los hilos cuando la ventana se cierra
//    isRunning = false;
//    updateThread.join();
//    renderThread.join();
//
//
//    // Limpieza final
//    SafeShutDown(g_controller);
//
//    return (int)msg.wParam;
//}
    

int MainWindow::Create(int width, int height)
{

    UNREFERENCED_PARAMETER(g_hPrevInstance);
    UNREFERENCED_PARAMETER(g_lpCmdLine);

    if (FAILED(Init(g_hInstance, g_nCmdShow, width, height)))
        return 0;

    g_controller = new MainController(g_hWnd, width, height);
    g_controller->Initialize(g_hWnd, width, height);

	g_keyboard = g_controller->GetKeyboard(); // Obtener el teclado del controlador
	g_mouse = g_controller->GetMouse(); // Obtener el ratón del controlador

	SetKeyboard(g_keyboard); // Establecer el teclado en el controlador
    SetMouse(g_mouse);

    //if (FAILED(g_controller->Init(g_hWnd, keyboard))) // Pasar el puntero correctamente
    //{
    //    SafeRelease(g_controller);
    //    return 0;
    //}

    //mouse = new Mouse(g_controller->GetCamera(), g_hWnd, width, height);

        // Inicializar Mouse
    /*if (FAILED(mouse->Init()))
    {
        SafeRelease(g_controller);
        return 0;
    }*/
    //SetTimer(g_hWnd, 1, 16, nullptr); // Configura el temporizador (16ms ~ 60 FPS)

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
            if (deltaTime > MAX_DELTA_TIME) {
                deltaTime = MAX_DELTA_TIME;
            }

            previousTime = currentTime;

            
            g_controller->Update(deltaTime); // Llama a Update
            g_controller->Render();


            //InvalidateRect(g_hWnd, NULL, TRUE); // Fuerza a repintar toda la ventana
            //UpdateWindow(g_hWnd); // Sincroniza el repintado
        }
    }

    //mouse->Release();   // Liberar Mouse
    //keyboard->Release(); // Liberar Keyboard

    SafeShutDown(g_controller);

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
        if (wParam == VK_ESCAPE) {
            PostQuitMessage(0);
            return 0;
        }
        if (pMainWindow) {
            pMainWindow->GetKeyboard()->SetKey(static_cast<unsigned char>(wParam), true);
            //pMainWindow->GetKeyboard()->Update(wParam);
			if (wParam == VK_F11) {
				pMainWindow->ToggleFullscreen();
			}
        }        
        break;
    case WM_KEYUP:
        if (pMainWindow) {
            pMainWindow->GetKeyboard()->SetKey(static_cast<unsigned char>(wParam), false);
            //pMainWindow->GetKeyboard()->Update(wParam);
        }
		break;
    case WM_MOUSEMOVE:
        if (pMainWindow) {
			std::shared_ptr<Mouse> mouse = pMainWindow->GetMouse();
            if (mouse) {
                mouse->Update(0); // Actualizar el ratón
			}
        }
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