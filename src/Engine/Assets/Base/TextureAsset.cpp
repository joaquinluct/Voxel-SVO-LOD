// Texture.cpp
#include "TextureAsset.h"
#include <DirectX/DirectXTex/DirectXTex.h>
#include <DirectX/WICTextureLoader/WICTextureLoader11.h> // De DirectX Tool Kit o librería similar
#include <string>
#include <windows.h> // Para OutputDebugString
#include <REGISTER_ASSET_MACRO.h>
#include <Util/Utils.h> // Para SafeRelease
#include <AssetManager.h>
#include <TextureLoader/TextureLoader.h>
#include <Defines/Texture.h>

REGISTER_ASSET_TYPE(TextureAsset, "TextureAsset")

TextureAsset::TextureAsset(): m_textureConfig(nullptr) {
    // Los recursos D3D se inicializan en InitD3D11Resources
}

TextureAsset::~TextureAsset() {
    Shutdown();
}

void TextureAsset::Shutdown() {
    // ComPtr maneja la liberación de recursos automáticamente en su destructor.
    // Solo necesitamos resetearlos explícitamente si queremos liberarlos antes
    // o para indicar que ya no poseen el recurso.
    if (m_shaderResourceView) m_shaderResourceView->Release();
    SafeRelease(m_textureAlbedo2D);
    SafeRelease(m_textureRoughtness2D);
    SafeRelease(m_textureRoughtness2D);
    //OutputDebugStringA(("Texture '" + m_name + "' released.\n").c_str());
}

HRESULT TextureAsset::InitD3D11Resources(ID3D11Device* pDevice, const std::string& filePath) {
    if (!pDevice) {
        OutputDebugStringA("ERROR: ID3D11Device is null when initializing texture.\n");
        return E_INVALIDARG;
    }

    // Usar la función de carga de WICTextureLoader (parte de DirectX Tool Kit)
    // Carga la textura desde el archivo y crea la vista de recurso de shader.
    HRESULT hr = DirectX::CreateWICTextureFromFile(pDevice,
        std::wstring(filePath.begin(), filePath.end()).c_str(), // Convertir a wstring
        &m_textureAlbedo2D,
        &m_shaderResourceView);
    if (FAILED(hr)) {
        OutputDebugStringA(("ERROR: Failed to load texture from file '" + filePath + "'. HRESULT: " + std::to_string(hr) + "\n").c_str());		
    }

    return hr;
}

void TextureAsset::SetTexture(Material* material, std::string textureType, std::string textureMap) {
    ID3D11ShaderResourceView* view = GetTextureView(textureType, textureMap);
    if (view) {
        material->SetTexture(view, textureMap);
    }
}

void TextureAsset::SetTextureView(Material* material) {
    std::string textureType = m_textureConfig->mapConfig->texture_type;
    std::string textureMap = "";
    ID3D11ShaderResourceView* view = nullptr;
    if (m_textureConfig->mapConfig->albedo.size()) {
        textureMap = TEXTURE_MAP_ALBEDO.data();
        SetTexture(material, textureType, textureMap);
    }
    if (m_textureConfig->mapConfig->normal.size()) {
        textureMap = TEXTURE_MAP_NORMAL.data();
        SetTexture(material, textureType, textureMap);
    }
    if (m_textureConfig->mapConfig->roughness.size()) {
        textureMap = TEXTURE_MAP_ROUGHNESS.data();
        SetTexture(material, textureType, textureMap);
    }
    if (m_textureConfig->mapConfig->metallic.size()) {
        textureMap = TEXTURE_MAP_METALLIC.data();
        SetTexture(material, textureType, textureMap);
    }
    if (m_textureConfig->mapConfig->ao.size()) {
        textureMap = TEXTURE_MAP_AO.data();
        SetTexture(material, textureType, textureMap);
    }    
}

ID3D11ShaderResourceView* TextureAsset::GetTextureView(std::string textureType, std::string mapType) {
    if (textureType == TEXTURE_TYPE_JPG) {
        return GetFileTextureView(mapType);
    }
    else if (textureType == TEXTURE_TYPE_CUBEMAP) {
        return GetCubemapTextureView();
    }
	return nullptr;
}

std::string TextureAsset::GetFilePathByMapType(std::string mapType) {
    if (mapType == TEXTURE_MAP_ALBEDO.data()) return m_textureConfig->mapConfig->albedo;
    if (mapType == TEXTURE_MAP_NORMAL.data()) return m_textureConfig->mapConfig->normal;
    if (mapType == TEXTURE_MAP_ROUGHNESS.data()) return m_textureConfig->mapConfig->roughness;
    if (mapType == TEXTURE_MAP_METALLIC.data()) return m_textureConfig->mapConfig->metallic;
    if (mapType == TEXTURE_MAP_AO.data()) return m_textureConfig->mapConfig->ao;
    return "";
}

ID3D11ShaderResourceView* TextureAsset::GetFileTextureView(std::string mapType) {
    std::string fileMap = GetFilePathByMapType(mapType);
    if (fileMap.empty()) {
        return nullptr;
    }
    ID3D11ShaderResourceView* textureView;
	std::shared_ptr<DeviceManager> deviceManager = ManagerLocator::GetDeviceManager();
    HRESULT hr = TextureLoader::LoadTextureFromFile(deviceManager->GetDevice(),
        fileMap,
    	&textureView);
    if (FAILED(hr)) {
    	OutputDebugStringA("Error al cargar la textura de la fuente.\n");
        return nullptr;
    }
    return textureView;
}

ID3D11ShaderResourceView* TextureAsset::GetCubemapTextureView() const {
    if (m_textureConfig == nullptr) {
        return nullptr;
    }
    ID3D11ShaderResourceView* textureView;
    std::shared_ptr<DeviceManager> deviceManager = ManagerLocator::GetDeviceManager();
    HRESULT hr = TextureLoader::LoadCubemapFromFile(deviceManager->GetDevice(),
        //"Resources/DejaVuSansMono.jpg", // Ruta a tu imagen
        m_textureConfig->mapConfig->albedo,
        &textureView);
    if (FAILED(hr)) {
        // Manejar el error (por ejemplo, mostrar un mensaje y salir)
        OutputDebugStringA("Error al cargar la textura de la fuente.\n");
        return nullptr;
    }
    return textureView;
}