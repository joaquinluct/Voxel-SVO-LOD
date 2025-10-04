#pragma once

#include <d3d11.h>
#include <Defines/Matrix/MatrixDefinition.h>
#include <Defines/ShaderSampler.h>
#include <Defines/Usings/Sampler.h>
#include <ManagerBase.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <wrl/client.h>

class DeviceManager;
class ShaderAsset;

class ShaderManager : public ManagerBase
{
public:
    ShaderManager();
    ~ShaderManager();

    HRESULT Init(EngineContext* context) override;
    HRESULT InitManagers();
    HRESULT InitShaders();
    void Shutdown() override;
    void Update(float deltaTime) override {};
    void Render() override {};
    const std::string& GetManagerName() const override {
        static const std::string name = "ShaderManager";
        return name;
    }
    static const std::string& GetStaticManagerName()
    {
        static const std::string name = "ShaderManager";
        return name;
    }
    std::shared_ptr<ShaderAsset> LoadShaderByName(std::wstring shaderName);
    HRESULT LoadShader(Microsoft::WRL::ComPtr<ID3D11Device> device, std::shared_ptr<ShaderAsset>& shader, std::wstring shaderName, std::wstring vsPath, std::wstring psPath, D3D11_INPUT_ELEMENT_DESC layoutDesc[], UINT numElements);

    /*void SetConstantsBuffers(std::wstring shaderName, MatrixDefinitionBase::MatrixParams& matrixParams, std::map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>>&  constantBuffers, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);*/

    void GetMatrixDefinitions(std::wstring shaderName, std::map<int, std::pair<std::string, MatrixDefinition::AnyMatrixBuffer>>& matrixDefs);

    std::shared_ptr<MatrixDefinition::AnyMatrixBuffer> GetConstantsBuffer(std::string matrixName);
    std::map<std::string, std::shared_ptr<MatrixDefinition::AnyMatrixBuffer>> GetConstantsBuffers();
    std::map<int, std::pair<std::string, std::shared_ptr<MatrixDefinition::AnyMatrixBuffer>>>& GetMatrixBuffers(std::wstring shaderName);
    Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader(std::wstring shaderName);
    Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader(std::wstring shaderName);
    ID3D11InputLayout* GetInputLayout(std::wstring shaderName);
    ID3DBlob* GetVertexShaderBytecode(std::wstring shaderName); // Nuevo método
    UINT GetVertexShaderBytecodeLength(std::wstring shaderName);

    bool NeedsShadow(std::wstring shaderName);

    std::vector<D3D11_SAMPLER_DESC> GetSamplersDescAsVector(std::wstring shaderName);
    std::vector<ShaderSampler::SamplerDefinition> GetAllSamplersDesc();
    std::vector<std::pair<int, D3D11_SAMPLER_DESC>> GetSamplersDescAsVector();
    //std::vector<std::string> GetSamplersNames(std::wstring shaderName);
    std::map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> GetSamplersStates(std::wstring shaderName, SamplerStates state);

    std::map<std::wstring, ShaderSampler::SamplerDefinition> samplers;
    std::map<std::wstring, std::vector<ShaderSampler::SamplerDefinition>> samplersDesc;
    std::map<std::wstring, std::map<int, std::pair<std::string, std::shared_ptr<MatrixDefinition::AnyMatrixBuffer>>>> matrixShaders;
    std::map<std::string, std::shared_ptr<MatrixDefinition::AnyMatrixBuffer>> constantsBuffers;
    std::map<std::wstring, ID3D11VertexShader*> vertexShaders;
    std::map<std::wstring, ID3D11PixelShader*> pixelShaders;
    std::map<std::wstring, ID3D11InputLayout*> inputLayouts;
    std::map<std::wstring, ID3DBlob*> vertexShaderBlobs; // Nuevo mapa para los blobs VS
    std::map<std::wstring, ID3DBlob*> pixelShaderBlobs;  // Nuevo mapa para los blobs PS
private:
    std::shared_ptr<DeviceManager> deviceManager;
};
