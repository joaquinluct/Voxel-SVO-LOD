#pragma once
#include <Assets/Base/AssetBase.h>
#include <Assets/IAssetShaderConfig.h>
#include <ConfigBase.h>
#include <d3d11.h>
#include <d3dcommon.h>
#include <Defines/Matrix/MatrixDefinition.h>
#include <Defines/ShaderSampler.h>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <Windows.h>

class ShaderAsset : public AssetBase
{
public:
    ShaderAsset();
    std::unique_ptr<ShaderAsset> CloneShader() const {
        // Usa std::make_unique para crear una nueva instancia de VertexShader
        // y el constructor de copia para copiar el contenido.
        return std::make_unique<ShaderAsset>(*this);
    }
    virtual std::shared_ptr<AssetBase> Clone() const override {
        // Usa std::make_unique para crear una nueva instancia de VertexShader
        // y el constructor de copia para copiar el contenido.
        return std::make_unique<ShaderAsset>(*this);
    }
    ~ShaderAsset() override;
    void Load() override {};
    void Unload() override {};
    HRESULT Init() override;
    void Render() override {};
    void Update(float deltaTime) override {};
    void Shutdown() override;
    const std::string& GetAssetName() override {
        return m_name;
    }
    static const std::string& GetStaticAssetName()
    {
        static const std::string name = "ShaderAsset";
        return name;
    }

    void SetConfig(std::shared_ptr<ConfigBase> config) override;
    std::shared_ptr<IAssetShaderConfig> GetConfig() const;
    void SetName(const std::string& name);


    void SetConstantsBuffers(std::string name, std::shared_ptr<MatrixDefinition::AnyMatrixBuffer> constantBuffers) {
        constantsBuffers[name] = constantBuffers;
    }
    std::map<std::string, std::shared_ptr<MatrixDefinition::AnyMatrixBuffer>> GetConstantsBuffers() const {
        return constantsBuffers;
    }
    void SetSamplersDesc(std::string name, ShaderSampler::SamplerDefinition samplerDesc) {
        samplersDesc[name] = samplerDesc;
    }
    std::unordered_map<std::string, ShaderSampler::SamplerDefinition> GetSamplersDesc() const {
        return samplersDesc;
    }

    void SetVertexShader(ID3D11VertexShader* vertexShader) {
        vertexShaders = vertexShader;
    }
    ID3D11VertexShader* GetVertexShader() const {
        return vertexShaders;
    }
    void SetPixelShader(ID3D11PixelShader* pixelShader) {
        pixelShaders = pixelShader;
    }
    ID3D11PixelShader* GetPixelShader() const {
        return pixelShaders;
    }
    void SetInputLayout(ID3D11InputLayout* inputLayout) {
        inputLayouts = inputLayout;
    }
    ID3D11InputLayout* GetInputLayout() const {
        return inputLayouts;
    }
    void SetVertexShaderBlob(ID3DBlob* vertexShaderBlob) {
        vertexShaderBlobs = vertexShaderBlob;
    }
    ID3DBlob* GetVertexShaderBlob() const {
        return vertexShaderBlobs;
    }
    void SetPixelShaderBlob(ID3DBlob* pixelShaderBlob) {
        pixelShaderBlobs = pixelShaderBlob;
    }
    ID3DBlob* GetPixelShaderBlob() const {
        return pixelShaderBlobs;
    }
    // Hull / Domain shader support (optional)
    void SetHullShader(ID3D11HullShader* hullShader) { hullShaders = hullShader; }
    ID3D11HullShader* GetHullShader() const { return hullShaders; }
    void SetDomainShader(ID3D11DomainShader* domainShader) { domainShaders = domainShader; }
    ID3D11DomainShader* GetDomainShader() const { return domainShaders; }
    void SetHullShaderBlob(ID3DBlob* hullShaderBlob) { hullShaderBlobs = hullShaderBlob; }
    ID3DBlob* GetHullShaderBlob() const { return hullShaderBlobs; }
    void SetDomainShaderBlob(ID3DBlob* domainShaderBlob) { domainShaderBlobs = domainShaderBlob; }
    ID3DBlob* GetDomainShaderBlob() const { return domainShaderBlobs; }


private:
    std::shared_ptr<IAssetShaderConfig> m_shaderConfig = nullptr; // Configuraci�n del shader
    std::string m_name = "ShaderAsset";

    std::unordered_map<std::string, ShaderSampler::SamplerDefinition> samplersDesc;

    std::map<std::string, std::shared_ptr<MatrixDefinition::AnyMatrixBuffer>> constantsBuffers;

    ID3D11VertexShader* vertexShaders;
    ID3D11PixelShader* pixelShaders;

    ID3D11InputLayout* inputLayouts;

    ID3DBlob* vertexShaderBlobs; // Nuevo mapa para los blobs VS
    ID3DBlob* pixelShaderBlobs;  // Nuevo mapa para los blobs PS
    // Optional hull/domain shaders and blobs (may be null)
    ID3D11HullShader* hullShaders = nullptr;
    ID3D11DomainShader* domainShaders = nullptr;
    ID3DBlob* hullShaderBlobs = nullptr;
    ID3DBlob* domainShaderBlobs = nullptr;
};
