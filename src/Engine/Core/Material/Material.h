#pragma once
#include <DirectXMath.h> // Para XMMATRIX
#include <d3d11.h>
#include <DirectX/DirectXTex/DirectXTex.h>
#include <wincodec.h> // Para Windows Imaging Component (WIC)
#include <iostream>
#include "IMaterial.h"
#include "ShaderManager.h"
#include "Utils.h"
#include "Texture.h"
#include "ITextureInitializer.h"

class Material : public IMaterial {
public:
    Material(std::wstring shaderName);
    ~Material();
    void SetTexture(ID3D11ShaderResourceView* texture);
    void Apply(ID3D11DeviceContext* context) override;
    HRESULT Init(ID3D11Device* device, TextureType textureType = TextureType::NONE); // Solo necesita device para crear buffers
    void Release();

    ID3D11InputLayout* GetInputLayout() {
        return inputLayout;
    }

    // Método para actualizar el contenido del buffer de matrices y vincularlo
    // Ahora toma las tres matrices por separado.
    void SetShaderParameters(ID3D11DeviceContext* context, const DirectX::XMMATRIX& worldMatrix, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix, int slot = 0);

    ID3D11VertexShader* GetVertexShader() const { return vertexShader; }
    ID3D11PixelShader* GetPixelShader() const { return pixelShader; }

    ID3D11ShaderResourceView* LoadTextureFromFile(ID3D11Device* device, const std::wstring& filename);

    PBRTextures textures;

private:
	std::wstring m_shaderName;
    ID3D11ShaderResourceView* m_texture;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* inputLayout;
    ID3D11SamplerState* m_samplerState;
    ID3D11Buffer* m_matrixBuffer; // Miembro para el buffer de matrices
    // Aquí guardamos la estrategia actual.
    // std::unique_ptr es ideal porque Material es el dueño de la estrategia.
    std::unique_ptr<ITextureInitializer> m_initializerStrategy;
};