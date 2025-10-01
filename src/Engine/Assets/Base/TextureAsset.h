// Texture.h
#pragma once
#include <Assets/Base/AssetBase.h>
#include <ConfigBase.h>
#include <d3d11.h>
#include <ITextureConfig.h>
#include <ManagerLocator/ManagerLocator.h> // Asegúrate de que este archivo exista y contenga la definición de IConfig
#include <memory> // Para std::shared_ptr
#include <Services/Material.h>
#include <string>
#include <wrl/client.h> // Para Microsoft::WRL::ComPtr

// Define un tipo de alias para ComPtr para mayor comodidad
template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

class TextureAsset : public AssetBase {
public:
    TextureAsset(); // Constructor simple, la carga es en el AssetManager
    ~TextureAsset() override;

    // IAsset overrides
    virtual std::shared_ptr<AssetBase> Clone() const override {
        // Crea una nueva instancia utilizando el constructor de copia
        // y la devuelve como un shared_ptr.
        return std::make_shared<TextureAsset>(*this);
    }
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

    void SetConfig(std::shared_ptr<ConfigBase> config) override {
        SetConfig(std::dynamic_pointer_cast<ITextureConfig>(config));
    }
    void SetConfig(std::shared_ptr<ITextureConfig> config) {
        m_textureConfig = config;

    };
    void SetTexture(Material* material, std::string textureType, std::string textureName, std::vector<std::string> textureMap);
    void SetTextureView(Material* material);
    ID3D11ShaderResourceView* GetTextureView(std::string textureType, std::string mapType, std::vector<std::string> textureMap = {});
    ID3D11ShaderResourceView* GetFileTextureView(std::string mapType, D3D11_TEXTURE2D_DESC textureDesc);
    ID3D11ShaderResourceView* GetFileTextureArrayView(std::vector<std::string> textureMap, D3D11_TEXTURE2D_DESC textureDesc);
    ID3D11ShaderResourceView* GetCubemapTextureView() const;

    D3D11_TEXTURE2D_DESC GetTextureDesc() const;

    std::string GetTextureType() const { return m_textureType; }

    // Métodos específicos de Texture
    ID3D11Resource* GetTexture2D() const { return m_textureAlbedo2D; }

    // Método para inicializar la textura con los recursos de D3D11
    HRESULT InitD3D11Resources(ID3D11Device* pDevice, const std::string& filePath);

    // Devolver la matriz de transformación de la textura
    XMFLOAT4 GetTextureTransform() const {
        float scaleX = m_textureConfig ? m_textureConfig->x_scale : 1.0f;
        float scaleY = m_textureConfig ? m_textureConfig->y_scale : 1.0f;
        float offsetX = m_textureConfig ? m_textureConfig->x_offset : 0.0f;
        float offsetY = m_textureConfig ? m_textureConfig->y_offset : 0.0f;
        return XMFLOAT4(scaleX, scaleY, offsetX, offsetY);
    }

    std::shared_ptr<ITextureConfig> m_textureConfig = nullptr; // Configuración de textura
private:
    std::string m_textureType;
    /*ComPtr<ID3D11Texture2D> m_texture2D;
    ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;*/

    ID3D11Resource* m_textureAlbedo2D = nullptr; // Puntero a la textura 2D de D3D11
    ID3D11Resource* m_textureNormal2D = nullptr; // Puntero a la textura 2D de D3D11
    ID3D11Resource* m_textureRoughtness2D = nullptr; // Puntero a la textura 2D de D3D11
    ID3D11Resource* m_textureAmbientOcclusion2D = nullptr; // Puntero a la textura 2D de D3D11
    ID3D11Resource* m_textureARM_Map2D = nullptr; // Puntero a la textura 2D de D3D11
    ID3D11ShaderResourceView* m_shaderResourceView = nullptr; // Vista de recurso de shader

    std::string GetFilePathByMapType(std::string mapType);

    // *******************************************
    // ****Evitar copias para recursos D3D11******
    /*TextureAsset(const TextureAsset&) = delete;
    TextureAsset& operator=(const TextureAsset&) = delete;*/
};
