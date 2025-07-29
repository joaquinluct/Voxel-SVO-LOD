// Texture.h
#pragma once
#include <DeviceManager.h> // Asegúrate de que este archivo exista y contenga la definición de ITextureConfig
#include <ManagerLocator/ManagerLocator.h> // Asegúrate de que este archivo exista y contenga la definición de IConfig
#include <Assets/Base/AssetBase.h>
#include <d3d11.h>
#include <ConfigBase.h>
#include <string>
#include <wrl/client.h> // Para Microsoft::WRL::ComPtr
#include <memory> // Para std::shared_ptr
#include <RenderTargetManager.h> 

// Define un tipo de alias para ComPtr para mayor comodidad
template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

class MaterialAsset : public AssetBase {
public:
    MaterialAsset(); // Constructor simple, la carga es en el AssetManager
    ~MaterialAsset() override;

    // IAsset overrides
    virtual std::shared_ptr<AssetBase> Clone() const override {
        // Crea una nueva instancia utilizando el constructor de copia
        // y la devuelve como un shared_ptr.
        return std::make_shared<MaterialAsset>(*this);
    }
    void Load() override {};
    void Unload() override {};
    HRESULT Init() override { return S_OK; };
    void Render() override {};
    void Update(float deltaTime) override {};
    void Shutdown() override;
    const std::string& GetAssetName() override {
        static const std::string name = "MaterialAsset";
        return name;
    }
    static const std::string& GetStaticAssetName()
    {
        static const std::string name = "MaterialAsset";
        return name;
    }

    void SetConfig(std::shared_ptr<ConfigBase> config) override {
        SetConfig(std::dynamic_pointer_cast<ITextureConfig>(config));
    }
    void SetConfig(std::shared_ptr<ITextureConfig> config) {
        m_textureConfig = config;

    };

    ID3D11ShaderResourceView* GetTextureView() const;
    ID3D11ShaderResourceView* GetCubemapTextureView() const;

    // Métodos específicos de Texture
    ID3D11Resource* GetTexture2D() const { return m_texture2D; }

    // Método para inicializar la textura con los recursos de D3D11
    HRESULT InitD3D11Resources(ID3D11Device* pDevice, const std::string& filePath);

    std::shared_ptr<ITextureConfig> m_textureConfig = nullptr; // Configuración de textura
private:
    /*ComPtr<ID3D11Texture2D> m_texture2D;
    ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;*/

    ID3D11Resource* m_texture2D = nullptr; // Puntero a la textura 2D de D3D11
    ID3D11ShaderResourceView* m_shaderResourceView = nullptr; // Vista de recurso de shader

    // *******************************************
    // ****Evitar copias para recursos D3D11******
    /*MaterialAsset(const MaterialAsset&) = delete;
    MaterialAsset& operator=(const MaterialAsset&) = delete;*/
};
