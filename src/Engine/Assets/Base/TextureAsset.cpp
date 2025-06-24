// Texture.cpp
#include "TextureAsset.h"
#include <DirectX/DirectXTex/DirectXTex.h>
#include <DirectX/WICTextureLoader/WICTextureLoader11.h> // De DirectX Tool Kit o librería similar
#include <string>
#include <windows.h> // Para OutputDebugString
#include <REGISTER_ASSET_MACRO.h>
#include <Util/Utils.h> // Para SafeRelease

REGISTER_ASSET_TYPE(TextureAsset, "SkyboxAsset")

TextureAsset::TextureAsset() {
    // Los recursos D3D se inicializan en InitD3D11Resources
}

TextureAsset::~TextureAsset() {
    Shutdown();
}

void TextureAsset::Shutdown() {
    // ComPtr maneja la liberación de recursos automáticamente en su destructor.
    // Solo necesitamos resetearlos explícitamente si queremos liberarlos antes
    // o para indicar que ya no poseen el recurso.
    SafeRelease(m_shaderResourceView);
    SafeRelease(m_texture2D);
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
        &m_texture2D,
        &m_shaderResourceView);
    if (FAILED(hr)) {
        OutputDebugStringA(("ERROR: Failed to load texture from file '" + filePath + "'. HRESULT: " + std::to_string(hr) + "\n").c_str());
    }
    else {
        //OutputDebugStringA(("Texture '" + filePath + "' loaded successfully.\n").c_str());
    }
    return hr;
}