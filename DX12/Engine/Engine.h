#pragma once

// Platform specific
#ifdef _WIN32
    #include <windows.h>
#endif

// Engine includes (alfabético por categoría)
#include "Core/Context/EngineContext.h"
#include "Core/Foundation/Macros.h"
#include "Core/Foundation/Types.h"

namespace Engine {
    /**
     * @brief Motor principal DirectX 12
     * 
     * Esta es la clase principal que coordina todos los sistemas del motor:
     * - Graphics (DX12)
     * - Managers (Scene, Render, Input, etc.)
     * - Services (Threading, Audio, etc.)
     * - World/ECS
     */
    class Engine {
    public:
        Engine();
        ~Engine();
        
        // Non-copyable
        ENGINE_NON_COPYABLE(Engine)
        ENGINE_NON_MOVABLE(Engine)
        
        /**
         * @brief Inicializa el motor con el contexto proporcionado
         * @param context Contexto del motor con configuración inicial
         * @return true si la inicialización fue exitosa
         */
        bool Init(EngineContext* context);
        
        /**
         * @brief Actualiza el delta time del motor
         * @param deltaTime Tiempo transcurrido desde el último frame
         */
        void SetDeltaTime(f32 deltaTime);
        
        /**
         * @brief Maneja eventos de entrada de Windows
         * @param message Tipo de mensaje de Windows
         * @param wParam Parámetro W del mensaje
         * @param lParam Parámetro L del mensaje
         * @param deltaTime Delta time actual
         */
        void OnInput(uint32 message, uintptr_t wParam, intptr_t lParam, f32 deltaTime);
        
        /**
         * @brief Maneja el redimensionamiento de la ventana
         * @param width Nueva anchura
         * @param height Nueva altura
         */
        void OnResize(int width, int height);
        
        /**
         * @brief Cierra el motor de forma ordenada
         */
        void Shutdown();
        
        // State queries
        bool IsInitialized() const noexcept { return m_isInitialized; }
        bool IsRunning() const noexcept { return m_isRunning; }
        
    private:
        // Core state
        EngineContext* m_context = nullptr;
        bool m_isInitialized = false;
        bool m_isRunning = false;
        
        // Timing
        f32 m_deltaTime = 0.0f;
        
        // TODO: Agregar managers cuando estén implementados
        // UniquePtr<DeviceManager> m_deviceManager;
        // UniquePtr<RenderManager> m_renderManager;
        // UniquePtr<SceneManager> m_sceneManager;
        // etc.
        
        // Initialization phases
        bool InitializeCore();
        bool InitializeManagers();
        bool InitializeGraphics();
        bool InitializeWorld();
        
        // Shutdown phases
        void ShutdownWorld();
        void ShutdownGraphics();
        void ShutdownManagers();
        void ShutdownCore();
    };
}
