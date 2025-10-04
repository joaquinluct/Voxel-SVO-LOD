#pragma once

#include "Core/Foundation/Types.h"

#ifdef _WIN32
    #include <windows.h>
#endif

namespace Engine {
    /**
     * @brief Contexto global del motor que contiene información compartida
     * 
     * Este contexto se pasa entre diferentes sistemas del motor y contiene
     * información de estado, configuración y recursos compartidos.
     */
    struct EngineContext {
        // Timing information
        f32 deltaTime = 0.0f;
        uint64 frameCount = 0;
        f64 totalTime = 0.0;
        
        // Engine state
        bool isInitialized = false;
        bool isRunning = true;
        bool isPaused = false;
        
        // Platform specific (Windows)
        #ifdef _WIN32
            HWND windowHandle = nullptr;
            HINSTANCE instanceHandle = nullptr;
        #else
            void* windowHandle = nullptr;
            void* instanceHandle = nullptr;
        #endif
        
        // Window properties
        uint32 windowWidth = 1920;
        uint32 windowHeight = 1080;
        bool isFullscreen = false;
        bool isMinimized = false;
        
        // Graphics context (se poblará en DX12)
        void* graphicsDevice = nullptr;
        void* commandQueue = nullptr;
        void* swapChain = nullptr;
        
        // Input state
        bool inputCaptured = false;
        bool mouseVisible = true;
        
        // Debug information
        bool debugMode = false;
        uint32 debugFlags = 0;
        
        // Performance metrics
        f32 averageFPS = 0.0f;
        f32 frameTime = 0.0f;
        
        // Constructor
        EngineContext() = default;
        
        // Utility methods
        f32 GetAspectRatio() const noexcept {
            return windowHeight > 0 ? static_cast<f32>(windowWidth) / static_cast<f32>(windowHeight) : 1.0f;
        }
        
        void UpdateFrameStats(f32 currentDeltaTime) noexcept {
            frameCount++;
            frameTime = currentDeltaTime;
            totalTime += static_cast<f64>(currentDeltaTime);
            
            // Calculate running average FPS
            if (currentDeltaTime > 0.0f) {
                const f32 currentFPS = 1.0f / currentDeltaTime;
                averageFPS = (averageFPS * 0.95f) + (currentFPS * 0.05f); // Exponential moving average
            }
        }
    };
    
    /**
     * @brief Contexto específico para el sistema gráfico DirectX 12
     * 
     * Contiene punteros a objetos específicos de DX12 con forward declarations
     * para evitar dependencias circulares.
     */
    struct GraphicsContext {
        // Forward declarations para evitar includes de DX12
        struct Device;
        struct CommandQueue;
        struct SwapChain;
        struct CommandList;
        struct DescriptorHeap;
        
        // DX12 Core objects
        SharedPtr<Device> device;
        SharedPtr<SwapChain> swapChain;
        
        // Command queues
        SharedPtr<CommandQueue> directQueue;
        SharedPtr<CommandQueue> computeQueue;
        SharedPtr<CommandQueue> copyQueue;
        
        // Descriptor heaps
        SharedPtr<DescriptorHeap> rtvHeap;
        SharedPtr<DescriptorHeap> dsvHeap;
        SharedPtr<DescriptorHeap> srvHeap;
        SharedPtr<DescriptorHeap> samplerHeap;
        
        // Frame resources
        uint32 frameIndex = 0;
        uint32 backBufferCount = 2;
        
        // Synchronization
        uint64 fenceValues[3] = {0}; // Para frames en vuelo
        void* fenceEvent = nullptr;
        
        // Constructor
        GraphicsContext() = default;
    };
}
