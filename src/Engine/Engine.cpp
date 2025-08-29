#include "Engine.h"
#include <iostream>
#include <ServiceLocator/ServiceLocator.h>
#include <InitManager.h>
#include <DeviceManager.h>
#include <UpdateManager.h>
#include <SceneManager.h>
#include <MainManagers/InputManager.h>
#include <RenderManager/RenderManager.h>
#include <Services/ThreadPool.h>

//--------------------------------------------------------------------------------------
// Constructor: Inicializa las variables miembro.
//--------------------------------------------------------------------------------------
Engine::Engine() :
    m_isShutdown(false),
    m_hWnd(nullptr),
    m_lastTime(std::chrono::high_resolution_clock::now()),
	m_deltaTime(std::chrono::duration<float>(0)),
    m_deviceManager(nullptr),
    m_inputManager(nullptr),
    m_renderManager(nullptr),
	m_updateManager(nullptr),
    m_threadPool(nullptr),
    m_updateThread(nullptr),
	m_renderThread(nullptr),
	m_renderCondition(),
    m_sceneManager(nullptr)
{
}

//--------------------------------------------------------------------------------------
// Destructor: Se asegura de que los hilos se detengan correctamente.
//--------------------------------------------------------------------------------------
Engine::~Engine() {
    ShutdownThreads();
}

//--------------------------------------------------------------------------------------
// Inicializa de los componentes globales del motor.
//--------------------------------------------------------------------------------------
bool Engine::InitEngine(HWND* hwnd, int width, int height) {
    InitManager* initManager = new InitManager();

    HRESULT hr = initManager->Init(hwnd, width, height);

    return SUCCEEDED(hr);
}

//--------------------------------------------------------------------------------------
// Inicializa los Managers principales del motor.
//--------------------------------------------------------------------------------------
bool Engine::InitManagers() {

    int width = 1920; // Ejemplo de ancho
    int height = 900; // Ejemplo de alto

    m_deviceManager = ManagerLocator::GetManager<DeviceManager>();
    if (!m_deviceManager) return false;
    m_inputManager = ManagerLocator::GetManager<InputManager>();
    if (!m_inputManager) return false;
    m_renderManager = ManagerLocator::GetManager<RenderManager>();
    if (!m_renderManager) return false;
    m_updateManager = ManagerLocator::GetManager<UpdateManager>();
    if (!m_updateManager) return false;
	if (FAILED(m_updateManager->Init())) return false;
    m_sceneManager = ManagerLocator::GetManager<SceneManager>();
    if (!m_sceneManager) return false;

    return true;
}

//--------------------------------------------------------------------------------------
// Inicializa el ThreadPool y los hilos de actualización y renderizado.
//--------------------------------------------------------------------------------------
bool Engine::InitThreads() {
    // Inicializa el ThreadPool
    m_threadPool = ServiceLocator::GetService<ThreadPool>();
    if (!m_threadPool) {
        OutputDebugStringA("[Engine] Thread - ERROR - Obteniendo el servicio: ThreadPool.\n");
        return false;
    }

	// Inicializa el ThreadPool con el número de hilos basado en la concurrencia del hardware.
	HRESULT hr = m_threadPool->Init(std::thread::hardware_concurrency());
    if (FAILED(hr)) {
        OutputDebugStringA("[Engine] Thread - ERROR - Inicializando ThreadPool.\n");
        return false;
    }

    // Inicia los hilos de actualización y renderizado.
    m_updateThread = std::make_unique<std::thread>(&Engine::UpdateLoop, this);
    m_renderThread = std::make_unique<std::thread>(&Engine::RenderLoop, this);

    return true;
}

//--------------------------------------------------------------------------------------
// Inicializa los recursos del motor.
//--------------------------------------------------------------------------------------
bool Engine::Init(HINSTANCE hInstance, int nCmdShow, HWND* hWnd, int width, int height) {
    
    OutputDebugStringA("[Engine] Comenzando la inicialización.\n");

    if (!InitEngine(hWnd, width, height)) {
        OutputDebugStringA("[Engine] Engine - ERROR.\n");
        return false;
	}

    OutputDebugStringA("[Engine] Engine - OK.\n");
    
    // Inicializa los Managers principales del motor.
    if (!InitManagers()) {
        OutputDebugStringA("[Engine] Managers - ERROR.\n");
        return false;
    }

    OutputDebugStringA("[Engine] Managers - OK.\n");

    // Inicializa los hilos de actualización y renderizado.
    if (!InitThreads()) {
        OutputDebugStringA("[Engine] Threads - ERROR.\n");
        return false;
    }

    OutputDebugStringA("[Engine] Threads - OK.\n");

    return true;
}

//--------------------------------------------------------------------------------------
// Bucle principal para la actualización y renderizado, llamado por MainWindow.
//--------------------------------------------------------------------------------------
void Engine::UpdateAndRender(float deltaTime) {
    // Este método ya no es necesario ya que la lógica de bucle principal
    // se ha movido a los hilos dedicados. Lo mantenemos como un marcador
    // para indicar que el bucle se gestiona internamente.
}

//--------------------------------------------------------------------------------------
// Maneja la entrada de la ventana.
//--------------------------------------------------------------------------------------
void Engine::OnInput(UINT message, WPARAM wParam, LPARAM lParam) {
    // Pasa el mensaje de Windows al InputManager.
    if (m_inputManager) {
        m_inputManager->HandleInput(message, wParam, lParam, m_deltaTime.count());
    }
}

//--------------------------------------------------------------------------------------
// Maneja el redimensionamiento de la ventana.
//--------------------------------------------------------------------------------------
void Engine::OnResize(int width, int height) {
    // Delega el manejo del redimensionamiento al DeviceManager.
    if (m_deviceManager) {
        m_deviceManager->OnResize(width, height);
    }
}

//--------------------------------------------------------------------------------------
// Bucle del hilo de actualización.
//--------------------------------------------------------------------------------------
void Engine::UpdateLoop() {
    while (!m_isShutdown) {
        // Calcula el tiempo transcurrido desde el último frame
        auto currentTime = std::chrono::high_resolution_clock::now();
        m_deltaTime = currentTime - m_lastTime;
        m_lastTime = currentTime;

        // Inicializar las tareas
        m_updateManager->StartThreads(m_deltaTime.count());

        // Llama a la lógica de actualización del motor a través del UpdateManager
        m_updateManager->Update(m_deltaTime.count());

        // Pequeña pausa para evitar el uso excesivo de la CPU
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

//--------------------------------------------------------------------------------------
// Bucle del hilo de renderizado.
//--------------------------------------------------------------------------------------
void Engine::RenderLoop() {
    while (!m_isShutdown) {
        // Llama a la lógica de renderizado del motor a través del RenderManager
        m_renderManager->Render();
    }
}

//--------------------------------------------------------------------------------------
// Apaga los hilos de forma segura.
//--------------------------------------------------------------------------------------
void Engine::ShutdownThreads() {
    m_isShutdown = true;

    // Notifica a todos los hilos en espera.
    m_renderCondition.notify_all();

    if (m_updateThread && m_updateThread->joinable()) {
        m_updateThread->join();
    }
    if (m_renderThread && m_renderThread->joinable()) {
        m_renderThread->join();
    }
}
