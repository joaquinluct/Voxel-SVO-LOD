// Texture.cpp
#include "TextureAsset.h"
#include <DirectX/DirectXTex/DirectXTex.h>
#include <DirectX/WICTextureLoader/WICTextureLoader11.h> // De DirectX Tool Kit o librería similar
#include <string>
#include <windows.h> // Para OutputDebugString
#include <memory> // Para std::shared_ptr
#include <REGISTER_ASSET_MACRO.h>
#include <Util/Utils.h> // Para SafeRelease
#include <AssetManager.h>
#include <Locators/Pipeline/PipelineStateLocator.h>
#include <Pipeline/IPipelineTexture2DDesc.h>
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

void TextureAsset::SetTexture(Material* material, std::string textureType, std::string textureName, std::vector<std::string> textureMap) {
    ID3D11ShaderResourceView* view = GetTextureView(textureType, textureName, textureMap);        
    if (textureType == TEXTURE_TYPE_JPG_ARRAY) {
        textureName = "";
	} 
    material->SetTexture(view, textureName);    
}

void TextureAsset::SetTextureView(Material* material) {
    m_textureType = m_textureConfig->mapConfig->texture_type;
    std::string textureMap = "";
    ID3D11ShaderResourceView* view = nullptr;
    if (m_textureType == TEXTURE_TYPE_JPG_ARRAY) {
        std::vector<std::string> map = m_textureConfig->mapConfig->texture2DArray;
        SetTexture(material, m_textureType, "", map);
        return;
    }
    if (m_textureConfig->mapConfig->albedo.size()) {
        textureMap = TEXTURE_MAP_ALBEDO.data();
        SetTexture(material, m_textureType, textureMap, {});
    }
    if (m_textureConfig->mapConfig->normal.size()) {
        textureMap = TEXTURE_MAP_NORMAL.data();
        SetTexture(material, m_textureType, textureMap, {});
    }
    if (m_textureConfig->mapConfig->roughness.size()) {
        textureMap = TEXTURE_MAP_ROUGHNESS.data();
        SetTexture(material, m_textureType, textureMap, {});
    }
    if (m_textureConfig->mapConfig->metallic.size()) {
        textureMap = TEXTURE_MAP_METALLIC.data();
        SetTexture(material, m_textureType, textureMap, {});
    }
    if (m_textureConfig->mapConfig->ao.size()) {
        textureMap = TEXTURE_MAP_AO.data();
        SetTexture(material, m_textureType, textureMap, {});
    }    
}
D3D11_TEXTURE2D_DESC TextureAsset::GetTextureDesc() const {

    std::string textureDescName = m_textureConfig->texture_desc;

    std::shared_ptr<IPipelineTexture2DDesc> configDesc = PipelineStateLocator::GetPipelineState<IPipelineTexture2DDesc>(textureDescName);

    D3D11_TEXTURE2D_DESC desc = {};
    desc.MipLevels = configDesc->MipLevels;
    desc.ArraySize = configDesc->ArraySize;
    desc.Format = static_cast<DXGI_FORMAT>(configDesc->Format);
    desc.SampleDesc.Count = configDesc->SampleDescCount;
    desc.SampleDesc.Quality = configDesc->SampleDescQuality;
    desc.Usage = static_cast<D3D11_USAGE>(configDesc->Usage);
    desc.BindFlags = configDesc->BindFlags;
    desc.CPUAccessFlags = configDesc->CPUAccessFlags;
    desc.MiscFlags = configDesc->MiscFlags;

    return desc;
}

ID3D11ShaderResourceView* TextureAsset::GetTextureView(std::string textureType, std::string mapType, std::vector<std::string> textureMap) {

    if (textureType == TEXTURE_TYPE_JPG) {
        D3D11_TEXTURE2D_DESC textureDesc = GetTextureDesc();
        return GetFileTextureView(mapType, textureDesc);
    }
    if (textureType == TEXTURE_TYPE_JPG_ARRAY) {
        D3D11_TEXTURE2D_DESC textureDesc = GetTextureDesc();
        return GetFileTextureArrayView(textureMap, textureDesc);
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

ID3D11ShaderResourceView* TextureAsset::GetFileTextureView(std::string mapType, D3D11_TEXTURE2D_DESC textureDesc) {

    std::string fileMap = GetFilePathByMapType(mapType);
    if (fileMap.empty()) {
        return nullptr;
    }
    ID3D11ShaderResourceView* textureView;
	std::shared_ptr<DeviceManager> deviceManager = ManagerLocator::GetDeviceManager();
    HRESULT hr = TextureLoader::LoadTextureFromFile(deviceManager->GetDevice(),
        fileMap,
        textureDesc,
    	&textureView);
    if (FAILED(hr)) {
        OutputDebugStringA(("[TextureAsset] Error al cargar la textura de: " + fileMap + ".\n").c_str());
        return nullptr;
    }
    return textureView;
}

ID3D11ShaderResourceView* TextureAsset::GetFileTextureArrayView(std::vector<std::string> textureMap, D3D11_TEXTURE2D_DESC textureDesc) {
    ID3D11ShaderResourceView* view;
    textureDesc.ArraySize = static_cast<UINT>(textureMap.size());
    std::shared_ptr<DeviceManager> deviceManager = ManagerLocator::GetDeviceManager();
    HRESULT hr = TextureLoader::LoadTextureArrayFromFiles(
        deviceManager->GetDevice(), 
        deviceManager->GetContext(),
        textureMap,
        textureDesc,
        &view);
    if (FAILED(hr)) {
        OutputDebugStringA(("[TextureAsset] Error al cargar array de texturas de: " + (textureMap.empty() ? "" : textureMap[0]) + ".\n").c_str());
        return {};
    }		
 
    return view;
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