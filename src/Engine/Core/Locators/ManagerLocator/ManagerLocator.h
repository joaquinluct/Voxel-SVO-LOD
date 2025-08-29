// ManagerLocator.h
#pragma once

#include <string>
#include <memory>
#include <functional>
#include <map>
#include "yaml-cpp/yaml.h"
#include <windows.h> // Para HWND, HRESULT
#include <RenderManager/RenderManager.h>
#include <AssetManager.h>
#include <CameraManager.h>
#include <DeviceManager.h>
#include <KeyboardManager.h>
#include <ShaderManager.h>
#include <ManagerBase.h>

// #include <typeindex> // No es estrictamente necesario si usamos nombres de string para s_managers

// Forward declarations para asegurar que ManagerBase, IInitializable, IWindowDependentInitializable
// estén declaradas antes de ser usadas en std::shared_ptr en las lambdas.
// Si estas interfaces están definidas en sus propios archivos .h y esos archivos
// ya se incluyen antes de ManagerLocator.h en algunos .cpp, podrías no necesitar
// estas forward declarations aquí, pero es una buena práctica para prevenir
// dependencias circulares o problemas de orden de inclusión.
//class ManagerBase;
class IInitializable;
class IWindowDependentInitializable;

// Define el tipo para la lambda de creación.
using CreateManagerLambda = std::function<std::shared_ptr<ManagerBase>()>;
using InitializeManagerLambda = std::function<HRESULT(std::shared_ptr<ManagerBase>)>;
using InitializeWithParamasManagerLambda = std::function<HRESULT(std::shared_ptr<ManagerBase>, HWND*, int, int)>;
using RenderManagerLambda = std::function<void(std::shared_ptr<ManagerBase>)>;
using UpdateManagerLambda = std::function<void(std::shared_ptr<ManagerBase>, float)>;

class ManagerLocator {
public:
    // Constructor privado para Singleton
    ManagerLocator() = default;
    ManagerLocator(const ManagerLocator&) = delete;
    ManagerLocator& operator=(const ManagerLocator&) = delete;

    static ManagerLocator& GetInstance();

    // Registra las lambdas de creación e inicialización para un servicio.
    // El 'name' debe coincidir con la clave en tu archivo YAML para este manager.
    static void RegisterManagerCreator(
        const std::string& name,
        CreateManagerLambda createFn,
        InitializeManagerLambda initFn,
		InitializeWithParamasManagerLambda initWithParamsFn,
		RenderManagerLambda renderFn,
		UpdateManagerLambda updateFn
    );
    
    static HRESULT InitializeManagers(const std::vector<std::string>& orderList, HWND* hwnd, int width, int height);
    static HRESULT RenderManagers(const std::vector<std::string>& orderList);
    static HRESULT UpdateManagers(const std::vector<std::string>& orderList, float delta);
    //static HRESULT UpdateManagers(const std::vector<std::string>& orderList, float deltaTime);
    //static void ShutdownManagers(const std::vector<std::string>& orderList); // Shutdown suele ser void

    // Función para obtener un servicio ya inicializado por su tipo C++.
    // Ahora usa dynamic_pointer_cast internamente, ya que s_managers almacena ManagerBase.
    template<typename T>
    static std::shared_ptr<T> GetManager() {
        const std::string name1 = typeid(T).name();
		const std::string name = T::GetStaticManagerName();
		auto& s_managerEntries = GetManagerEntries();
        auto it = s_managerEntries.find(name);
        if (it != s_managerEntries.end()) {
            return std::dynamic_pointer_cast<T>(it->second.instance);
        }
        return nullptr;
    }
    static std::shared_ptr<ManagerBase> GetManager(const std::string& name);

    // Función para registrar directamente un servicio ya creado (para casos especiales o tests).
    // También usa T::GetStaticManagerName() como clave para mantener la consistencia.
    // Ahora acepta std::shared_ptr<ManagerBase> o un tipo que herede de ManagerBase.
    template<typename T>
    // T debe ser un ManagerBase o heredar de ManagerBase
    static void RegisterManager(std::shared_ptr<T> manager) {
        // Asume que la clase T tiene un método estático 'GetStaticManagerName()'.
        // La conversión implícita de std::shared_ptr<T> a std::shared_ptr<ManagerBase> es segura.
        static_assert(std::is_base_of<ManagerBase, T>::value, "T must derive from ManagerBase for ManagerLocator::RegisterManager");
        auto& s_managerEntries = GetManagerEntries();
        s_managerEntries[manager->GetStaticManagerName()] = manager;
        //s_managers[T::GetStaticManagerName()] = manager; 
    }

    // Función para limpiar todos los servicios registrados y sus creadores.
    // Este método es fundamental para el apagado correcto y la liberación de recursos.
    static void Shutdown();


	// Funciones helper para obtener el manager directamente.
    static const std::shared_ptr<DeviceManager> GetDeviceManager() {
		return GetManager<DeviceManager>();
    };
    /*static const std::shared_ptr<RenderTargetManager> GetRenderManager() {
        return GetManager<RenderTargetManager>();
	};*/
    /*static const std::shared_ptr<AssetManager> GetAssetManager() {
        return GetManager<AssetManager>();
	};*/
    static const std::shared_ptr<CameraManager> GetCameraManager() {
		return GetManager<CameraManager>();
	};    
    static const std::shared_ptr<KeyboardManager> GetKeyboardManager() {
		return GetManager<KeyboardManager>();
	};    
    static const std::shared_ptr<ShaderManager> GetShaderManager() {
        return GetManager<ShaderManager>();
    };


    struct ManagerEntry {
        std::shared_ptr<ManagerBase> instance; // La instancia real del manager
        CreateManagerLambda creator;
        InitializeManagerLambda initializer;
        InitializeWithParamasManagerLambda initializerWithParams;
        RenderManagerLambda renderer; // La lambda de render que definiste en la macro
		UpdateManagerLambda updater; // La lambda de actualización que definiste en la macro
        // ... otras lambdas (Update, Shutdown)
    };
private:
    static std::map<std::string, ManagerEntry>& GetManagerEntries();
};