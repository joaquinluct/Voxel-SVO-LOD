// DefineLocator.h
#pragma once

#include <string>
#include <memory>
#include <functional>
#include <map>
#include <windows.h> // Para HWND, HRESULT
#include <Defines/VertexDefinition.h>
#include <ConfigLocator/ConfigLocator.h>
#include <IDefine.h>
#include <IDefine/IVertex.h>

// #include <typeindex> // No es estrictamente necesario si usamos nombres de string para s_defines

// Forward declarations para asegurar que IDefine, IInitializable, IWindowDependentInitializable
// estén declaradas antes de ser usadas en std::shared_ptr en las lambdas.
// Si estas interfaces están definidas en sus propios archivos .h y esos archivos
// ya se incluyen antes de DefineLocator.h en algunos .cpp, podrías no necesitar
// estas forward declarations aquí, pero es una buena práctica para prevenir
// dependencias circulares o problemas de orden de inclusión.
class IInitializable;
class IWindowDependentInitializable;

// Define el tipo para la lambda de creación.
// Ahora retorna std::shared_ptr<IDefine>, ya que IDefine es nuestra base polimórfica común.
using CreateDefineLambda = std::function<std::shared_ptr<IVertex>()>;

class DefineLocator {
public:
    // Constructor privado para Singleton
    DefineLocator() = default;
    DefineLocator(const DefineLocator&) = delete;
    DefineLocator& operator=(const DefineLocator&) = delete;

    static DefineLocator& GetInstance();

    // Registra las lambdas de creación e inicialización para un servicio.
    // El 'name' debe coincidir con la clave en tu archivo YAML para este manager.
    static void RegisterDefineCreator(
        const std::string& name,
        CreateDefineLambda createFn
    );

    
    static HRESULT InitializeDefines();

    // Función para obtener un servicio ya inicializado por su tipo C++.
    // Ahora usa dynamic_pointer_cast internamente, ya que s_defines almacena IDefine.
    template<typename T>
    static std::shared_ptr<T> GetDefine() {
        const std::string name1 = typeid(T).name();
		const std::string name = T::GetStaticDefineName();
		auto& s_defineEntries = GetDefineEntries();
        auto it = s_defineEntries.find(name);
        if (it != s_defineEntries.end()) {
            return std::dynamic_pointer_cast<T>(it->second.instance);
        }
        return nullptr;
    }
    static std::shared_ptr<IVertex> GetDefine(const std::string& name);
    static std::shared_ptr<IVertex> GetVertexDefine(const std::string& name);
    static std::vector < std::shared_ptr<VertexDefinition::VertexVariant>> GetVertexDefineAsVector(const std::string& name);

    // Función para registrar directamente un servicio ya creado (para casos especiales o tests).
    // También usa T::GetStaticDefineName() como clave para mantener la consistencia.
    // Ahora acepta std::shared_ptr<IDefine> o un tipo que herede de IDefine.
    template<typename T>
    // T debe ser un IDefine o heredar de IDefine
    static void RegisterDefine(std::shared_ptr<T> define) {
        // Asume que la clase T tiene un método estático 'GetStaticDefineName()'.
        // La conversión implícita de std::shared_ptr<T> a std::shared_ptr<IDefine> es segura.
        static_assert(std::is_base_of<IVertex, T>::value, "T must derive from IDefine for DefineLocator::RegisterDefine");
        auto& s_defineEntries = GetDefineEntries();
        s_defineEntries[define->GetStaticDefineName()] = define;
        //s_defines[T::GetStaticDefineName()] = define; 
    }

    // Función para limpiar todos los servicios registrados y sus creadores.
    // Este método es fundamental para el apagado correcto y la liberación de recursos.
    static void Shutdown();

    struct DefineEntry {
        std::shared_ptr<IVertex> instance; // La instancia real del manager
        CreateDefineLambda creator;
    };
private:
    static std::map<std::string, DefineEntry>& GetDefineEntries();
};