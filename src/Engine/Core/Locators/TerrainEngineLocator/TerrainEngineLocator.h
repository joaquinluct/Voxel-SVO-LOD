// TerrainEngineLocator.h
#pragma once

#include <functional>
#include <Game/Systems/Terrain/Defines/TerrainEngineBase.h>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <windows.h>

class IInitializable;
class IEngineDependentInitializable;

// Define el tipo para la lambda de creación.
// Ahora retorna std::shared_ptr<TerrainEngineBase>, ya que TerrainEngineBase es nuestra base polimórfica común.
using CreateTerrainEngineLambda = std::function<std::shared_ptr<TerrainEngineBase>()>;

// Define el tipo para la lambda de inicialización universal.
// Ahora recibe std::shared_ptr<TerrainEngineBase> en lugar de std::shared_ptr<void>.
// ESTA ES LA FIRMA QUE DEBE COINCIDIR EXACTAMENTE CON LA MACRO REGISTER_MANAGER_TYPE.
using InitializeTerrainEngineLambda = std::function<HRESULT(std::shared_ptr<TerrainEngineBase>)>;

class TerrainEngineLocator {
public:
    // Constructor privado para Singleton
    TerrainEngineLocator() = default;
    TerrainEngineLocator(const TerrainEngineLocator&) = delete;
    TerrainEngineLocator& operator=(const TerrainEngineLocator&) = delete;

    static TerrainEngineLocator& GetInstance();

    // Registra las lambdas de creación e inicialización para un servicio.
    // El 'name' debe coincidir con la clave en tu archivo YAML para este manager.
    static void RegisterTerrainEngineCreator(
        const std::string& name,
        CreateTerrainEngineLambda createFn
    );


    static HRESULT CreateTerrainEngines();
    static HRESULT CreateTerrainEngine(const std::string& engineName);

    // Función para obtener un servicio ya inicializado por su tipo C++.
    // Ahora usa dynamic_pointer_cast internamente, ya que s_services almacena TerrainEngineBase.
    template<typename T>
    static std::shared_ptr<T> GetEngine() {
        //const std::string name1 = typeid(T).name();
        const std::string name = T::GetStaticTerrainEngineName();
        auto& s_serviceEntries = GetTerrainEngineEntries();
        auto it = s_serviceEntries.find(name);
        if (it != s_serviceEntries.end()) {
            return std::dynamic_pointer_cast<T>(it->second.instance);
        }
        return nullptr;
    }
    template <typename TTerrainEngine>
    static std::shared_ptr<TTerrainEngine> GetEngine(const std::string& name) {
        auto& entries = TerrainEngineLocator::GetTerrainEngineEntries();
        auto it = entries.find(name);
        if (it != entries.end()) {
            auto instance = std::dynamic_pointer_cast<TTerrainEngine>(it->second.instance);
            return instance ? instance : nullptr;
        }
        return nullptr;
    }

    // Función para registrar directamente un servicio ya creado (para casos especiales o tests).
    // También usa T::GetStaticTerrainEngineName() como clave para mantener la consistencia.
    // Ahora acepta std::shared_ptr<TerrainEngineBase> o un tipo que herede de TerrainEngineBase.
    template<typename T>
    // T debe ser un TerrainEngineBase o heredar de TerrainEngineBase
    static void RegisterTerrainEngine(std::shared_ptr<T> service) {
        // Asume que la clase T tiene un método estático 'GetStaticTerrainEngineName()'.
        // La conversión implícita de std::shared_ptr<T> a std::shared_ptr<TerrainEngineBase> es segura.
        static_assert(std::is_base_of<TerrainEngineBase, T>::value, "T must derive from TerrainEngineBase for TerrainEngineLocator::RegisterTerrainEngine");
        auto& s_serviceEntries = GetTerrainEngineEntries();
        s_serviceEntries[service->GetStaticTerrainEngineName()] = service;
        //s_services[T::GetStaticTerrainEngineName()] = service; 
    }

    // Función para limpiar todos los servicios registrados y sus creadores.
    // Este método es fundamental para el apagado correcto y la liberación de recursos.
    static void Shutdown();

    struct TerrainEngineEntry {
        std::shared_ptr<TerrainEngineBase> instance; // La instancia real del terrainEngine
        CreateTerrainEngineLambda creator;
    };
private:
    static std::map<std::string, TerrainEngineEntry>& GetTerrainEngineEntries();
};
