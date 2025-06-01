#pragma once
#include "..\Interfaces\iMaterial.h"
#include "..\Managers\ShaderManager.h"
#include "..\Util\Utils.h" // Aquí ya tienes MatrixBufferType y MarchingCubesVertex

#include <DirectXMath.h> // Para XMMATRIX

class Material : public iMaterial {
public:
    Material(ShaderManager* shaderManager, std::wstring shaderName);
    ~Material();
    void SetTexture(ID3D11ShaderResourceView* texture);
    void Apply(ID3D11DeviceContext* context) override;
    HRESULT Init(ID3D11Device* device); // Solo necesita device para crear buffers
    void Release();

    ID3D11InputLayout* GetInputLayout() {
        return inputLayout;
    }

    // Método para actualizar el contenido del buffer de matrices y vincularlo
    // Ahora toma las tres matrices por separado.
    void SetShaderParameters(ID3D11DeviceContext* context, const DirectX::XMMATRIX& worldMatrix, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix);

    ID3D11VertexShader* GetVertexShader() const { return vertexShader; }
    ID3D11PixelShader* GetPixelShader() const { return pixelShader; }

private:
    ID3D11ShaderResourceView* m_texture;
    ID3D11VertexShader* vertexShader;
    ID3D11PixelShader* pixelShader;
    ID3D11InputLayout* inputLayout;
    ID3D11SamplerState* m_samplerState;
    ID3D11Buffer* m_matrixBuffer; // Miembro para el buffer de matrices
};