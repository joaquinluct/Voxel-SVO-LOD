#include "Engine.h"

// Standard C++14 includes (alphabetical)
#include <sstream>

// Platform specific (alphabetical)
#ifdef _WIN32
    #include <windows.h>
#endif

// Engine includes (alphabetical by category)
#include "Core/Foundation/Macros.h"

namespace Engine {
    
    Engine::Engine() {
        ENGINE_LOG("Constructor - Creando instancia del motor");
    }
    
    Engine::~Engine() {
        if (m_isRunning) {
            Shutdown();
        }
        ENGINE_LOG("Destructor - Motor destruido");
    }
    
    bool Engine::Init(EngineContext* context) {
        ENGINE_ASSERT(context != nullptr);
        ENGINE_ASSERT(!m_isInitialized);
        
        ENGINE_LOG("Iniciando motor DirectX 12...");
        
        m_context = context;
        
        // Phase 1: Core systems
        if (!InitializeCore()) {
            ENGINE_LOG_ERROR("Fallo en inicializacion del core");
            return false;
        }
        
        // Phase 2: Managers
        if (!InitializeManagers()) {
            ENGINE_LOG_ERROR("Fallo en inicializacion de managers");
            return false;
        }
        
        // Phase 3: Graphics (DX12)
        if (!InitializeGraphics()) {
            ENGINE_LOG_ERROR("Fallo en inicializacion de graficos");
            return false;
        }
        
        // Phase 4: World/Scene
        if (!InitializeWorld()) {
            ENGINE_LOG_ERROR("Fallo en inicializacion del mundo");
            return false;
        }
        
        m_isInitialized = true;
        m_isRunning = true;
        m_context->isInitialized = true;
        
        ENGINE_LOG("Motor DirectX 12 inicializado correctamente");
        return true;
    }
    
    void Engine::SetDeltaTime(f32 deltaTime) {
        m_deltaTime = deltaTime;
        
        if (m_context) {
            m_context->deltaTime = deltaTime;
            m_context->UpdateFrameStats(deltaTime);
        }
    }
    
    void Engine::OnInput(uint32 message, uintptr_t wParam, intptr_t lParam, f32 deltaTime) {
        if (!m_isInitialized) {
            return;
        }
        
        // TODO: Delegar a InputManager cuando este implementado
        // m_inputManager->HandleInput(message, wParam, lParam, deltaTime);
        
        std::ostringstream oss;
        oss << "Input recibido: msg=" << message << ", wParam=" << wParam << ", lParam=" << lParam;
        ENGINE_LOG(oss.str());
    }
    
    void Engine::OnResize(int width, int height) {
        if (!m_isInitialized || !m_context) {
            return;
        }
        
        std::ostringstream oss;
        oss << "Redimensionamiento: " << width << "x" << height;
        ENGINE_LOG(oss.str());
        
        // Actualizar contexto
        m_context->windowWidth = static_cast<uint32>(width);
        m_context->windowHeight = static_cast<uint32>(height);
        
        // TODO: Notificar a DeviceManager/RenderManager cuando esten implementados
        // m_deviceManager->OnResize(width, height);
        // m_renderManager->OnResize(width, height);
    }
    
    void Engine::Shutdown() {
        if (!m_isRunning) {
            return;
        }
        
        ENGINE_LOG("Iniciando shutdown del motor...");
        
        m_isRunning = false;
        if (m_context) {
            m_context->isRunning = false;
        }
        
        // Shutdown en orden inverso a la inicializacion
        ShutdownWorld();
        ShutdownGraphics();
        ShutdownManagers();
        ShutdownCore();
        
        m_isInitialized = false;
        
        ENGINE_LOG("Shutdown del motor completado");
    }
    
    // Private initialization methods
    bool Engine::InitializeCore() {
        ENGINE_LOG("Inicializando sistemas core...");
        
        // TODO: Inicializar ConfigLocator, ServiceLocator, etc.
        // ConfigLocator::Initialize();
        // ServiceLocator::Initialize();
        
        ENGINE_LOG("Sistemas core inicializados");
        return true;
    }
    
    bool Engine::InitializeManagers() {
        ENGINE_LOG("Inicializando managers...");
        
        // TODO: Crear e inicializar managers usando ManagerLocator
        // m_deviceManager = ManagerLocator::CreateManager<DeviceManager>();
        // m_renderManager = ManagerLocator::CreateManager<RenderManager>();
        // etc.
        
        ENGINE_LOG("Managers inicializados");
        return true;
    }
    
    bool Engine::InitializeGraphics() {
        ENGINE_LOG("Inicializando sistema grafico DirectX 12...");
        
        // TODO: Inicializar DX12 Device, SwapChain, Command Queues, etc.
        // Crear GraphicsContext y poblar m_context->graphicsDevice, etc.
        
        ENGINE_LOG("Sistema grafico DirectX 12 inicializado");
        return true;
    }
    
    bool Engine::InitializeWorld() {
        ENGINE_LOG("Inicializando mundo/escena...");
        
        // TODO: Inicializar ECS, SceneManager, etc.
        
        ENGINE_LOG("Mundo/escena inicializado");
        return true;
    }
    
    // Private shutdown methods
    void Engine::ShutdownWorld() {
        ENGINE_LOG("Cerrando mundo/escena...");
        // TODO: Cleanup world/scene
    }
    
    void Engine::ShutdownGraphics() {
        ENGINE_LOG("Cerrando sistema grafico...");
        // TODO: Cleanup DX12 resources
    }
    
    void Engine::ShutdownManagers() {
        ENGINE_LOG("Cerrando managers...");
        // TODO: Shutdown managers in reverse order
    }
    
    void Engine::ShutdownCore() {
        ENGINE_LOG("Cerrando sistemas core...");
        // TODO: Cleanup core systems
    }
    
} // namespace Engine
