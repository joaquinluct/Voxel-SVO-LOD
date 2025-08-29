#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <string>
#include <vector>
#include <memory>

class TextureLoader {
public:
    // Carga una textura desde un archivo y crea una ShaderResourceView.
    static HRESULT LoadTextureFromFile(
        Microsoft::WRL::ComPtr<ID3D11Device> device,
        const std::string& filename,
        D3D11_TEXTURE2D_DESC& textureDesc,
        ID3D11ShaderResourceView** textureView
    );
    static HRESULT LoadTextureArrayFromFiles(
        Microsoft::WRL::ComPtr<ID3D11Device> device,
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext,
        const std::vector<std::string>& filenames,
        D3D11_TEXTURE2D_DESC textureDesc,
        ID3D11ShaderResourceView** textureArrayView
    );
    static HRESULT LoadCubemapFromFile(
        Microsoft::WRL::ComPtr<ID3D11Device> device,
        const std::string& filename,
        ID3D11ShaderResourceView** textureView
    );

    // (Opcional) Función para liberar la ShaderResourceView.
    static void ReleaseTextureView(ID3D11ShaderResourceView*& textureView);

private:
    // Funciones auxiliares internas (si es necesario).
};
