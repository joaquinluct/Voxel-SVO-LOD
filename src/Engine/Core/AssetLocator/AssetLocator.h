// AssetLocator.h
#pragma once

#include <string>
#include <memory>
#include <functional>
#include <map>
#include <windows.h> // Para HWND, HRESULT
#include <Assets/IAsset.h>

class IAsset;
class IInitializable;
class IWindowDependentInitializable;

// Define el tipo para la lambda de creación.
// Ahora retorna std::shared_ptr<IAsset>, ya que IAsset es nuestra base polimórfica común.
using CreateAssetLambda = std::function<std::shared_ptr<IAsset>()>;

// Define el tipo para la lambda de inicialización universal.
// Ahora recibe std::shared_ptr<IAsset> en lugar de std::shared_ptr<void>.
// ESTA ES LA FIRMA QUE DEBE COINCIDIR EXACTAMENTE CON LA MACRO REGISTER_MANAGER_TYPE.
using InitializeAssetLambda = std::function<HRESULT(std::shared_ptr<IAsset>)>;
using RenderAssetLambda = std::function<void(std::shared_ptr<IAsset>)>;

class AssetLocator {
public:
    // Constructor privado para Singleton
    AssetLocator() = default;
    AssetLocator(const AssetLocator&) = delete;
    AssetLocator& operator=(const AssetLocator&) = delete;

    static AssetLocator& GetInstance();

    // Registra las lambdas de creación e inicialización para un servicio.
    // El 'name' debe coincidir con la clave en tu archivo YAML para este manager.
    static void RegisterAssetCreator(
        const std::string& name,
        CreateAssetLambda createFn,
        InitializeAssetLambda initFn,
        RenderAssetLambda renderFn
    );


    static HRESULT InitializeAssets(const std::vector<std::string>& orderList);
    static HRESULT RenderAssets(const std::vector<std::string>& orderList);
    //static HRESULT UpdateAssets(const std::vector<std::string>& orderList, float deltaTime);
    //static void ShutdownAssets(const std::vector<std::string>& orderList); // Shutdown suele ser void

    // Función para obtener un servicio ya inicializado por su tipo C++.
    // Ahora usa dynamic_pointer_cast internamente, ya que s_services almacena IAsset.
    template<typename T>
    static std::shared_ptr<T> GetAsset() {
        const std::string name1 = typeid(T).name();
        const std::string name = T::GetStaticAssetName();
        auto& s_serviceEntries = GetAssetEntries();
        auto it = s_serviceEntries.find(name);
        if (it != s_serviceEntries.end()) {
            return std::dynamic_pointer_cast<T>(it->second.instance);
        }
        return nullptr;
    }
    static std::shared_ptr<IAsset> GetAsset(const std::string& name);

    // Función para registrar directamente un servicio ya creado (para casos especiales o tests).
    // También usa T::GetStaticAssetName() como clave para mantener la consistencia.
    // Ahora acepta std::shared_ptr<IAsset> o un tipo que herede de IAsset.
    template<typename T>
    // T debe ser un IAsset o heredar de IAsset
    static void RegisterAsset(std::shared_ptr<T> service) {
        // Asume que la clase T tiene un método estático 'GetStaticAssetName()'.
        // La conversión implícita de std::shared_ptr<T> a std::shared_ptr<IAsset> es segura.
        static_assert(std::is_base_of<IAsset, T>::value, "T must derive from IAsset for AssetLocator::RegisterAsset");
        auto& s_serviceEntries = GetAssetEntries();
        s_serviceEntries[service->GetStaticAssetName()] = service;
        //s_services[T::GetStaticAssetName()] = service; 
    }

    // Función para limpiar todos los servicios registrados y sus creadores.
    // Este método es fundamental para el apagado correcto y la liberación de recursos.
    static void Shutdown();

    struct AssetEntry {
        std::shared_ptr<IAsset> instance; // La instancia real del manager
        CreateAssetLambda creator;
        InitializeAssetLambda initializer;
        RenderAssetLambda renderer; // La lambda de render que definiste en la macro
        // ... otras lambdas (Update, Shutdown)
    };
private:
    static std::map<std::string, AssetEntry>& GetAssetEntries();
};