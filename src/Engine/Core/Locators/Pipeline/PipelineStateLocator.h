// PipelineStateLocator.h
#pragma once

#include <string>
#include <memory>
#include <functional>
#include <map>
#include <windows.h> // Para HWND, HRESULT
#include <IPipelineState.h>

// #include <typeindex> // No es estrictamente necesario si usamos nombres de string para s_services

// Forward declarations para asegurar que IPipelineState, IInitializable, IEngineDependentInitializable
// estén declaradas antes de ser usadas en std::shared_ptr en las lambdas.
// Si estas interfaces están definidas en sus propios archivos .h y esos archivos
// ya se incluyen antes de PipelineStateLocator.h en algunos .cpp, podrías no necesitar
// estas forward declarations aquí, pero es una buena práctica para prevenir
// dependencias circulares o problemas de orden de inclusión.
class IInitializable;
class IEngineDependentInitializable;

// Define el tipo para la lambda de creación.
// Ahora retorna std::shared_ptr<IPipelineState>, ya que IPipelineState es nuestra base polimórfica común.
using CreatePipelineStateLambda = std::function<std::shared_ptr<IPipelineState>()>;

// Define el tipo para la lambda de inicialización universal.
// Ahora recibe std::shared_ptr<IPipelineState> en lugar de std::shared_ptr<void>.
// ESTA ES LA FIRMA QUE DEBE COINCIDIR EXACTAMENTE CON LA MACRO REGISTER_MANAGER_TYPE.
using InitializePipelineStateLambda = std::function<HRESULT(std::shared_ptr<IPipelineState>)>;

class PipelineStateLocator {
public:
    // Constructor privado para Singleton
    PipelineStateLocator() = default;
    PipelineStateLocator(const PipelineStateLocator&) = delete;
    PipelineStateLocator& operator=(const PipelineStateLocator&) = delete;

    static PipelineStateLocator& GetInstance();

    // Registra las lambdas de creación e inicialización para un servicio.
    // El 'name' debe coincidir con la clave en tu archivo YAML para este manager.
    static void RegisterPipelineStateCreator(
        const std::string& name,
        CreatePipelineStateLambda createFn
    );


    static HRESULT CreatePipelineStates();

    // Función para obtener un servicio ya inicializado por su tipo C++.
    // Ahora usa dynamic_pointer_cast internamente, ya que s_services almacena IPipelineState.
    template<typename T>
    static std::shared_ptr<T> GetPipelineState() {
        //const std::string name1 = typeid(T).name();
        const std::string name = T::GetStaticPipelineStateName();
        auto& s_serviceEntries = GetPipelineStateEntries();
        auto it = s_serviceEntries.find(name);
        if (it != s_serviceEntries.end()) {
            return std::dynamic_pointer_cast<T>(it->second.instance);
        }
        return nullptr;
    }
    template <typename TPipelineState>
    static std::shared_ptr<TPipelineState> GetPipelineState(const std::string& name) {
        auto& entries = PipelineStateLocator::GetPipelineStateEntries();
        auto it = entries.find(name);
        if (it != entries.end()) {
            auto instance = std::dynamic_pointer_cast<TPipelineState>(it->second.instance);
            return instance ? instance : nullptr;
        }
        return nullptr;
    }

    static std::shared_ptr<IPipelineState> GetPipelineState(const std::string& name) {
        auto& entries = PipelineStateLocator::GetPipelineStateEntries();
        auto it = entries.find(name);
        if (it != entries.end()) {
            auto instance = std::dynamic_pointer_cast<IPipelineState>(it->second.instance);
            return instance ? instance : nullptr;
        }
        return nullptr;
    }

    // Función para registrar directamente un servicio ya creado (para casos especiales o tests).
    // También usa T::GetStaticPipelineStateName() como clave para mantener la consistencia.
    // Ahora acepta std::shared_ptr<IPipelineState> o un tipo que herede de IPipelineState.
    template<typename T>
    // T debe ser un IPipelineState o heredar de IPipelineState
    static void RegisterPipelineState(std::shared_ptr<T> service) {
        // Asume que la clase T tiene un método estático 'GetStaticPipelineStateName()'.
        // La conversión implícita de std::shared_ptr<T> a std::shared_ptr<IPipelineState> es segura.
        static_assert(std::is_base_of<IPipelineState, T>::value, "T must derive from IPipelineState for PipelineStateLocator::RegisterPipelineState");
        auto& s_serviceEntries = GetPipelineStateEntries();
        s_serviceEntries[service->GetStaticPipelineStateName()] = service;
        //s_services[T::GetStaticPipelineStateName()] = service; 
    }

    // Función para limpiar todos los servicios registrados y sus creadores.
    // Este método es fundamental para el apagado correcto y la liberación de recursos.
    static void Shutdown();

    struct PipelineStateEntry {
        std::shared_ptr<IPipelineState> instance; // La instancia real del config
        CreatePipelineStateLambda creator;
    };
private:
    static std::map<std::string, PipelineStateEntry>& GetPipelineStateEntries();
};
