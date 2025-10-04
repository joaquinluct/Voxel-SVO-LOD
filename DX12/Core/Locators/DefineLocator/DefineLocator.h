// DefineLocator.h
#pragma once

// Standard C++23 includes (alfabético)
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

// Platform specific
#ifdef _WIN32
    #include <windows.h>
#endif

// Forward declarations para evitar dependencias circulares
// TODO: Estas interfaces serán implementadas posteriormente
class IInitializable;
class IEngineDependentInitializable;
class IVertex;
class IDefine;

// TODO: Activar cuando VertexDefinition esté implementado
// namespace VertexDefinition {
//     class VertexVariant;
// }

// Define el tipo para la lambda de creación
// Retorna std::shared_ptr<IVertex>, ya que IVertex es nuestra base polimórfica común
using CreateDefineLambda = std::function<std::shared_ptr<IVertex>()>;

class DefineLocator {
public:
    // Singleton pattern
    DefineLocator() = default;
    DefineLocator(const DefineLocator&) = delete;
    DefineLocator& operator=(const DefineLocator&) = delete;

    static DefineLocator& GetInstance();

    // Registra las lambdas de creación para un define
    // El 'name' debe coincidir con la clave en tu archivo YAML para este define
    static void RegisterDefineCreator(
        const std::string& name,
        CreateDefineLambda createFn
    );

    static HRESULT InitializeDefines();

    // Función para obtener un define ya inicializado por su tipo C++
    // Usa dynamic_pointer_cast internamente, ya que s_defines almacena IVertex
    template<typename T>
    static std::shared_ptr<T> GetDefine() {
        const std::string name = T::GetStaticDefineName();
        auto& defineEntries = GetDefineEntries();
        auto it = defineEntries.find(name);
        if (it != defineEntries.end()) {
            return std::dynamic_pointer_cast<T>(it->second.instance);
        }
        return nullptr;
    }
    
    static std::shared_ptr<IVertex> GetDefine(const std::string& name);
    static std::shared_ptr<IVertex> GetVertexDefine(const std::string& name);
    
    // TODO: Activar cuando VertexDefinition esté implementado
    // static std::vector<std::shared_ptr<VertexDefinition::VertexVariant>> GetVertexDefinitionAsVector(const std::string& name);

    // Función para registrar directamente un define ya creado (para casos especiales o tests)
    // Acepta std::shared_ptr<IVertex> o un tipo que herede de IVertex
    template<typename T>
    static void RegisterDefine(std::shared_ptr<T> define) {
        static_assert(std::is_base_of<IVertex, T>::value, 
                     "T must derive from IVvertex for DefineLocator::RegisterDefine");
        auto& defineEntries = GetDefineEntries();
        defineEntries[define->GetStaticDefineName()] = {define, nullptr};
    }

    // Función para limpiar todos los defines registrados y sus creadores
    static void Shutdown();

    struct DefineEntry {
        std::shared_ptr<IVertex> instance;    // La instancia real del define
        CreateDefineLambda creator;           // Lambda de creación
    };

private:
    static std::map<std::string, DefineEntry>& GetDefineEntries();
};
