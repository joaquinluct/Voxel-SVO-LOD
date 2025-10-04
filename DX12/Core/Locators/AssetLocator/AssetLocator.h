// AssetLocator.h
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
    #include <windows.h> // Para HRESULT
#endif

// Forward declarations para evitar dependencias circulares
// TODO: Estas clases serán implementadas posteriormente
class IAsset;
class AssetBase;
class MaterialAsset;
class MeshAsset;
class ShaderAsset;
class TerrainAsset;
class TextureAsset;
class VertexAsset;
class ConfigBase;
class IInitializable;
class IEngineDependentInitializable;

// TODO: Activar cuando ConfigLocator esté implementado
// class ConfigLocator;

// Define el tipo para las lambdas de asset
// Retorna std::shared_ptr<AssetBase>, ya que AssetBase es nuestra base polimórfica común
using CreateAssetLambda = std::function<std::shared_ptr<AssetBase>()>;
using InitializeAssetLambda = std::function<HRESULT(std::shared_ptr<AssetBase>)>;
using RenderAssetLambda = std::function<void(std::shared_ptr<AssetBase>)>;

class AssetLocator {
public:
    // Singleton pattern
    AssetLocator() = default;
    AssetLocator(const AssetLocator&) = delete;
    AssetLocator& operator=(const AssetLocator&) = delete;

    static AssetLocator& GetInstance();

    // Registra las lambdas de creación e inicialización para un asset
    // El 'name' debe coincidir con la clave en tu archivo YAML para este asset
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

    // Funciones getter para assets específicos
    static std::shared_ptr<AssetBase> GetAsset(const std::string& name);
    static std::shared_ptr<AssetBase> GetAssetBase(const std::string& name);
    
    // TODO: Activar cuando los assets específicos estén implementados
    /*
    static std::shared_ptr<MeshAsset> GetMeshAsset(const std::string& name);
    static std::shared_ptr<VertexAsset> GetVertexAsset(const std::string& name);
    static std::shared_ptr<TextureAsset> GetTextureAsset(const std::string& name);
    static std::shared_ptr<TerrainAsset> GetTerrainAsset(const std::string& name);
    static std::shared_ptr<ShaderAsset> GetShaderAsset(const std::string& name);
    static std::vector<std::shared_ptr<ShaderAsset>> GetAllShaderAsset();
    static std::vector<std::shared_ptr<ShaderAsset>> GetShaders();
    */

    // Función para obtener un asset ya inicializado por su tipo C++
    // Usa dynamic_pointer_cast internamente, ya que almacenamos AssetBase
    template<typename T>
    static std::shared_ptr<T> GetAssetBase() {
        const std::string name = T::GetStaticAssetName();
        auto& assetEntries = GetAssetBaseEntries();
        auto it = assetEntries.find(name);
        if (it != assetEntries.end()) {
            return std::dynamic_pointer_cast<T>(it->second.instance);
        }
        return nullptr;
    }

    template<typename T>
    static std::shared_ptr<T> GetAsset(const std::string& name) {
        auto& assetEntries = GetAssetEntries();
        auto it = assetEntries.find(name);
        if (it != assetEntries.end()) {
            std::shared_ptr<T> asset = std::dynamic_pointer_cast<T>(it->second);
            
            // TODO: Activar cuando ConfigLocator esté implementado
            /*
            const std::string configName = name + "Config";
            std::shared_ptr<ConfigBase> config = ConfigLocator::GetConfig<ConfigBase>(configName);
            if (config) {
                asset->SetConfig(config);
            }
            */
            
            return asset;
        }
        return nullptr;
    }

    // Función para registrar directamente un asset ya creado (para casos especiales o tests)
    // Acepta std::shared_ptr<AssetBase> o un tipo que herede de AssetBase
    template<typename T>
    static void RegisterAsset(std::shared_ptr<T> asset) {
        static_assert(std::is_base_of<AssetBase, T>::value, 
                     "T must derive from AssetBase for AssetLocator::RegisterAsset");
        auto& assetEntries = GetAssetBaseEntries();
        assetEntries[asset->GetStaticAssetName()] = {asset, nullptr, nullptr, nullptr};
    }

    // Función para limpiar todos los assets registrados y sus creadores
    static void Shutdown();

    struct AssetEntry {
        std::shared_ptr<AssetBase> instance;     // La instancia real del asset
        CreateAssetLambda creator;               // Lambda de creación
        InitializeAssetLambda initializer;       // Lambda de inicialización
        RenderAssetLambda renderer;              // Lambda de render
    };

private:
    static std::map<std::string, AssetEntry>& GetAssetBaseEntries();
    static std::map<std::string, std::shared_ptr<AssetBase>>& GetAssetEntries();
};
