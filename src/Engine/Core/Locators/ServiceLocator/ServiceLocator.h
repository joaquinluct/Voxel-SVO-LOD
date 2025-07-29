// ServiceLocator.h
#pragma once

#include <string>
#include <memory>
#include <functional>
#include <map>
#include <windows.h> // Para HWND, HRESULT
#include <IService.h>
#include <ConfigService.h>
#include <ConfigLocator/ConfigLocator.h>

// #include <typeindex> // No es estrictamente necesario si usamos nombres de string para s_services

// Forward declarations para asegurar que IService, IInitializable, IWindowDependentInitializable
// estén declaradas antes de ser usadas en std::shared_ptr en las lambdas.
// Si estas interfaces están definidas en sus propios archivos .h y esos archivos
// ya se incluyen antes de ServiceLocator.h en algunos .cpp, podrías no necesitar
// estas forward declarations aquí, pero es una buena práctica para prevenir
// dependencias circulares o problemas de orden de inclusión.
class IService;
class IInitializable;
class IWindowDependentInitializable;

// Define el tipo para la lambda de creación.
// Ahora retorna std::shared_ptr<IService>, ya que IService es nuestra base polimórfica común.
using CreateServiceLambda = std::function<std::shared_ptr<IService>()>;

// Define el tipo para la lambda de inicialización universal.
// Ahora recibe std::shared_ptr<IService> en lugar de std::shared_ptr<void>.
// ESTA ES LA FIRMA QUE DEBE COINCIDIR EXACTAMENTE CON LA MACRO REGISTER_MANAGER_TYPE.
using InitializeServiceLambda = std::function<HRESULT(std::shared_ptr<IService>)>;
using RenderServiceLambda = std::function<void(std::shared_ptr<IService>)>;
using UpdateServiceLambda = std::function<void(std::shared_ptr<IService>, float)>;

class ServiceLocator {
public:
    // Constructor privado para Singleton
    ServiceLocator() = default;
    ServiceLocator(const ServiceLocator&) = delete;
    ServiceLocator& operator=(const ServiceLocator&) = delete;

    static ServiceLocator& GetInstance();

    // Registra las lambdas de creación e inicialización para un servicio.
    // El 'name' debe coincidir con la clave en tu archivo YAML para este manager.
    static void RegisterServiceCreator(
        const std::string& name,
        CreateServiceLambda createFn,
        InitializeServiceLambda initFn,
		RenderServiceLambda renderFn,
		UpdateServiceLambda updateFn
    );

    
    static HRESULT InitializeServices(const std::vector<std::string>& orderList);
    static HRESULT RenderServices(const std::vector<std::string>& orderList);
    static HRESULT RenderShadowPassServices(const std::vector<std::string>& orderList);
    static HRESULT UpdateServices(const std::vector<std::string>& orderList, float DeltaTime);
    //static void ShutdownServices(const std::vector<std::string>& orderList); // Shutdown suele ser void

    // Función para obtener un servicio ya inicializado por su tipo C++.
    // Ahora usa dynamic_pointer_cast internamente, ya que s_services almacena IService.
    template<typename T>
    static std::shared_ptr<T> GetService() {
        //const std::string name1 = typeid(T).name();
		const std::string name = T::GetStaticServiceName();
		auto& s_serviceEntries = GetServiceEntries();
        auto it = s_serviceEntries.find(name);
        if (it != s_serviceEntries.end()) {
            std::shared_ptr<T> service = std::dynamic_pointer_cast<T>(it->second.instance);
            return service;
        }
        return nullptr;
    }
    static std::shared_ptr<IService> GetService(const std::string& name);

    // Función para registrar directamente un servicio ya creado (para casos especiales o tests).
    // También usa T::GetStaticServiceName() como clave para mantener la consistencia.
    // Ahora acepta std::shared_ptr<IService> o un tipo que herede de IService.
    template<typename T>
    // T debe ser un IService o heredar de IService
    static void RegisterService(std::shared_ptr<T> service) {
        // Asume que la clase T tiene un método estático 'GetStaticServiceName()'.
        // La conversión implícita de std::shared_ptr<T> a std::shared_ptr<IService> es segura.
        static_assert(std::is_base_of<IService, T>::value, "T must derive from IService for ServiceLocator::RegisterService");
        auto& s_serviceEntries = GetServiceEntries();
        s_serviceEntries[service->GetStaticServiceName()] = service;
        //s_services[T::GetStaticServiceName()] = service; 
    }

    // Función para limpiar todos los servicios registrados y sus creadores.
    // Este método es fundamental para el apagado correcto y la liberación de recursos.
    static void Shutdown();

    struct ServiceEntry {
        std::shared_ptr<IService> instance; // La instancia real del manager
        CreateServiceLambda creator;
        InitializeServiceLambda initializer;
        RenderServiceLambda renderer; // La lambda de render que definiste en la macro
		UpdateServiceLambda updater; // Si decides usar una lambda de actualización
        // ... otras lambdas (Update, Shutdown)
    };
private:
    static std::map<std::string, ServiceEntry>& GetServiceEntries();
};