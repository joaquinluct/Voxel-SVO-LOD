#include "Engine.h"
#include <InitManager.h>
#include <Locators/ManagerLocator/ManagerLocator.h>
#include <MainManagers/InputManager.h>
#include <Managers/DeviceManager.h>
#include <Managers/SceneManager.h>
#include <Managers/UpdateManager.h>
#include <RenderManager/RenderManager.h>
#include <ServiceLocator/ServiceLocator.h>
#include <Services/ThreadPool.h>
#include <Util/DirectXDebug.h>
//--------------------------------------------------------------------------------------
// Constructor: Inicializa las variables miembro.
//--------------------------------------------------------------------------------------
Engine::Engine() :
    m_hWnd(nullptr),
    m_deviceManager(nullptr),
    m_inputManager(nullptr),
    m_renderManager(nullptr),
    m_updateManager(nullptr),
    m_threadPool(nullptr),
    m_updateThread(nullptr),
    m_renderThread(nullptr),
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
bool Engine::InitEngine(EngineContext* context) {
    InitManager* initManager = new InitManager();

    HRESULT hr = initManager->Init(context);

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
    //if (FAILED(m_updateManager->Init())) return false;
    m_sceneManager = ManagerLocator::GetManager<SceneManager>();
    if (!m_sceneManager) return false;
    m_sceneManager->Init(m_renderManager->Context());
    m_sceneManager->PostInit();
    m_updateManager->PostInit();

    return true;
}

//--------------------------------------------------------------------------------------
// Inicializa el ThreadPool y los hilos de actualización y renderizado.
//--------------------------------------------------------------------------------------
bool Engine::InitThreads() {
    // Creación del ThreadPool
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

    if (
        !m_sceneManager->IsInitialized() ||
        !m_updateManager->IsInitialized() ||
        !m_renderManager->IsInitialized()
        ) {
        OutputDebugStringA("[Engine] Thread - ERROR - Managers no inicializados.\n");
        return false;
    }

    // Inicia los hilos de actualización y renderizado. (bucles principales separados)
    /*m_updateThread = std::make_unique<std::thread>(&Engine::UpdateLoop, this);
    m_renderThread = std::make_unique<std::thread>(&Engine::RenderLoop, this);*/
    m_sceneManager->Start();
    m_updateManager->Start();
    m_renderManager->Start();

    return true;
}

//--------------------------------------------------------------------------------------
// Inicializa los recursos del motor.
//--------------------------------------------------------------------------------------
bool Engine::Init(EngineContext* context) {

    m_context = context;
    OutputDebugStringA("[Engine] Comenzando la inicialización.\n");

    if (!InitEngine(context)) {
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

    // -----------------------------------------------------------------
    // Inicializa los hilos de actualización y renderizado.
    // 
    //      ¡¡¡¡¡AQUI SE INICLIZA REALMENTE LA APLICACIÓN!!!!!
    // 
    // -----------------------------------------------------------------
    if (!InitThreads()) {
        OutputDebugStringA("[Engine] Threads - ERROR.\n");
        return false;
    }

    OutputDebugStringA("[Engine] Threads - OK.\n");
    OutputDebugStringA("[Engine] Running...\n");

    return true;
}

//--------------------------------------------------------------------------------------
// Maneja la entrada de la ventana.
//--------------------------------------------------------------------------------------
const float MAX_DELTA_TIME = 0.1f;
const float MIN_DELTA_TIME = 0.042f;
void Engine::OnInput(UINT message, WPARAM wParam, LPARAM lParam, float  deltaTime) {
    // Pasa el mensaje de Windows al InputManager.
    if (m_inputManager) {
        //float deltaTime = m_context->deltaTime;
        if (deltaTime > MAX_DELTA_TIME) {
            deltaTime = MAX_DELTA_TIME;
        }
        else if (deltaTime < MIN_DELTA_TIME) {
            deltaTime = MIN_DELTA_TIME;
        }
        m_inputManager->HandleInput(message, wParam, lParam, deltaTime);
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

// CONTROL DE HILOS

void Engine::Pause() {
    m_context->isRunning = false;
    m_sceneManager->Pause();
    m_updateManager->Pause();
    m_renderManager->Pause();
}

void Engine::Resume() {
    m_context->isRunning = true;
    m_sceneManager->Resume();
    m_updateManager->Resume();
    m_renderManager->Resume();
}


//--------------------------------------------------------------------------------------
// Bucle del hilo de actualización.
//--------------------------------------------------------------------------------------
//void Engine::UpdateLoop() {
//    while (!m_isShutdown) {
//        // Calcula el tiempo transcurrido desde el último frame
//        auto currentTime = std::chrono::high_resolution_clock::now();
//        m_deltaTime = currentTime - m_lastTime;
//        m_lastTime = currentTime;
//
//        // Inicializar las tareas
//		m_sceneManager->Update(m_deltaTime.count());
//
//        // Llama a la lógica de actualización del motor a través del UpdateManager
//        m_updateManager->Update(m_deltaTime.count());
//
//        // Pequeña pausa para evitar el uso excesivo de la CPU
//        std::this_thread::sleep_for(std::chrono::milliseconds(1));
//    }
//}

//--------------------------------------------------------------------------------------
// Bucle del hilo de renderizado.
//--------------------------------------------------------------------------------------
//void Engine::RenderLoop() {
//    while (!m_isShutdown) {
//        // Llama a la lógica de renderizado del motor a través del RenderManager
//        m_renderManager->Render();
//    }
//}

//--------------------------------------------------------------------------------------
// Apaga los hilos de forma segura.
//--------------------------------------------------------------------------------------
void Engine::ShutdownThreads() {
    m_context->isRunning = false;

    m_sceneManager->Stop();
    m_updateManager->Stop();
    m_renderManager->Stop();

    /*if (m_updateThread && m_updateThread->joinable()) {
        m_updateThread->join();
    }
    if (m_renderThread && m_renderThread->joinable()) {
        m_renderThread->join();
    }*/
}

void Engine::Shutdown() {
    ShutdownThreads();
    if (m_sceneManager) {
        m_sceneManager->Shutdown();
        m_sceneManager = nullptr;
    }
    if (m_threadPool) {
        m_threadPool->Shutdown();
        m_threadPool = nullptr;
    }
    if (m_renderManager) {
        m_renderManager->Shutdown();
        m_renderManager = nullptr;
    }
    if (m_updateManager) {
        m_updateManager->Shutdown();
        m_updateManager = nullptr;
    }
    if (m_inputManager) {
        m_inputManager->Shutdown();
        m_inputManager = nullptr;
    }
    if (m_deviceManager) {
        m_deviceManager->Shutdown();
        m_deviceManager = nullptr;
    }

    ReportLiveObjects();

    OutputDebugStringA("[Engine] Shutdown complete.\n");
}
