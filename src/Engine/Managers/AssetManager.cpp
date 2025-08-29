// AssetManager.cpp
#include "AssetManager.h"
#include "DeviceManager.h"
#include "ShaderManager.h"
#include <windows.h>
#include <chrono>
#include "Assets/Base/TextureAsset.h"
#include <stdexcept>
#include <Assets/Base/Commands/CreateMeshGPUCommand.h>
#include <yaml-cpp/yaml.h>
#include <Assets/Base/Commands/CreateTextureGPUCommand.h>
#include <AssetLocator/AssetLocator.h>
#include <ConfigLocator/ConfigLocator.h>
#include "REGISTER_MANAGER_MACRO.h"
#include <TextureLoader/TextureLoader.h>
#include <ITextureConfig.h>

REGISTER_MANAGER_TYPE(AssetManager, "AssetManager")

AssetManager::AssetManager() {
    // Constructor. m_loadContext se construirá por defecto.
    // Sus miembros (pDevice, pImmediateContext, etc.) se inicializarán a nullptr si son punteros
    // o shared_ptr vacíos si son smart pointers.
    // Los valores correctos se asignan en Init().
	m_config = new MainIndexConfig();    
}

AssetManager::~AssetManager() {
    Shutdown();
}

HRESULT AssetManager::InitShaders(HWND* hwnd, int width, int height) {
    if (!m_config) {
        OutputDebugStringA("AssetManager InitShaders: m_config is null.\n");
		return E_FAIL; // Error si m_config no está inicializado
    }

    return S_OK;
}

HRESULT AssetManager::Init(HWND* hwnd, int width, int height) {
    /*OutputDebugStringA("Incializando AssetManager...\n");

    m_configBase = new BaseIndexConfig();


    HRESULT hr = AssetLocator::InitializeBaseAssets(m_configBase->index);
    if (FAILED(hr)) {
        OutputDebugStringA("MainController Init: Failed to initialize services.\n");
        return hr;
    }

    hr = AssetLocator::InitializeShaders(m_config->mainIndex);

    m_config = new MainIndexConfig();

    hr = AssetLocator::InitializeAssets(m_config->mainIndex);
    if (FAILED(hr)) {
        OutputDebugStringA("MainController Init: Failed to initialize services.\n");
        return hr;
    }
    OutputDebugStringA(("Resultado Init " + std::to_string(hr) + " en AssetManager\n").c_str());
    return hr;*/
	return S_OK; // Retorna S_OK si la inicialización fue exitosa
}

void AssetManager::Render() {
    // AssetManager no renderiza por sí mismo.
    // Asegúrate de que en AssetManager.h esta función se declara como 'void Render() override;'
}

void AssetManager::Update(float deltaTime) {
    // 1. Procesar cargas asíncronas pendientes
    //{
    //    std::lock_guard<std::mutex> lock(m_pendingLoadsMutex); // Proteger la cola de pendientes
    //    std::queue<std::shared_ptr<std::future<std::shared_ptr<IAsset>>>> tempPendingLoads;
    //    while (!m_pendingLoads.empty()) {
    //        std::shared_ptr<std::future<std::shared_ptr<IAsset>>> currentFuture = m_pendingLoads.front();
    //        m_pendingLoads.pop();

    //        if (currentFuture->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
    //            // El asset está listo para ser obtenido
    //            try {
    //                std::shared_ptr<IAsset> loadedAsset = currentFuture->get(); // Esto no bloquea si ya está ready
    //                if (loadedAsset) {
    //                    std::lock_guard<std::mutex> assetLock(m_assetsMutex); // Proteger el mapa de assets cargados
    //                    m_loadedAssets[loadedAsset->GetName()] = loadedAsset;
    //                    OutputDebugStringA(("Asset '" + loadedAsset->GetName() + "' loaded and added to cache.\n").c_str());
    //                }
    //                else {
    //                    OutputDebugStringA("WARNING: An asset failed to load asynchronously (returned nullptr).\n");
    //                }
    //            }
    //            catch (const std::exception& e) {
    //                OutputDebugStringA(("ERROR: Exception during async asset load: " + std::string(e.what()) + "\n").c_str());
    //            }
    //        }
    //        else {
    //            tempPendingLoads.push(currentFuture); // Todavía no listo, volver a poner en la cola
    //        }
    //    }
    //    m_pendingLoads = std::move(tempPendingLoads); // Restaurar la cola con los pendientes
    //}

    // 2. Procesar comandos de creación de recursos GPU
    //{
    //    std::lock_guard<std::mutex> lock(m_gpuCommandsMutex); // Proteger la cola de comandos GPU
    //    while (!m_gpuCreateCommands.empty()) {
    //        std::unique_ptr<IGPUCreateCommand> command = std::move(m_gpuCreateCommands.front());
    //        m_gpuCreateCommands.pop();

    //        if (command) {
    //            HRESULT hr = command->Execute(m_loadContext.pDevice, m_loadContext.pImmediateContext);
    //            if (FAILED(hr)) {
    //                OutputDebugStringA("ERROR: Failed to execute GPU creation command.\n");
    //            }
    //        }
    //    }
    //}
    // Asegúrate de que en AssetManager.h esta función se declara como 'void Update(float deltaTime) override;'
}

