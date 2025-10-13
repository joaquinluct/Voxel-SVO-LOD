#pragma once

#include <chrono>
#include <Defines/EngineDefinition.h>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <windows.h>

class DeviceManager;
class RenderManager;
class UpdateManager;
class InputManager;
class InitManager;
class ThreadPool;
class SceneManager;

// Forward declarations para nuevos sistemas AAA
//class UpdateSystem;
//class SceneSystem;
//class RenderCommandSystem;

//--------------------------------------------------------------------------------------
// Estructura para contener las tareas de renderizado.
//--------------------------------------------------------------------------------------
struct RenderTask {
    // Los datos de la tarea de renderizado.
    // Ej: ID del objeto a renderizar, matriz de transformación, etc.
};


//--------------------------------------------------------------------------------------
// Clase principal del motor. Contiene los hilos y la lógica principal de la aplicación.
//--------------------------------------------------------------------------------------
class Engine {
public:
    //--------------------------------------------------------------------------------------
    // Constructor
    //--------------------------------------------------------------------------------------
    Engine();

    //--------------------------------------------------------------------------------------
    // Destructor
    //--------------------------------------------------------------------------------------
    ~Engine();

    //--------------------------------------------------------------------------------------
    // Función para cerrar el motor y liberar los recursos.
    //--------------------------------------------------------------------------------------
    void Shutdown();

    //--------------------------------------------------------------------------------------
    // Inicializa los recursos del motor, incluido DirectX.
    // La ventana se crea y se pasa desde MainWindow.
    //--------------------------------------------------------------------------------------
    bool Init(EngineContext* context);

    //--------------------------------------------------------------------------------------
    // Métodos llamados por MainWindow para la entrada y el redimensionamiento.
    //--------------------------------------------------------------------------------------
    void OnInput(UINT message, WPARAM wParam, LPARAM lParam, float deltaTime);
    void OnResize(int width, int height);

    // --------------------------------------------------------------------------------------
    // Control del estado del motor
    // --------------------------------------------------------------------------------------
    bool IsRunning() const { return m_context->isRunning; }
    void SetRunning(bool running) { m_context->isRunning = running; }

    void SetDeltaTime(float deltaTime) { m_context->deltaTime = deltaTime; }

    // THREADING AAA: Comentado temporalmente para mantener compilación
    void MainLoop();

private:
    //--------------------------------------------------------------------------------------
    // Inicialización
    //--------------------------------------------------------------------------------------
    bool InitEngine(EngineContext* context);
    bool InitManagers();
    bool InitThreads();
    bool InitAAASystems();  // NUEVO: Inicializar sistemas AAA

    //--------------------------------------------------------------------------------------
    // NUEVA ARQUITECTURA THREADING AAA: Solo Render Thread + ThreadPool
    //--------------------------------------------------------------------------------------
    std::unique_ptr<std::thread> m_renderThread;      // Solo render thread
    std::shared_ptr<ThreadPool> m_threadPool;         // Jobs asíncronos

    // Sincronización Update ? Render (Producer-Consumer)
    //std::queue<RenderCommandPacket> m_renderQueue;
    std::mutex m_renderQueueMutex;
    std::condition_variable m_renderCondition;

    // Frame synchronization
    std::atomic<bool> m_frameReady{ false };
    std::atomic<int> m_frameCounter{ 0 };

    //--------------------------------------------------------------------------------------
    // NUEVO: Threading AAA Methods
    //--------------------------------------------------------------------------------------
    void RenderLoop();                                 // Solo renderizado en thread separado
    void UpdateGameLogic(float deltaTime);             // Game logic en main thread
    void HandleInput();                                // Input en main thread
    void SubmitRenderCommands();                       // Enviar comandos a render thread
    void WaitForFrameLimit(std::chrono::high_resolution_clock::time_point frameStart);
    void WaitForRenderCommands();
    void ExecuteRenderCommands();
    void SignalFrameComplete();

    //--------------------------------------------------------------------------------------
    // Puntero para la ventana, necesario para la inicialización de DirectX.
    //--------------------------------------------------------------------------------------
    HWND* m_hWnd = nullptr;

    // --------------------------------------------------------------------------------------
    // Control del estado
    // --------------------------------------------------------------------------------------
    EngineContext* m_context = nullptr; // Contexto del motor

    //--------------------------------------------------------------------------------------
    // NUEVO: Sistemas AAA (reemplazan managers threading)
    //--------------------------------------------------------------------------------------
    //std::unique_ptr<UpdateSystem> m_updateSystem;     // Sin thread propio
    //std::unique_ptr<SceneSystem> m_sceneSystem;       // Sin thread propio
    //std::unique_ptr<RenderCommandSystem> m_renderCommandSystem;

    //--------------------------------------------------------------------------------------
    // Managers principales del motor (Legacy + Required)
    //--------------------------------------------------------------------------------------
    std::shared_ptr<InputManager> m_inputManager;
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<InitManager> m_initManager;
    std::shared_ptr<RenderManager> m_renderManager;   // Mantener para compatibilidad

    // LEGACY: Mantener para migración gradual
    std::shared_ptr<UpdateManager> m_updateManager;   // Temporal durante migración
    std::shared_ptr<SceneManager> m_sceneManager;     // Temporal durante migración

    //--------------------------------------------------------------------------------------
    // Control threading flags
    //--------------------------------------------------------------------------------------
    bool m_useNewThreading = true;  // Flag para cambio gradual

    //--------------------------------------------------------------------------------------
    // Métodos para el control de los hilos.
    //--------------------------------------------------------------------------------------
    void Pause();
    void Resume();
    void ShutdownThreads();
};
