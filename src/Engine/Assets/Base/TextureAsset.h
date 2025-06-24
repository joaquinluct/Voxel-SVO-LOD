// Texture.h
#pragma once
#include "Assets/IAsset.h"
#include <d3d11.h>
#include <string>
#include <wrl/client.h> // Para Microsoft::WRL::ComPtr

// Define un tipo de alias para ComPtr para mayor comodidad
template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

class TextureAsset : public IAsset {
public:
    TextureAsset(); // Constructor simple, la carga es en el AssetManager
    ~TextureAsset() override;

	// IAsset overrides
	void Load() override {};
	void Unload() override {};
    HRESULT Init() override { return S_OK; };
    void Render() override {};
    void Update(float deltaTime) override {};
    void Shutdown() override;
    const std::string& GetAssetName() override {
        static const std::string name = "TextureAsset";
        return name;
    }
    static const std::string& GetStaticAssetName()
    {
        static const std::string name = "TextureAsset";
        return name;
    }

    // Métodos específicos de Texture
    ID3D11Resource* GetTexture2D() const { return m_texture2D; }
    
    // Método para inicializar la textura con los recursos de D3D11
    HRESULT InitD3D11Resources(ID3D11Device* pDevice, const std::string& filePath);

private:
    /*ComPtr<ID3D11Texture2D> m_texture2D;
    ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;*/

    ID3D11Resource* m_texture2D = nullptr; // Puntero a la textura 2D de D3D11
	ID3D11ShaderResourceView* m_shaderResourceView = nullptr; // Vista de recurso de shader

    // *******************************************
    // ****Evitar copias para recursos D3D11******
    /*TextureAsset(const TextureAsset&) = delete;
    TextureAsset& operator=(const TextureAsset&) = delete;*/
};