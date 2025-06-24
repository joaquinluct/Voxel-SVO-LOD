// AssetLoadContext.h
#pragma once
#include <d3d11.h> // Para ID3D11Device, ID3D11DeviceContext
#include <memory> // Para shared_ptr

// Forward declarations para los managers que podrían ser necesarios
class DeviceManager;
class SkyboxManager;
// ... otros managers

struct AssetLoadContext {
    // Referencias a managers o interfaces de bajo nivel necesarias para crear assets
    // Es mejor pasar directamente los punteros de la API gráfica para evitar dependencias circulares completas
    ID3D11Device* pDevice = nullptr;
    ID3D11DeviceContext* pImmediateContext = nullptr;

    // Si tuvieras un SkyboxManager, podrías pasarlo para cargar shaders dentro de modelos
    std::shared_ptr<DeviceManager> deviceManager = nullptr;
    std::shared_ptr<SkyboxManager> shaderManager = nullptr;

	AssetLoadContext() = default; // Constructor por defecto

    // Constructor para facilitar la inicialización
    AssetLoadContext(ID3D11Device* dev, ID3D11DeviceContext* ctx,
        std::shared_ptr<DeviceManager> devMgr = nullptr,
        std::shared_ptr<SkyboxManager> shdMgr = nullptr)
        : pDevice(dev), pImmediateContext(ctx), deviceManager(devMgr), shaderManager(shdMgr) {
    }

    // Otros datos de contexto que podrían ser necesarios para la carga de assets
    // Por ejemplo, rutas base, configuraciones globales, etc.
};