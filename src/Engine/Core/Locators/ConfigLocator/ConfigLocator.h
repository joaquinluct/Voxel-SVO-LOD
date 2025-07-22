// ConfigLocator.h
#pragma once

#include <string>
#include <memory>
#include <functional>
#include <map>
#include <windows.h> // Para HWND, HRESULT
#include <IConfig.h>
// #include <typeindex> // No es estrictamente necesario si usamos nombres de string para s_services

// Forward declarations para asegurar que IConfig, IInitializable, IWindowDependentInitializable
// estén declaradas antes de ser usadas en std::shared_ptr en las lambdas.
// Si estas interfaces están definidas en sus propios archivos .h y esos archivos
// ya se incluyen antes de ConfigLocator.h en algunos .cpp, podrías no necesitar
// estas forward declarations aquí, pero es una buena práctica para prevenir
// dependencias circulares o problemas de orden de inclusión.
class IInitializable;
class IWindowDependentInitializable;

// Define el tipo para la lambda de creación.
// Ahora retorna std::shared_ptr<IConfig>, ya que IConfig es nuestra base polimórfica común.
using CreateConfigLambda = std::function<std::shared_ptr<IConfig>()>;

// Define el tipo para la lambda de inicialización universal.
// Ahora recibe std::shared_ptr<IConfig> en lugar de std::shared_ptr<void>.
// ESTA ES LA FIRMA QUE DEBE COINCIDIR EXACTAMENTE CON LA MACRO REGISTER_MANAGER_TYPE.
using InitializeConfigLambda = std::function<HRESULT(std::shared_ptr<IConfig>)>;

class ConfigLocator {
public:
    // Constructor privado para Singleton
    ConfigLocator() = default;
    ConfigLocator(const ConfigLocator&) = delete;
    ConfigLocator& operator=(const ConfigLocator&) = delete;

    static ConfigLocator& GetInstance();

    // Registra las lambdas de creación e inicialización para un servicio.
    // El 'name' debe coincidir con la clave en tu archivo YAML para este manager.
    static void RegisterConfigCreator(
        const std::string& name,
        CreateConfigLambda createFn
    );

    
    static HRESULT CreateConfigs();

    // Función para obtener un servicio ya inicializado por su tipo C++.
    // Ahora usa dynamic_pointer_cast internamente, ya que s_services almacena IConfig.
    template<typename T>
    static std::shared_ptr<T> GetConfig() {
        //const std::string name1 = typeid(T).name();
		const std::string name = T::GetStaticConfigName();
		auto& s_serviceEntries = GetConfigEntries();
        auto it = s_serviceEntries.find(name);
        if (it != s_serviceEntries.end()) {
            return std::dynamic_pointer_cast<T>(it->second.instance);
        }
        return nullptr;
    }
    template <typename TConfig>
    static std::shared_ptr<TConfig> GetConfig(const std::string& name) {
        auto& entries = ConfigLocator::GetConfigEntries();
        auto it = entries.find(name);
        if (it != entries.end()) {
			auto instance = std::dynamic_pointer_cast<TConfig>(it->second.instance);
			return instance ? instance : nullptr;
        }
        return nullptr;
    }

    // Función para registrar directamente un servicio ya creado (para casos especiales o tests).
    // También usa T::GetStaticConfigName() como clave para mantener la consistencia.
    // Ahora acepta std::shared_ptr<IConfig> o un tipo que herede de IConfig.
    template<typename T>
    // T debe ser un IConfig o heredar de IConfig
    static void RegisterConfig(std::shared_ptr<T> service) {
        // Asume que la clase T tiene un método estático 'GetStaticConfigName()'.
        // La conversión implícita de std::shared_ptr<T> a std::shared_ptr<IConfig> es segura.
        static_assert(std::is_base_of<IConfig, T>::value, "T must derive from IConfig for ConfigLocator::RegisterConfig");
        auto& s_serviceEntries = GetConfigEntries();
        s_serviceEntries[service->GetStaticConfigName()] = service;
        //s_services[T::GetStaticConfigName()] = service; 
    }

    // Función para limpiar todos los servicios registrados y sus creadores.
    // Este método es fundamental para el apagado correcto y la liberación de recursos.
    static void Shutdown();

    struct ConfigEntry {
        std::shared_ptr<IConfig> instance; // La instancia real del config
        CreateConfigLambda creator;
    };
private:
    static std::map<std::string, ConfigEntry>& GetConfigEntries();
};