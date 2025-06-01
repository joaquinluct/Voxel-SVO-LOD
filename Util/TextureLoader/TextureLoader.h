#pragma once
#include <d3d11.h>
#include <string>

class TextureLoader {
public:
    // Carga una textura desde un archivo y crea una ShaderResourceView.
    static HRESULT LoadTextureFromFile(
        ID3D11Device* device,
        const std::string& filename,
        ID3D11ShaderResourceView** textureView
    );

    // (Opcional) Función para liberar la ShaderResourceView.
    static void ReleaseTextureView(ID3D11ShaderResourceView*& textureView);

private:
    // Funciones auxiliares internas (si es necesario).
};
