#include "TextureLoader.h"
#include <iostream> 
#include <fstream>
#include <vector>
#include <stdexcept> // Para manejar errores con excepciones si lo prefieres

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h" // Incluir stb_image (o DirectXTex)
#include <DirectX/DirectXTex/DirectXTex.h>

HRESULT TextureLoader::LoadTextureFromFile(
    Microsoft::WRL::ComPtr<ID3D11Device> device,
    const std::string& filename,
	D3D11_TEXTURE2D_DESC& textureDesc,
    ID3D11ShaderResourceView** textureView
) {
    HRESULT hr = S_OK;

    int width, height, channels;
    unsigned char* image_data = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
    if (!image_data) {
        std::cerr << "Error loading texture: " << filename << std::endl;
        return E_FAIL;
    }

    textureDesc.Width = width;
    textureDesc.Height = height;

    //D3D11_TEXTURE2D_DESC textureDesc = {};
    //textureDesc.Width = width;
    //textureDesc.Height = height;
    //textureDesc.MipLevels = 0; // Generar mipmaps automáticamente
    //textureDesc.ArraySize = filenames.size();
    //textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    //textureDesc.SampleDesc.Count = 1;
    //textureDesc.SampleDesc.Quality = 0;
    //textureDesc.Usage = D3D11_USAGE_DEFAULT; // Permite generar mipmaps
    //textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    //textureDesc.CPUAccessFlags = 0;
    //textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    //D3D11_TEXTURE2D_DESC textureDesc = {};
    //textureDesc.Width = width;
    //textureDesc.Height = height;
    //textureDesc.MipLevels = 1;
    //textureDesc.ArraySize = 1;
    //textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Assuming RGBA
    //textureDesc.SampleDesc.Count = 1;
    //textureDesc.SampleDesc.Quality = 0;
    //textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
    //textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    //textureDesc.CPUAccessFlags = 0;
    //textureDesc.MiscFlags = 0;

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

HRESULT TextureLoader::LoadTextureArrayFromFiles(
    Microsoft::WRL::ComPtr<ID3D11Device> device,
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext,
    const std::vector<std::string>& filenames,
    D3D11_TEXTURE2D_DESC textureDesc,
    ID3D11ShaderResourceView** textureArrayView
) {
    // Definir fuera del bucle para que el valor persista
    int width = 0, height = 0, channels = 0;
    std::vector<unsigned char*> image_data_list;

    // 1. Cargar todas las imágenes y validar dimensiones
    for (const auto& filename : filenames) {
        int currentWidth, currentHeight, currentChannels;
        unsigned char* image_data = stbi_load(filename.c_str(), &currentWidth, &currentHeight, &currentChannels, STBI_rgb_alpha);

        if (!image_data) {
            // Error: No se pudo cargar la imagen.
            // Limpiar los datos ya cargados y salir.
            for (auto data : image_data_list) {
                stbi_image_free(data);
            }
            // Puedes imprimir un mensaje de error más específico aquí.
            return E_FAIL;
        }

        // Si es la primera imagen, establecer las dimensiones base.
        if (image_data_list.empty()) {
            width = currentWidth;
            height = currentHeight;
            channels = currentChannels;
        }
        else {
            // Asegurarse de que todas las texturas tengan las mismas dimensiones.
            if (currentWidth != width || currentHeight != height) {
                // Error: las dimensiones no coinciden.
                stbi_image_free(image_data);
                for (auto data : image_data_list) {
                    stbi_image_free(data);
                }
                return E_FAIL; // O un código de error más específico.
            }
        }

        image_data_list.push_back(image_data);
    }

    // Si no se cargó ninguna imagen, salir.
    if (image_data_list.empty()) {
        return E_FAIL;
    }

    // Ahora que todas las imágenes están cargadas y validadas,
    // puedes continuar con la creación de la textura.

    // 2. Crear las subrecursos para cada capa
	int arraySize = static_cast<int>(filenames.size());
    std::vector<D3D11_SUBRESOURCE_DATA> initialData(filenames.size());
	// Reservar nuevo espacio para las subrecursos

    for (size_t i = 0; i < filenames.size(); ++i) {
        initialData[i].pSysMem = image_data_list[i];
        // Asegúrate de que SysMemPitch no sea 0
        initialData[i].SysMemPitch = width * 4;
        initialData[i].SysMemSlicePitch = 0;
    }

    // 3. Configurar la descripción del Texture2DArray
    //D3D11_TEXTURE2D_DESC texDesc = {};
    //texDesc.Width = width;
    //texDesc.Height = height;
    //texDesc.MipLevels = 0; // Generar mipmaps automáticamente
    //texDesc.ArraySize = filenames.size();
    //texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    //texDesc.SampleDesc.Count = 1;
    //texDesc.SampleDesc.Quality = 0;
    //texDesc.Usage = D3D11_USAGE_DEFAULT;
    //texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    //texDesc.CPUAccessFlags = 0;
    //texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.MipLevels = 1; // para arrays de texturas, 0 para que DirectX lo calcule solo.
    texDesc.ArraySize = filenames.size();
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

    ID3D11Texture2D* textureArray = nullptr;
    HRESULT hr = device->CreateTexture2D(&texDesc, initialData.data(), &textureArray);
    if (FAILED(hr)) {
        for (auto data : image_data_list) {
            stbi_image_free(data);
        }
        return hr;
    }
    
    // 4. Crear el Shader Resource View
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texDesc.Format; // Usa la descripción de la textura
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MostDetailedMip = 0;
    srvDesc.Texture2DArray.MipLevels = -1;
    srvDesc.Texture2DArray.FirstArraySlice = 0;
    srvDesc.Texture2DArray.ArraySize = filenames.size();

    hr = device->CreateShaderResourceView(textureArray, &srvDesc, textureArrayView);
    if (FAILED(hr)) {
        textureArray->Release();
        for (auto data : image_data_list) {
            stbi_image_free(data);
        }
        return hr;
    }

    // 5. Generar los mipmaps
    deviceContext->GenerateMips(*textureArrayView);

    // 6. Limpieza final de la memoria de la CPU y del Texture2D
    for (auto image_data : image_data_list) {
        stbi_image_free(image_data);
    }
    textureArray->Release();

    return S_OK;
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