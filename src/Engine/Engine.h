#pragma once

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
    void OnInput(UINT message, WPARAM wParam, LPARAM lParam);
    void OnResize(int width, int height);

    // --------------------------------------------------------------------------------------
    // Control del estado del motor
    // --------------------------------------------------------------------------------------
    bool IsRunning() const { return m_context->isRunning; }
    void SetRunning(bool running) { m_context->isRunning = running; }

    void SetDeltaTime(float deltaTime) { m_context->deltaTime = deltaTime; }

private:
    //--------------------------------------------------------------------------------------
    // Inicialización
    //--------------------------------------------------------------------------------------
    bool InitEngine(EngineContext* context);
    bool InitManagers();
    bool InitThreads();

    //--------------------------------------------------------------------------------------
    // Hilos de la aplicación.
    // Los hilos se controlan a través de unique_ptr para una correcta gestión de la memoria.
    //--------------------------------------------------------------------------------------
    std::unique_ptr<std::thread> m_updateThread;
    std::unique_ptr<std::thread> m_renderThread;
    std::shared_ptr<ThreadPool> m_threadPool;

    //--------------------------------------------------------------------------------------
    // Cola para la comunicación entre los hilos de actualización y de renderizado.
    //--------------------------------------------------------------------------------------
    std::queue<RenderTask> m_renderQueue;
    std::mutex m_renderQueueMutex;

    //--------------------------------------------------------------------------------------
    // Puntero para la ventana, necesario para la inicialización de DirectX.
    //--------------------------------------------------------------------------------------
    HWND* m_hWnd = nullptr;

    // --------------------------------------------------------------------------------------
    // Control del estado
    // --------------------------------------------------------------------------------------
    EngineContext* m_context = nullptr; // Contexto del motor

    //--------------------------------------------------------------------------------------
    // Managers principales del motor.
    //--------------------------------------------------------------------------------------
    std::shared_ptr<InputManager> m_inputManager;
    std::shared_ptr<DeviceManager> m_deviceManager;
    std::shared_ptr<InitManager> m_initManager;
    std::shared_ptr<RenderManager> m_renderManager;
    std::shared_ptr<UpdateManager> m_updateManager;
    std::shared_ptr<SceneManager> m_sceneManager;

    //--------------------------------------------------------------------------------------
    // Métodos para el control de los hilos.
    //--------------------------------------------------------------------------------------
    /*void UpdateLoop();
    void RenderLoop();*/
    void Pause();
    void Resume();
    void ShutdownThreads();
};
