#pragma once
#include <d3d11.h>
#include <string>
#include <memory>

class TextureLoader {
public:
    // Carga una textura desde un archivo y crea una ShaderResourceView.
    static HRESULT LoadTextureFromFile(
        std::shared_ptr<ID3D11Device> device,
        const std::string& filename,
        ID3D11ShaderResourceView** textureView
    );
    static HRESULT LoadCubemapFromFile(
        std::shared_ptr<ID3D11Device> device,
        const std::string& filename,
        ID3D11ShaderResourceView** textureView
    );

    // (Opcional) Función para liberar la ShaderResourceView.
    static void ReleaseTextureView(ID3D11ShaderResourceView*& textureView);

private:
    // Funciones auxiliares internas (si es necesario).
};
