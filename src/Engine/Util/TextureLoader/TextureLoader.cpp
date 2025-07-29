#include "TextureLoader.h"
#include <iostream> 
#include <fstream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // Incluir stb_image (o DirectXTex)
#include <DirectX/DirectXTex/DirectXTex.h>

HRESULT TextureLoader::LoadTextureFromFile(
    Microsoft::WRL::ComPtr<ID3D11Device> device,
    const std::string& filename,
    ID3D11ShaderResourceView** textureView
) {
    HRESULT hr = S_OK;

    int width, height, channels;
    unsigned char* image_data = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (!image_data) {
        std::cerr << "Error loading texture: " << filename << std::endl;
        return E_FAIL;
    }

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Assuming RGBA
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = 0;
    textureDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA initialData = {};
    initialData.pSysMem = image_data;
    initialData.SysMemPitch = width * 4; // Assuming 4 bytes per pixel (RGBA)
    initialData.SysMemSlicePitch = 0;

    ID3D11Texture2D* texture = nullptr;
    hr = device->CreateTexture2D(&textureDesc, &initialData, &texture);
    if (FAILED(hr)) {
        stbi_image_free(image_data);
        return hr;
    }

    hr = device->CreateShaderResourceView(texture, nullptr, textureView);
    if (FAILED(hr)) {
        texture->Release();
        stbi_image_free(image_data);
        return hr;
    }

    texture->Release();
    stbi_image_free(image_data);

    return hr;
}


HRESULT TextureLoader::LoadCubemapFromFile(
    Microsoft::WRL::ComPtr<ID3D11Device> device,
    const std::string& filename,
    ID3D11ShaderResourceView** textureView
) {
    HRESULT hr = S_OK;

    // Convertir std::string a std::wstring para DirectXTex
    std::wstring wFilename(filename.begin(), filename.end());

    DirectX::TexMetadata metadata;
    DirectX::ScratchImage scratchImage;

    // Intentar cargar el archivo como DDS
    hr = DirectX::LoadFromDDSFile(wFilename.c_str(), DirectX::DDS_FLAGS_NONE, &metadata, scratchImage);

    // Si no es DDS, o quieres cargar otros formatos con mipmaps, puedes añadir esto:
    if (FAILED(hr)) {
        // Intentar cargar otros formatos de imagen (PNG, JPG, etc.)
        // Puedes usar LoadWICTextureFromFile, LoadDDSTextureFromFile, etc.
        // O si quieres mipmaps, usar LoadImage y GenerateMipmaps
        // Para este ejemplo, solo nos centramos en DDS para cubemaps.
        std::cerr << "Error loading DDS or other texture: " << filename << std::endl;
        return hr;
    }

    // Verificar si es un cubemap
    if (!metadata.IsCubemap())
    {
        std::cerr << "Loaded DDS is not a cubemap: " << filename << std::endl;
        return E_FAIL;
    }

    // Crear la vista de recurso de shader (SRV) a partir de la imagen cargada
    // Esto manejará automáticamente la creación de ID3D11Texture2D con MiscFlags=D3D11_RESOURCE_MISC_TEXTURECUBE
    hr = DirectX::CreateShaderResourceView(device.Get(), scratchImage.GetImages(), scratchImage.GetImageCount(), metadata, textureView);
    if (FAILED(hr)) {
        std::cerr << "Error creating shader resource view for " << filename << std::endl;
        return hr;
    }

    // scratchImage liberará sus recursos automáticamente cuando salga del scope.
    return hr;
}

void TextureLoader::ReleaseTextureView(ID3D11ShaderResourceView*& textureView) {
    if (textureView) {
        textureView->Release();
        textureView = nullptr;
    }
}