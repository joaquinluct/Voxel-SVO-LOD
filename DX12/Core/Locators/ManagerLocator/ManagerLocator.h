// ManagerLocator.h
#pragma once

// Standard C++23 includes (alfabético)
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <typeindex>
#include <vector>

// Platform specific
#ifdef _WIN32
    #include <windows.h> // Para HRESULT
#endif

// Forward declarations para evitar dependencias circulares
// TODO: Estos managers serán implementados posteriormente
class ManagerBase;
class IInitializable;
class IEngineDependentInitializable;
struct EngineContext;

// Forward declarations de managers específicos (evita includes circulares)
// TODO: Activar cuando los managers estén implementados
// class DeviceManager;
// class CameraManager;
// class KeyboardManager;
// class ShaderManager;

// Define el tipo para las lambdas de gestión
using CreateManagerLambda = std::function<std::shared_ptr<ManagerBase>()>;
using InitializeManagerLambda = std::function<HRESULT(std::shared_ptr<ManagerBase>, EngineContext* context)>;
using RenderManagerLambda = std::function<void(std::shared_ptr<ManagerBase>)>;
using UpdateManagerLambda = std::function<void(std::shared_ptr<ManagerBase>, float)>;

class ManagerLocator {
public:
    // Singleton pattern
    ManagerLocator() = default;
    ManagerLocator(const ManagerLocator&) = delete;
    ManagerLocator& operator=(const ManagerLocator&) = delete;

    static ManagerLocator& GetInstance();

    // Registra las lambdas de creación e inicialización para un manager
    // El 'name' debe coincidir con la clave en tu archivo YAML para este manager
    static void RegisterManagerCreator(
        const std::string& name,
        CreateManagerLambda createFn,
        InitializeManagerLambda initFn,
        RenderManagerLambda renderFn,
        UpdateManagerLambda updateFn
    );

    static HRESULT InitializeManagers(const std::vector<std::string>& orderList, EngineContext* context);
    static HRESULT RenderManagers(const std::vector<std::string>& orderList);
    static HRESULT UpdateManagers(const std::vector<std::string>& orderList, float delta);

    // Función para obtener un manager ya inicializado por su tipo C++
    // Usa dynamic_pointer_cast internamente, ya que s_managers almacena ManagerBase
    template<typename T>
    static std::shared_ptr<T> GetManager() {
        const std::string name = T::GetStaticManagerName();
        if (name.empty()) {
            // Manejo de error: La clase T no tiene un método estático GetStaticManagerName()
            return nullptr;
        }
        auto& managerEntries = GetManagerEntries();
        auto it = managerEntries.find(name);
        if (it != managerEntries.end()) {
            return std::dynamic_pointer_cast<T>(it->second.instance);
        }
        return nullptr;
    }
    
    static std::shared_ptr<ManagerBase> GetManager(const std::string& name);

    // Función para registrar directamente un manager ya creado (para casos especiales o tests)
    // Acepta std::shared_ptr<ManagerBase> o un tipo que herede de ManagerBase
    template<typename T>
    static void RegisterManager(std::shared_ptr<T> manager) {
        static_assert(std::is_base_of<ManagerBase, T>::value, 
                     "T must derive from ManagerBase for ManagerLocator::RegisterManager");
        auto& managerEntries = GetManagerEntries();
        managerEntries[manager->GetStaticManagerName()] = {manager, nullptr, nullptr, nullptr, nullptr};
    }

    // Función para limpiar todos los managers registrados y sus creadores
    static void Shutdown();

    // Funciones helper para obtener managers específicos
    // TODO: Activar cuando los managers estén implementados
    /*
    static std::shared_ptr<DeviceManager> GetDeviceManager() {
        return GetManager<DeviceManager>();
    }
    
    static std::shared_ptr<CameraManager> GetCameraManager() {
        return GetManager<CameraManager>();
    }
    
    static std::shared_ptr<KeyboardManager> GetKeyboardManager() {
        return GetManager<KeyboardManager>();
    }
    
    static std::shared_ptr<ShaderManager> GetShaderManager() {
        return GetManager<ShaderManager>();
    }
    */

    struct ManagerEntry {
        std::shared_ptr<ManagerBase> instance;     // La instancia real del manager
        CreateManagerLambda creator;               // Lambda de creación
        InitializeManagerLambda initializer;       // Lambda de inicialización
        RenderManagerLambda renderer;              // Lambda de render
        UpdateManagerLambda updater;               // Lambda de actualización
    };

private:
    static std::map<std::string, ManagerEntry>& GetManagerEntries();
};
