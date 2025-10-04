#pragma once

// C++14 Standard Library (alphabetic)
#include <memory>

// Platform specific (alphabetic)
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

// Engine includes (alphabetic by category)
#include "Core/Context/EngineContext.h"
#include "Core/Foundation/Macros.h"
#include "Core/Foundation/Types.h"
#include "Engine/Engine.h"

// Generated config includes - C++14 compatible
#include "../../generated/Configs/Window/MainWindowConfig.h"

namespace Engine {
    /**
     * @brief Clase principal que gestiona la ventana y el bucle de la aplicación en DX12
     * 
     * Responsabilidades:
     * - Creación y gestión de la ventana Win32
     * - Bucle principal de mensajes  
     * - Integración con el motor Engine DX12
     * - Gestión del contexto de la aplicación
     * - Configuración YAML completa (C++14)
     */
    class MainWindow {
    public:
        // Constructor moderno con configuración YAML
        explicit MainWindow(HINSTANCE hInstance, 
                           HINSTANCE hPrevInstance, 
                           LPWSTR lpCmdLine, 
                           int nCmdShow);
        
        // Destructor
        ~MainWindow();
        
        // C++14 Enhanced move semantics
        ENGINE_NON_COPYABLE(MainWindow)
        ENGINE_DEFAULT_MOVE(MainWindow)

        /**
         * @brief Crea la ventana y ejecuta el bucle principal
         * @return Expected con código de salida o error (C++14 compatible)
         */
        ENGINE_NODISCARD Expected<int> Create();
        
        /**
         * @brief Inicializa la ventana con configuración YAML
         * @return Expected con resultado o error (C++14 compatible)
         */
        ENGINE_NODISCARD Expected<bool> Initialize();

        // C++14 Getters with contracts
        ENGINE_NODISCARD HWND GetWindowHandle() const noexcept { 
            ENGINE_EXPECTS(m_windowHandle != nullptr);
            return m_windowHandle; 
        }
        
        ENGINE_NODISCARD const EngineContext& GetEngineContext() const noexcept { 
            ENGINE_EXPECTS(m_engineContext != nullptr);
            return *m_engineContext; 
        }
        
        ENGINE_NODISCARD f32 GetDeltaTime() const noexcept { return m_deltaTime; }
        ENGINE_NODISCARD bool IsRunning() const noexcept { return m_isRunning; }
        
        ENGINE_NODISCARD const MainWindowConfig& GetConfig() const noexcept { 
            ENGINE_EXPECTS(m_config != nullptr);
            return *m_config; 
        }

    private:
        // C++14 Core members
        UniquePtr<Engine> m_gameEngine;
        UniquePtr<EngineContext> m_engineContext;
        UniquePtr<MainWindowConfig> m_config;

        // Platform specific
        HINSTANCE m_hInstance = nullptr;
        HWND m_windowHandle = nullptr;
        int m_showCommand = 0;
        
        // Timing
        f32 m_deltaTime = 0.0f;
        LARGE_INTEGER m_frequency{};
        LARGE_INTEGER m_previousTime{};
        
        // State
        bool m_isRunning = false;
        bool m_isInitialized = false;

        // C++14 Window management with Expected
        ENGINE_NODISCARD Expected<bool> InitializeWindow();
        ENGINE_NODISCARD Expected<bool> CreateWindowClass();
        ENGINE_NODISCARD Expected<bool> CreateWindowInstance();
        void UpdateTiming();
        void ProcessMessages();
        
        // Message handling
        void HandleInput(UINT message, WPARAM wParam, LPARAM lParam);
        void OnResize(uint32 width, uint32 height);
        void OnDestroy();
        void OnKeyDown(WPARAM key);
        
        // Window procedure
        LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        
        // Static window procedure bridge
        static MainWindow* s_instance;
        static LRESULT CALLBACK StaticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
        
        // C++14 Configuration loading with Expected
        ENGINE_NODISCARD Expected<bool> LoadConfiguration();
        void ApplyWindowConfiguration();
        ENGINE_NODISCARD Expected<bool> LoadConfigurationFromYAML();
        
        // C++14 Constants with constexpr
        static constexpr f32 MAX_DELTA_TIME = 0.052f;
        static constexpr f32 MIN_DELTA_TIME = 0.042f;
        static constexpr const wchar_t* WINDOW_CLASS_NAME = L"DirectX12VoxelEngine";
        static constexpr const wchar_t* WINDOW_TITLE = L"DirectX 12 Voxel Engine";
    };
}
