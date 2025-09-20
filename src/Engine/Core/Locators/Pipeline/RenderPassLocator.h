// RenderPassLocator.h
#pragma once

#include <string>
#include <memory>
#include <functional>
#include <map>
#include <windows.h> // Para HWND, HRESULT
#include <IRenderPass.h>
#include <RenderManager/RenderPass.h>

// #include <typeindex> // No es estrictamente necesario si usamos nombres de string para s_services

// Forward declarations para asegurar que IRenderPass, IInitializable, IEngineDependentInitializable
// estén declaradas antes de ser usadas en std::shared_ptr en las lambdas.
// Si estas interfaces están definidas en sus propios archivos .h y esos archivos
// ya se incluyen antes de RenderPassLocator.h en algunos .cpp, podrías no necesitar
// estas forward declarations aquí, pero es una buena práctica para prevenir
// dependencias circulares o problemas de orden de inclusión.
class IInitializable;
class IEngineDependentInitializable;

// Define el tipo para la lambda de creación.
// Ahora retorna std::shared_ptr<IRenderPass>, ya que IRenderPass es nuestra base polimórfica común.
using CreateRenderPassLambda = std::function<std::shared_ptr<IRenderPass>()>;

// Define el tipo para la lambda de inicialización universal.
// Ahora recibe std::shared_ptr<IRenderPass> en lugar de std::shared_ptr<void>.
// ESTA ES LA FIRMA QUE DEBE COINCIDIR EXACTAMENTE CON LA MACRO REGISTER_MANAGER_TYPE.
using InitializeRenderPassLambda = std::function<HRESULT(std::shared_ptr<IRenderPass>)>;

class RenderPassLocator {
public:
    // Constructor privado para Singleton
    RenderPassLocator() = default;
    RenderPassLocator(const RenderPassLocator&) = delete;
    RenderPassLocator& operator=(const RenderPassLocator&) = delete;

    static RenderPassLocator& GetInstance();

    // Registra las lambdas de creación e inicialización para un servicio.
    // El 'name' debe coincidir con la clave en tu archivo YAML para este manager.
    static void RegisterRenderPassCreator(
        const std::string& name,
        CreateRenderPassLambda createFn
    );

    
    static HRESULT CreateRenderPasss();

    // Función para obtener un servicio ya inicializado por su tipo C++.
    // Ahora usa dynamic_pointer_cast internamente, ya que s_services almacena IRenderPass.
    template<typename T>
    static std::shared_ptr<T> GetRenderPass() {
        //const std::string name1 = typeid(T).name();
		const std::string name = T::GetStaticRenderPassName();
		auto& s_serviceEntries = GetRenderPassEntries();
        auto it = s_serviceEntries.find(name);
        if (it != s_serviceEntries.end()) {
            return std::dynamic_pointer_cast<T>(it->second.instance);
        }
        return nullptr;
    }
    template <typename TRenderPass>
    static std::shared_ptr<TRenderPass> GetRenderPass(const std::string& name) {
        auto& entries = RenderPassLocator::GetRenderPassEntries();
        auto it = entries.find(name);
        if (it != entries.end()) {
			auto instance = std::dynamic_pointer_cast<TRenderPass>(it->second.instance);
			return instance ? instance : nullptr;
        }
        return nullptr;
    }

    static std::shared_ptr<RenderPass> GetRenderPass(const std::string& name) {
        auto& entries = RenderPassLocator::GetRenderPassEntries();
        auto it = entries.find(name);
        if (it != entries.end()) {
            auto instance = std::dynamic_pointer_cast<RenderPass>(it->second.instance);
            return instance ? instance : nullptr;
        }
        return nullptr;
    }

    // Función para registrar directamente un servicio ya creado (para casos especiales o tests).
    // También usa T::GetStaticRenderPassName() como clave para mantener la consistencia.
    // Ahora acepta std::shared_ptr<IRenderPass> o un tipo que herede de IRenderPass.
    template<typename T>
    // T debe ser un IRenderPass o heredar de IRenderPass
    static void RegisterRenderPass(std::shared_ptr<T> service) {
        // Asume que la clase T tiene un método estático 'GetStaticRenderPassName()'.
        // La conversión implícita de std::shared_ptr<T> a std::shared_ptr<IRenderPass> es segura.
        static_assert(std::is_base_of<IRenderPass, T>::value, "T must derive from IRenderPass for RenderPassLocator::RegisterRenderPass");
        auto& s_serviceEntries = GetRenderPassEntries();
        s_serviceEntries[service->GetStaticRenderPassName()] = service;
        //s_services[T::GetStaticRenderPassName()] = service; 
    }

    // Función para limpiar todos los servicios registrados y sus creadores.
    // Este método es fundamental para el apagado correcto y la liberación de recursos.
    static void Shutdown();

    struct RenderPassEntry {
        std::shared_ptr<IRenderPass> instance; // La instancia real del config
        CreateRenderPassLambda creator;
    };
private:
    static std::map<std::string, RenderPassEntry>& GetRenderPassEntries();
};