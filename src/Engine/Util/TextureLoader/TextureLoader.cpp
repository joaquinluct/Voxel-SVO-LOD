#include "TextureLoader.h"
#include <iostream> 
#include <fstream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // Incluir stb_image (o DirectXTex)

HRESULT TextureLoader::LoadTextureFromFile(
    ID3D11Device* device,
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

void TextureLoader::ReleaseTextureView(ID3D11ShaderResourceView*& textureView) {
    if (textureView) {
        textureView->Release();
        textureView = nullptr;
    }
}