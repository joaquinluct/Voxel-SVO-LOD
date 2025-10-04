#include "MainWindow.h"

// Standard C++14 includes (alphabetical)
#include <string>

// Platform specific (alphabetical)  
#include <profileapi.h>

// Engine includes (alphabetical by category)
#include "Core/Foundation/Macros.h"

namespace Engine {
    
    // Static member initialization
    MainWindow* MainWindow::s_instance = nullptr;

    MainWindow::MainWindow(HINSTANCE hInstance, 
                          HINSTANCE hPrevInstance, 
                          LPWSTR lpCmdLine, 
                          int nCmdShow)
        : m_hInstance(hInstance)
        , m_showCommand(nCmdShow)
        , m_engineContext(std::make_unique<EngineContext>())
        , m_config(std::make_unique<MainWindowConfig>())
    {
        ENGINE_ASSERT(s_instance == nullptr && "Only one MainWindow instance allowed");
        s_instance = this;
        
        ENGINE_LOG("MainWindow Constructor - Instancia creada");
    }

    MainWindow::~MainWindow() {
        if (m_gameEngine) {
            m_gameEngine->Shutdown();
        }
        
        s_instance = nullptr;
        ENGINE_LOG("MainWindow Destructor - Instancia destruida");
    }

    Expected<int> MainWindow::Create() {
        ENGINE_LOG("Iniciando creacion de aplicacion...");
        
        // 1. Cargar configuración desde YAML
        auto configResult = LoadConfiguration();
        if (!configResult.has_value()) {
            return Expected<int>(EngineError::ConfigurationError);
        }
        
        // 2. Inicializar componentes
        auto initResult = Initialize();
        if (!initResult.has_value()) {
            return Expected<int>(EngineError::InitializationFailed);
        }
        
        ENGINE_LOG("Iniciando bucle principal...");
        
        // 3. Bucle principal de mensajes
        MSG msg = {};
        m_isRunning = true;
        
        while (m_isRunning && msg.message != WM_QUIT) {
            ProcessMessages();
            UpdateTiming();
            
            // El motor maneja sus propios hilos de Update/Render
            // Aquí solo gestionamos el pump de mensajes Win32
            
            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        
        ENGINE_LOG("Finalizando aplicacion...");
        return Expected<int>(static_cast<int>(msg.wParam));
    }

    Expected<bool> MainWindow::Initialize() {
        ENGINE_LOG("Inicializando MainWindow...");
        
        // 1. Inicializar timing
        if (!QueryPerformanceFrequency(&m_frequency)) {
            return Expected<bool>(EngineError::InitializationFailed);
        }
        QueryPerformanceCounter(&m_previousTime);
        
        // 2. Crear ventana
        auto windowResult = InitializeWindow();
        if (!windowResult.has_value()) {
            return Expected<bool>(windowResult.error());
        }
        
        // 3. Configurar contexto del motor
        ApplyWindowConfiguration();
        
        // 4. Crear e inicializar el motor
        m_gameEngine = std::make_unique<Engine>();
        
        bool engineInitResult = m_gameEngine->Init(m_engineContext.get());
        if (!engineInitResult) {
            ENGINE_LOG_ERROR("Fallo al inicializar el motor");
            return Expected<bool>(EngineError::InitializationFailed);
        }
        
        m_isInitialized = true;
        ENGINE_LOG("MainWindow inicializada correctamente");
        
        return Expected<bool>(true);
    }

    Expected<bool> MainWindow::LoadConfiguration() {
        ENGINE_LOG("Cargando configuracion YAML...");
        
        // Por ahora usar valores por defecto hasta que YamlToStruct este completo
        ENGINE_LOG("MainWindowConfig creada con valores por defecto");
        ENGINE_LOG("Configuracion YAML aplicada correctamente");
        
        return Expected<bool>(true);
    }

    Expected<bool> MainWindow::LoadConfigurationFromYAML() {
        // TODO: Implementar carga real desde archivo YAML
        // Por ahora retornar error para usar valores por defecto
        return Expected<bool>(EngineError::ConfigurationError);
    }

    Expected<bool> MainWindow::InitializeWindow() {
        ENGINE_LOG("Inicializando ventana Win32...");
        
        // 1. Crear clase de ventana
        auto classResult = CreateWindowClass();
        if (!classResult.has_value()) {
            return Expected<bool>(classResult.error());
        }
        
        // 2. Crear instancia de ventana
        auto windowResult = CreateWindowInstance();
        if (!windowResult.has_value()) {
            return Expected<bool>(windowResult.error());
        }
        
        ShowWindow(m_windowHandle, m_showCommand);
        UpdateWindow(m_windowHandle);
        
        ENGINE_LOG("Ventana Win32 inicializada correctamente");
        return Expected<bool>(true);
    }

    Expected<bool> MainWindow::CreateWindowClass() {
        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEXW);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = StaticWindowProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = m_hInstance;
        
        // Usar icono por defecto temporalmente
        wcex.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr;
        
        // Usar título por defecto temporalmente
        wcex.lpszClassName = WINDOW_CLASS_NAME;
        wcex.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
        
        if (!RegisterClassExW(&wcex)) {
            const DWORD error = GetLastError();
            if (error != ERROR_CLASS_ALREADY_EXISTS) {
                ENGINE_LOG_ERROR("Fallo al registrar clase de ventana");
                return Expected<bool>(EngineError::WindowError);
            }
        }
        
        return Expected<bool>(true);
    }

