#include "Engine.h"
#include <chrono>
#include <InitManager.h>
#include <Locators/ManagerLocator/ManagerLocator.h>
#include <MainManagers/InputManager.h>
#include <Managers/DeviceManager.h>
#include <Managers/SceneManager.h>
#include <Managers/UpdateManager.h>
#include <mutex>
#include <RenderManager/RenderManager.h>
#include <ServiceLocator/ServiceLocator.h>
#include <Services/ThreadPool.h>
#include <Util/DirectXDebug.h>

// NUEVO: Includes para sistemas AAA
// #include <Systems/UpdateSystem.h>     // Se creará después
// #include <Systems/SceneSystem.h>      // Se creará después
// #include <Systems/RenderCommandSystem.h>  // Se creará después
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
    //m_updateThread(nullptr),
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
// NUEVO: Inicializa threading AAA - Solo Render Thread + ThreadPool
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

    if (m_useNewThreading) {
        //--------------------------------------------------------------------------------------
        // NUEVA ARQUITECTURA AAA: Solo Render Thread
        //--------------------------------------------------------------------------------------
        OutputDebugStringA("[Engine] Thread - Usando Nueva Arquitectura AAA.\n");

        // Inicializar sistemas AAA (sin threads propios)
        if (!InitAAASystems()) {
            OutputDebugStringA("[Engine] Thread - ERROR - Inicializando Sistemas AAA.\n");
            return false;
        }

        // Solo iniciar render thread
        m_renderThread = std::make_unique<std::thread>(&Engine::RenderLoop, this);

        OutputDebugStringA("[Engine] Thread - Render Thread iniciado (AAA).\n");
    }
    else {
        //--------------------------------------------------------------------------------------
        // ARQUITECTURA LEGACY: Para compatibilidad durante migración
        //--------------------------------------------------------------------------------------
        OutputDebugStringA("[Engine] Thread - Usando Arquitectura Legacy.\n");

        if (!m_sceneManager->IsInitialized() ||
            !m_updateManager->IsInitialized() ||
            !m_renderManager->IsInitialized()) {
            OutputDebugStringA("[Engine] Thread - ERROR - Managers no inicializados.\n");
            return false;
        }

        // Inicia los hilos legacy
        m_sceneManager->Start();
        m_updateManager->Start();
        m_renderManager->Start();
    }

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
// ACTUALIZADO: Apaga los hilos de forma segura (AAA + Legacy)
//--------------------------------------------------------------------------------------
void Engine::ShutdownThreads() {
    m_context->isRunning = false;

    if (m_useNewThreading) {
        //--------------------------------------------------------------------------------------
        // NUEVO: Shutdown AAA Threading
        //--------------------------------------------------------------------------------------
        OutputDebugStringA("[Engine] Shutdown - Nueva Arquitectura AAA.\n");

        // Notificar al render thread que termine
        m_renderCondition.notify_all();

        // Esperar a que termine el render thread
        if (m_renderThread && m_renderThread->joinable()) {
            m_renderThread->join();
        }

        OutputDebugStringA("[Engine] Shutdown - Render Thread terminado.\n");

    }
    else {
        //--------------------------------------------------------------------------------------
        // LEGACY: Shutdown old threading
        //--------------------------------------------------------------------------------------
        OutputDebugStringA("[Engine] Shutdown - Arquitectura Legacy.\n");

        if (m_sceneManager) m_sceneManager->Stop();
        if (m_updateManager) m_updateManager->Stop();
        if (m_renderManager) m_renderManager->Stop();
    }
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

//====================================================================================
// NUEVA IMPLEMENTACIÓN THREADING AAA
//====================================================================================

//--------------------------------------------------------------------------------------
// NUEVO: MainLoop consolidado - Game Logic en Main Thread (Estándar AAA)
//--------------------------------------------------------------------------------------
void Engine::MainLoop() {
    OutputDebugStringA("[Engine] MainLoop AAA iniciado.\n");

    while (m_context->isRunning) {
        auto frameStart = std::chrono::high_resolution_clock::now();

        // 1. INPUT (Main Thread)
        HandleInput();

        // 2. GAME LOGIC UPDATE (Main Thread)
        UpdateGameLogic(m_context->deltaTime);

        // 3. SCENE UPDATE (Main Thread)  
        //if (m_useNewThreading && m_sceneSystem) {
        //    m_sceneSystem->Update(m_context->deltaTime);  // Se implementará
        //}
        //else if (m_sceneManager) {
        //    // Legacy: usar SceneManager existente sin thread
        //    m_sceneManager->Update(m_context->deltaTime);
        //}

        // 4. SUBMIT RENDER COMMANDS
        SubmitRenderCommands();

        // 5. WAIT FOR VSYNC/FRAME LIMIT
        WaitForFrameLimit(frameStart);

        m_frameCounter++;
    }

    OutputDebugStringA("[Engine] MainLoop AAA terminado.\n");
}

//--------------------------------------------------------------------------------------
// NUEVO: Inicializar sistemas AAA (sin threads propios)
//--------------------------------------------------------------------------------------
bool Engine::InitAAASystems() {
    // TODO: Implementar cuando creemos los sistemas
    // m_updateSystem = std::make_unique<UpdateSystem>();
    // m_sceneSystem = std::make_unique<SceneSystem>();
    // m_renderCommandSystem = std::make_unique<RenderCommandSystem>();

    OutputDebugStringA("[Engine] Sistemas AAA inicializados (placeholder).\n");
    return true;
}

//--------------------------------------------------------------------------------------
// NUEVO: RenderThread puro - Solo renderizado (Estándar AAA)
//--------------------------------------------------------------------------------------
void Engine::RenderLoop() {
    // Inicialización del render thread
    OutputDebugStringA("[Engine] RenderLoop AAA iniciado.\n");

    // Configurar prioridad del thread
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    while (m_context->isRunning) {
        // 1. WAIT FOR RENDER COMMANDS
        WaitForRenderCommands();

        // 2. EXECUTE RENDER COMMANDS
        ExecuteRenderCommands();

        // 3. PRESENT
        if (m_renderManager) {
            // Usar RenderManager existente para present
            // m_renderManager->Present();  // Se implementará
        }

        // 4. SIGNAL FRAME COMPLETE
        SignalFrameComplete();
    }

    OutputDebugStringA("[Engine] RenderLoop AAA terminado.\n");
}

//--------------------------------------------------------------------------------------
// NUEVO: Update Game Logic en Main Thread
//--------------------------------------------------------------------------------------
void Engine::UpdateGameLogic(float deltaTime) {
    //if (m_useNewThreading && m_updateSystem) {
    //    // TODO: m_updateSystem->Update(deltaTime);
    //}
    //else {
    //    // Legacy: usar managers existentes
    //    // Consolidar lo que antes hacían UpdateManager/SceneManager threads
    //    // TODO: Implementar consolidación
    //}
}

//--------------------------------------------------------------------------------------
// NUEVO: Handle Input en Main Thread
//--------------------------------------------------------------------------------------
void Engine::HandleInput() {
    // Input ya se maneja en MainWindow, aquí podemos agregar lógica adicional
    // TODO: Consolidar input handling si es necesario
}

//--------------------------------------------------------------------------------------
// NUEVO: Submit Commands del Main Thread al Render Thread
//--------------------------------------------------------------------------------------
void Engine::SubmitRenderCommands() {
    std::lock_guard<std::mutex> lock(m_renderQueueMutex);

    // TODO: Recopilar comandos de todos los sistemas
    // auto sceneCommands = m_sceneSystem->GetRenderCommands();
    // auto terrainCommands = GetTerrainRenderCommands();

    // Crear packet de frame
    /*RenderCommandPacket framePacket;
    framePacket.frameId = m_frameCounter;*/
    // framePacket.commands.insert(framePacket.commands.end(), 
    //                            sceneCommands.begin(), sceneCommands.end());

    /*m_renderQueue.push(std::move(framePacket));*/
    m_renderCondition.notify_one();
}

//--------------------------------------------------------------------------------------
// NUEVO: Wait for Frame Limit (60 FPS target)
//--------------------------------------------------------------------------------------
void Engine::WaitForFrameLimit(std::chrono::high_resolution_clock::time_point frameStart) {
    const auto targetFrameTime = std::chrono::microseconds(16667); // 60 FPS

    auto frameEnd = std::chrono::high_resolution_clock::now();
    auto frameTime = frameEnd - frameStart;

    if (frameTime < targetFrameTime) {
        std::this_thread::sleep_for(targetFrameTime - frameTime);
    }
}

//--------------------------------------------------------------------------------------
// NUEVO: Wait for Render Commands en Render Thread
//--------------------------------------------------------------------------------------
void Engine::WaitForRenderCommands() {
    /*std::unique_lock<std::mutex> lock(m_renderQueueMutex);
    m_renderCondition.wait(lock, [this] {
        return !m_renderQueue.empty() || !m_context->isRunning;
        });*/
}

//--------------------------------------------------------------------------------------
// NUEVO: Execute Render Commands en Render Thread
//--------------------------------------------------------------------------------------
void Engine::ExecuteRenderCommands() {
    //std::unique_lock<std::mutex> lock(m_renderQueueMutex);

    //while (!m_renderQueue.empty()) {
    //    RenderCommandPacket packet = std::move(m_renderQueue.front());
    //    m_renderQueue.pop();
    //    lock.unlock();

    //    // TODO: Ejecutar comandos
    //    // for (auto& command : packet.commands) {
    //    //     command->Execute(deviceContext);
    //    // }

    //    lock.lock();
    //}
}

//--------------------------------------------------------------------------------------
// NUEVO: Signal Frame Complete
//--------------------------------------------------------------------------------------
void Engine::SignalFrameComplete() {
    m_frameReady = true;
    // TODO: Notificar al main thread si es necesario
}
