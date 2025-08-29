#pragma once

#include <windows.h>
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <chrono>

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
    // Inicializa los recursos del motor, incluido DirectX.
    // La ventana se crea y se pasa desde MainWindow.
    //--------------------------------------------------------------------------------------
    bool Init(HINSTANCE hInstance, int nCmdShow, HWND* outHwnd, int width, int height);

    //--------------------------------------------------------------------------------------
    // Método principal llamado por MainWindow para actualizar y renderizar.
    //--------------------------------------------------------------------------------------
    void UpdateAndRender(float deltaTime);

    //--------------------------------------------------------------------------------------
    // Métodos llamados por MainWindow para la entrada y el redimensionamiento.
    //--------------------------------------------------------------------------------------
    void OnInput(UINT message, WPARAM wParam, LPARAM lParam);
    void OnResize(int width, int height);

private:
	//--------------------------------------------------------------------------------------
	// Inicialización
	//--------------------------------------------------------------------------------------
    bool InitEngine(HWND* outHwnd, int width, int height);
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
    std::condition_variable m_renderCondition;

    //--------------------------------------------------------------------------------------
    // Puntero para la ventana, necesario para la inicialización de DirectX.
    //--------------------------------------------------------------------------------------
    HWND* m_hWnd = nullptr;

	// --------------------------------------------------------------------------------------
	// Control del estado
	// --------------------------------------------------------------------------------------
	// Delta time 
    std::chrono::duration<float> m_deltaTime;
    // Tiempo transcurrido desde la última actualización.
    std::chrono::time_point < std::chrono::high_resolution_clock> m_lastTime;
    // Bandera atómica para notificar a los hilos que deben detenerse.
    std::atomic<bool> m_isShutdown;           

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
    // Métodos que ejecutan los bucles de los hilos.
    //--------------------------------------------------------------------------------------
    void UpdateLoop();
    void RenderLoop();
    void ShutdownThreads();
};