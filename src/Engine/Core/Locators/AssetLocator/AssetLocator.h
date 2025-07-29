// AssetLocator.h
#pragma once

#include <Core/Defines/Asset.h>
#include <Assets/Base/AssetBase.h>
#include <Assets/Base/MeshAsset.h>
#include <Assets/Base/ShaderAsset.h>
#include <Assets/Base/MaterialAsset.h>
#include <Assets/Base/VertexAsset.h>
//#include <Assets/Base/TextureAsset.h>
#include <string>
#include <memory>
#include <functional>
#include <map>
#include <windows.h> // Para HWND, HRESULT
#include <ConfigLocator/ConfigLocator.h> // Para ConfigLocator::GetInstance()
#include <ConfigBase.h>
#include <Config/ITextureConfig.h>
#include <Config/ITextureMapConfig.h>
#include <IAssetShaderConfig.h>


// Si no está en un namespace:
class TextureAsset; 
class ShaderAsset;
class IAsset;
class IInitializable;
class IWindowDependentInitializable;

// Define el tipo para la lambda de creación.
// Ahora retorna std::shared_ptr<IAsset>, ya que IAsset es nuestra base polimórfica común.
using CreateAssetLambda = std::function<std::shared_ptr<AssetBase>()>;

// Define el tipo para la lambda de inicialización universal.
// Ahora recibe std::shared_ptr<IAsset> en lugar de std::shared_ptr<void>.
// ESTA ES LA FIRMA QUE DEBE COINCIDIR EXACTAMENTE CON LA MACRO REGISTER_MANAGER_TYPE.
using InitializeAssetLambda = std::function<HRESULT(std::shared_ptr<AssetBase>)>;
using RenderAssetLambda = std::function<void(std::shared_ptr<AssetBase>)>;

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


    static HRESULT InitializeAssets(const std::vector<std::string>& orderList, const std::string& filterType = "");
    static HRESULT InitializeBaseAssets(const std::vector<std::string>& orderList);

    static HRESULT InitializeShaders(const std::vector<std::string>& orderList);

    static HRESULT RenderAssets(const std::vector<std::string>& orderList);
    //static HRESULT UpdateAssets(const std::vector<std::string>& orderList, float deltaTime);
    //static void ShutdownAssets(const std::vector<std::string>& orderList); // Shutdown suele ser void

    static std::shared_ptr<AssetBase> GetAsset(const std::string& name);
    static std::shared_ptr<VertexAsset> GetVertexAsset(const std::string& name);
    static std::shared_ptr<TextureAsset> GetTextureAsset(const std::string& name);
    static std::shared_ptr<ShaderAsset> GetShaderAsset(const std::string& name);
    static std::shared_ptr<AssetBase> GetAssetBase(const std::string& name);

    static std::vector<std::shared_ptr<ShaderAsset>> GetAllShaderAsset();

    // Función para obtener un servicio ya inicializado por su tipo C++.
    // Ahora usa dynamic_pointer_cast internamente, ya que s_services almacena IAsset.
    template<typename T>
    static std::shared_ptr<T> GetAssetBase() {
        //const std::string name1 = typeid(T).name();
        const std::string name = T::GetStaticAssetName();
        auto& s_serviceEntries = GetAssetBaseEntries();
        auto it = s_serviceEntries.find(name);
        if (it != s_serviceEntries.end()) {
            return std::dynamic_pointer_cast<T>(it->second.instance);
        }
        return nullptr;
    }

    template<typename T>
    static std::shared_ptr<T> GetAsset(std::string name) {
        auto& s_serviceEntries = GetAssetEntries();
        auto it = s_serviceEntries.find(name);
        if (it != s_serviceEntries.end()) {
            std::shared_ptr<T> asset = std::dynamic_pointer_cast<T>(it->second);
            const std::string configName = name + "Config";
            std::shared_ptr<ConfigBase> config = ConfigLocator::GetConfig<ConfigBase>(configName);
            if (config.get()) {
                asset->SetConfig(config);
            }
            return asset;
        }
        return nullptr;
    }

    static std::vector<std::shared_ptr<ShaderAsset>> GetShaders() {
        auto& s_assetEntries = GetAssetEntries();
		// Itera y develve todos los s_assetEntries de tipo ShaderAsset
        std::vector<std::shared_ptr<ShaderAsset>> shaders;
        for (const auto& entry : s_assetEntries) {
            auto shader = std::dynamic_pointer_cast<ShaderAsset>(entry.second);
            if (shader) {
                const std::string configName = entry.first + "Config";
                std::shared_ptr<IAssetShaderConfig> config = ConfigLocator::GetConfig<IAssetShaderConfig>(configName);
                if (config.get()) {
                    shader->SetName(config->name);
                    shader->SetConfig(config);
                }
                //std::shared_ptr<ShaderAsset> m_shader = std::make_shared<ShaderAsset>(shader);
                shaders.push_back(shader->CloneShader());
            }
        }
		return shaders;
    }

    // Función para registrar directamente un servicio ya creado (para casos especiales o tests).
    // También usa T::GetStaticAssetName() como clave para mantener la consistencia.
    // Ahora acepta std::shared_ptr<IAsset> o un tipo que herede de IAsset.
    template<typename T>
    // T debe ser un IAsset o heredar de IAsset
    static void RegisterAsset(std::shared_ptr<T> service) {
        // Asume que la clase T tiene un método estático 'GetStaticAssetName()'.
        // La conversión implícita de std::shared_ptr<T> a std::shared_ptr<IAsset> es segura.
        static_assert(std::is_base_of<AssetBase, T>::value, "T must derive from IAsset for AssetLocator::RegisterAsset");
        auto& s_serviceEntries = GetAssetBaseEntries();
        s_serviceEntries[service->GetStaticAssetName()] = service;
        //s_services[T::GetStaticAssetName()] = service; 
    }

    // Función para limpiar todos los servicios registrados y sus creadores.
    // Este método es fundamental para el apagado correcto y la liberación de recursos.
    static void Shutdown();

    struct AssetEntry {
        std::shared_ptr<AssetBase> instance; // La instancia real del manager
        CreateAssetLambda creator;
        InitializeAssetLambda initializer;
        RenderAssetLambda renderer; // La lambda de render que definiste en la macro
        // ... otras lambdas (Update, Shutdown)
    };
private:
    static std::map<std::string, AssetEntry>& GetAssetBaseEntries();
    static std::map<std::string, std::shared_ptr<AssetBase>>& GetAssetEntries();
};