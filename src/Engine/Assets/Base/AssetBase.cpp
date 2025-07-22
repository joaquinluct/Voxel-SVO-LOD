#include "AssetBase.h"

AssetBase::AssetBase() {
    // Constructor, puedes inicializar m_assetName aquí o en una clase derivada
    // Por ejemplo: m_assetName = "DefaultAsset";
}

AssetBase::~AssetBase() {
    // Destructor
}

void AssetBase::Load() {
    // Implementación base para Load.
    // Las clases derivadas pueden llamar a esta base y añadir su lógica.
}

void AssetBase::Unload() {
    // Implementación base para Unload.
}

const std::string& AssetBase::GetAssetName() {
    return m_assetName; // Devuelve el nombre del asset
}

// Implementaciones de los métodos de ILifeCycle
HRESULT AssetBase::Init() {
    // Implementación base para Init.
    return S_OK; // Asume éxito por defecto
}

void AssetBase::Update(float deltaTime) {
    // Implementación base para Update.
}

void AssetBase::Render() {
    // Implementación base para Render.
}

void AssetBase::Shutdown() {
    // Implementación base para Shutdown.
}