void AssetManager::Shutdown() {
    // Esperar a que todas las cargas pendientes finalicen
    //{
    //    std::lock_guard<std::mutex> lock(m_pendingLoadsMutex);
    //    while (!m_pendingLoads.empty()) {
    //        std::shared_ptr<std::future<std::shared_ptr<IAsset>>> currentFuture = m_pendingLoads.front();
    //        m_pendingLoads.pop();
    //        if (currentFuture->valid()) {
    //            currentFuture->get(); // Bloquear hasta que cada futuro esté completo
    //        }
    //    }
    //}
    // Procesar cualquier comando GPU restante
    //Update(0.0f); // Llamar Update para vaciar la cola de comandos GPU

    //UnloadAllAssets(); // Liberar todos los assets de la caché
    //OutputDebugStringA("AssetManager shutdown.\n");
}

//ID3D11ShaderResourceView* AssetManager::LoadTexture(std::shared_ptr<ID3D11Device> device, const std::string& filePath) {
//    ID3D11ShaderResourceView* fontTextureView = nullptr;
//    HRESULT hr = TextureLoader::LoadTextureFromFile(device,
//		filePath.c_str(),
//        &fontTextureView);
//    if (FAILED(hr)) {
//        // Manejar el error (por ejemplo, mostrar un mensaje y salir)
//        OutputDebugStringA("Error al cargar la textura de la fuente.\n");
//        return nullptr;
//    }
//	return fontTextureView;
//}

ID3D11ShaderResourceView* AssetManager::LoadTexture(std::string assetName) {
    std::shared_ptr<DeviceManager> deviceManager = ManagerLocator::GetDeviceManager();
    if (!deviceManager) {
        OutputDebugStringA("ERROR: DeviceManager not found.\n");
        return nullptr;
	}
    //auto asset = AssetLocator::GetAssetBase(assetName);

    //std::shared_ptr<TextureAsset> textureAsset = std::dynamic_pointer_cast<TextureAsset>(asset);

    //if (textureAsset) { // Si el cast fue exitoso (no es nullptr)
    //    std::shared_ptr<ITextureConfig> config = ConfigLocator::GetConfig<ITextureConfig>(assetName + "Config");
    //    std::string path = config->file_path;
    //}
    return nullptr;
}


//void AssetManager::UnloadAsset(const std::string& assetId) {
//    std::lock_guard<std::mutex> lock(m_assetsMutex);
//    auto it = m_loadedAssets.find(assetId);
//    if (it != m_loadedAssets.end()) {
//        it->second->Shutdown(); // Llama al Release() polimórfico
//        m_loadedAssets.erase(it);
//        OutputDebugStringA(("Asset '" + assetId + "' unloaded.\n").c_str());
//    }
//    else {
//        OutputDebugStringA(("WARNING: Attempted to unload non-existent asset: '" + assetId + "'.\n").c_str());
//    }
//}
//
//void AssetManager::UnloadAllAssets() {
//    std::lock_guard<std::mutex> lock(m_assetsMutex);
//    for (const auto& pair : m_loadedAssets) {
//        pair.second->Shutdown();
//    }
//    m_loadedAssets.clear();
//    OutputDebugStringA("All assets unloaded.\n");
//}
//
//bool AssetManager::IsAssetLoaded(const std::string& assetId) const {
//    std::lock_guard<std::mutex> lock(m_assetsMutex);
//    return m_loadedAssets.count(assetId) > 0;
//}
//
//void AssetManager::AddGPUCreateCommand(std::unique_ptr<IGPUCreateCommand> command) {
//    std::lock_guard<std::mutex> lock(m_gpuCommandsMutex);
//    m_gpuCreateCommands.push(std::move(command));
//}
//
//void AssetManager::RegisterDefaultAssetLoaders() {
//    // Registrar la función de carga para Texturas
//    m_assetLoaders[AssetType::Texture] = [this](const YAML::Node& def, const AssetLoadContext& ctx) {
//        return LoadTextureFromDefinition(def, ctx);
//        };
//    // Registrar la función de carga para Mallas
//    m_assetLoaders[AssetType::Mesh] = [this](const YAML::Node& def, const AssetLoadContext& ctx) {
//        return LoadMeshFromDefinition(def, ctx);
//        };
//    // Registrar la función de carga para Skybox (que crearemos después)
//    m_assetLoaders[AssetType::Skybox] = [this](const YAML::Node& def, const AssetLoadContext& ctx) {
//        return LoadSkyboxFromDefinition(def, ctx);
//        };
//    // ... registra aquí otros tipos de cargadores
//    OutputDebugStringA("Default asset loaders registered.\n");
//}