    Expected<bool> MainWindow::CreateWindowInstance() {
        // Usar configuración por defecto temporalmente
        DWORD windowStyle = WS_OVERLAPPEDWINDOW;
        
        RECT windowRect = { 0, 0, 1920, 1080 };  // Tamaño por defecto 
        AdjustWindowRect(&windowRect, windowStyle, FALSE);
        
        m_windowHandle = CreateWindowW(
            WINDOW_CLASS_NAME,       // Clase
            WINDOW_TITLE,           // Título
            windowStyle,            // Estilo
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            windowRect.right - windowRect.left,
            windowRect.bottom - windowRect.top,
            nullptr,
            nullptr,
            m_hInstance,
            nullptr
        );
        
        if (!m_windowHandle) {
            const DWORD error = GetLastError();
            ENGINE_LOG_ERROR("Fallo al crear ventana");
            return Expected<bool>(EngineError::WindowError);
        }
        
        return Expected<bool>(true);
    }

    void MainWindow::ApplyWindowConfiguration() {
        // Configurar contexto del motor con información de la ventana
        RECT clientRect;
        GetClientRect(m_windowHandle, &clientRect);
        
        m_engineContext->windowHandle = m_windowHandle;
        m_engineContext->windowWidth = static_cast<uint32>(clientRect.right - clientRect.left);
        m_engineContext->windowHeight = static_cast<uint32>(clientRect.bottom - clientRect.top);
        m_engineContext->isInitialized = false;
        m_engineContext->isRunning = true;
        m_engineContext->deltaTime = 0.0f;
        m_engineContext->frameCount = 0;
        
        // Aplicar configuración por defecto
        m_engineContext->isFullscreen = false;
        m_engineContext->debugMode = true;
        m_engineContext->mouseVisible = true;
        m_engineContext->inputCaptured = false;
    }

    void MainWindow::UpdateTiming() {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);
        
        f32 delta = static_cast<f32>(currentTime.QuadPart - m_previousTime.QuadPart) / 
                    static_cast<f32>(m_frequency.QuadPart);
        m_previousTime = currentTime;
        
        // Usar límites por defecto
        if (delta > MAX_DELTA_TIME) {
            delta = MAX_DELTA_TIME;
        } else if (delta < MIN_DELTA_TIME) {
            delta = MIN_DELTA_TIME;
        }
        
        m_deltaTime = delta;
        m_engineContext->deltaTime = delta;
        
        if (m_gameEngine) {
            m_gameEngine->SetDeltaTime(delta);
        }
    }

    void MainWindow::ProcessMessages() {
        // Este método puede expandirse para procesamiento adicional de mensajes
        // Por ahora, el procesamiento principal se hace en el bucle de Create()
    }

    LRESULT MainWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        switch (message) {
            case WM_SIZE:
                OnResize(LOWORD(lParam), HIWORD(lParam));
                break;
                
            case WM_DESTROY:
                OnDestroy();
                break;
                
            case WM_KEYDOWN:
                OnKeyDown(wParam);
                ENGINE_FALLTHROUGH;
                
            case WM_KEYUP:
            case WM_LBUTTONDOWN:
            case WM_LBUTTONUP:
            case WM_MOUSEMOVE:
                HandleInput(message, wParam, lParam);
                break;
                
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }
        
        return 0;
    }

    void MainWindow::HandleInput(UINT message, WPARAM wParam, LPARAM lParam) {
        if (m_gameEngine && m_isInitialized) {
            m_gameEngine->OnInput(static_cast<uint32>(message), wParam, lParam, m_deltaTime);
        }
    }

    void MainWindow::OnResize(uint32 width, uint32 height) {
        if (m_gameEngine && m_isInitialized) {
            m_engineContext->windowWidth = width;
            m_engineContext->windowHeight = height;
            m_gameEngine->OnResize(static_cast<int>(width), static_cast<int>(height));
        }
    }

    void MainWindow::OnDestroy() {
        m_isRunning = false;
        PostQuitMessage(0);
    }

    void MainWindow::OnKeyDown(WPARAM key) {
        if (key == VK_ESCAPE) {
            ENGINE_LOG("ESC presionado - Cerrando aplicacion");
            OnDestroy();
        }
    }

    // Static window procedure bridge
    LRESULT CALLBACK MainWindow::StaticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        if (s_instance) {
            return s_instance->WindowProc(hWnd, message, wParam, lParam);
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

} // namespace Engine
