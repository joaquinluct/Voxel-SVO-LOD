#include "MainWindow.h"
#include <../Resources/resource.h>
#include <Defines/EngineDefinition.h>
#include <memory>
#include <profileapi.h>
#include <thread>
#include <chrono>
#include <Windows.h>

// -------------------------------------------------------------------------------
// Límite máximo para el tiempo delta para evitar saltos grandes en la simulación.
// -------------------------------------------------------------------------------
const float MAX_DELTA_TIME = 0.052f;
const float MIN_DELTA_TIME = 0.042f;

//--------------------------------------------------------------------------------------
// Variable estática para el puntero a la instancia de MainWindow.
//--------------------------------------------------------------------------------------
MainWindow* MainWindow::s_pMainWindow = nullptr;

//--------------------------------------------------------------------------------------
// Constructor
//--------------------------------------------------------------------------------------
MainWindow::MainWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
    : m_hInstance(hInstance), m_nCmdShow(nCmdShow) {
    // Almacena un puntero a esta instancia para que la función de procedimiento de ventana estática
    // pueda acceder a los métodos de la clase.
    s_pMainWindow = this;
}

//--------------------------------------------------------------------------------------
// Implementación de la creación de la ventana y el bucle principal.
//--------------------------------------------------------------------------------------
int MainWindow::Create(int width, int height) {

    OutputDebugStringA("[MainWindow] Creando aplicación...\n");
    if (!InitializeWindow(width, height)) {
        return 1;
    }
    OutputDebugStringA("[MainWindow] Inicializando aplicación...\n");

    // Se inicializa la instancia del motor de juego.
    // ----------------------------------------------
    m_gameEngine = std::make_unique<Engine>();
    OutputDebugStringA("[MainWindow] Aplicación creada - OK\n");

    // Inicializa la variable de tiempo para el bucle principal.
    // ---------------------------------------------------------
    LARGE_INTEGER frequency, previousTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&previousTime);
    OutputDebugStringA("[MainWindow] Bucle de Aplicación ...\n");
    MSG msg = { 0 };

    // ----------------------------------
    // Iniciar la aplicación.
    // ----------------------------------
    m_context = new EngineContext();
    m_context->width = static_cast<float>(width);
    m_context->height = static_cast<float>(height);
    m_context->hWnd = &m_hWnd;
    m_context->hInstance = m_hInstance;
    m_context->nCmdShow = m_nCmdShow;

    m_context->isRunning = m_gameEngine->Init(m_context);

    if (!m_context->isRunning) {
        return 1;
    }

    // ---------------------------------------------------------------
    // Usar MainLoop AAA por defecto: consolidamos input/update/scene
    // en el hilo principal y dejamos solo el render thread aparte.
    // ---------------------------------------------------------------
    OutputDebugStringA("[MainWindow] Usando MainLoop AAA por defecto.\n");
    // MainLoop se encarga de procesar mensajes y ejecutar la lógica del juego en el hilo principal.
    m_gameEngine->MainLoop();
    m_context->isRunning = false;
    OutputDebugStringA("[MainWindow] Finalizando Aplicación ...\n");
    return (int)msg.wParam;
}

//--------------------------------------------------------------------------------------
// Método privado para la creación de la ventana.
//--------------------------------------------------------------------------------------
//bool MainWindow::InitializeWindow(int width, int height) {
//    WNDCLASSEX wcex = { 0 };
//
//    wcex.cbSize = sizeof(WNDCLASSEX);
//    wcex.style = CS_HREDRAW | CS_VREDRAW;
//    wcex.lpfnWndProc = StaticWndProc; // Se usa el procedimiento de ventana estático.
//    wcex.hInstance = m_hInstance;
//    wcex.lpszMenuName = nullptr;
//    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
//    wcex.lpszClassName = L"EngineWindowClass";
//    if (!RegisterClassEx(&wcex)) {
//        return false;
//    }
//
//    m_hWnd = CreateWindowEx(0, L"EngineWindowClass", L"Engine", WS_OVERLAPPEDWINDOW,
//        CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, m_hInstance, nullptr);
//    if (!m_hWnd) {
//        return false;
//    }
//
//    //ShowWindow(m_hWnd, m_nCmdShow);
//    ShowWindow(m_hWnd, SW_MAXIMIZE);
//    UpdateWindow(m_hWnd);
//
//    return true;
//}

bool MainWindow::InitializeWindow(int width, int height) {
    // Register class
    OutputDebugStringA("[MainWindow] Inicializando ventana...\n");
    WNDCLASSEX wcex{};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = StaticWndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = m_hInstance;
    wcex.hIcon = LoadIcon(m_hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"MainWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return false;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Create window    
    RECT rc = { 0, 0, screenWidth, screenHeight };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    m_hWnd = CreateWindow(L"MainWindowClass", L"Direct3D 11 Voxelado",
        /*WS_EX_OVERLAPPEDWINDOW | WS_SYSMENU | WS_MAXIMIZE,*/
        WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, m_hInstance,
        nullptr);

    if (!m_hWnd) {
        MessageBox(nullptr, L"Error al crear la ventana", L"Error", MB_OK);
        return false;
    }

    //SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);

    ShowWindow(m_hWnd, m_nCmdShow);
    UpdateWindow(m_hWnd);
    OutputDebugStringA("[MainWindow] Ventana inicalizada - OK\n");
    return true;
}

//--------------------------------------------------------------------------------------
// Procedimiento de ventana estático que reenvía los mensajes a la instancia.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MainWindow::StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (s_pMainWindow) {
        return s_pMainWindow->WndProc(hWnd, message, wParam, lParam);
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

//--------------------------------------------------------------------------------------
// Procedimiento de ventana de instancia.
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MainWindow::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_SIZE:
        // Llama al método de redimensionamiento del motor.
        OnResize(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MOUSEMOVE:
        if (wParam == VK_ESCAPE) {
            m_gameEngine->Shutdown();
            PostQuitMessage(0);
            return 0;
        }
        // Pasa los mensajes de entrada al motor de juego.
        HandleInput(message, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//--------------------------------------------------------------------------------------
// Maneja la entrada de la ventana.
//--------------------------------------------------------------------------------------
void MainWindow::HandleInput(UINT message, WPARAM wParam, LPARAM lParam) {
    if (m_gameEngine) {
        m_gameEngine->OnInput(message, wParam, lParam, this->m_deltaTime);
    }
}

//--------------------------------------------------------------------------------------
// Maneja el redimensionamiento de la ventana.
//--------------------------------------------------------------------------------------
void MainWindow::OnResize(int width, int height) {
    if (m_gameEngine) {
        m_context->width = width;
        m_context->height = height;
        m_gameEngine->OnResize(width, height);
    }
}
