#pragma once
#include <d3d11.h>
#include <string>
#include <memory>
#include <map>
#include <wrl/client.h>
#include "IManager.h"
#include "DeviceManager.h"
#include "IInitializable.h"
#include "IUpdatable.h"
#include "IRenderable.h"
#include "IShutdownable.h"
#include <Util/Text/Text.h>
#include <Defines/MatrixDefinition.h>

class ShaderManager : public IManager, public IInitializable, public IUpdatable, public IRenderable, public IShutdownable
{
public:
	ShaderManager();
	~ShaderManager();
	
    HRESULT Init() override;
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

    HRESULT LoadShader(Microsoft::WRL::ComPtr<ID3D11Device> device, std::wstring shaderName, std::wstring vsPath, std::wstring psPath, D3D11_INPUT_ELEMENT_DESC layoutDesc[], UINT numElements);

    void SetConstantsBuffers(std::wstring shaderName, const MatrixDefinitionBase::MatrixParams& matrixParams, std::map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>>&  constantBuffers, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

    /*MatrixDefinition::AnyMatrixBuffer* GetMatrixBuffer(std::wstring shaderName);*/
    std::map<std::string, std::pair<int, std::unique_ptr<MatrixDefinition::AnyMatrixBuffer>>>& GetMatrixBuffers(std::wstring shaderName);
    Microsoft::WRL::ComPtr<ID3D11VertexShader> GetVertexShader(std::wstring shaderName);
    Microsoft::WRL::ComPtr<ID3D11PixelShader> GetPixelShader(std::wstring shaderName);
    ID3D11InputLayout* GetInputLayout(std::wstring shaderName);
    ID3DBlob* GetVertexShaderBytecode(std::wstring shaderName); // Nuevo método
    UINT GetVertexShaderBytecodeLength(std::wstring shaderName);
    std::map<std::wstring, std::map<std::string, std::pair<int, std::unique_ptr<MatrixDefinition::AnyMatrixBuffer>>>> matrixShaders;
    std::map<std::wstring, ID3D11VertexShader*> vertexShaders;
    std::map<std::wstring, ID3D11PixelShader*> pixelShaders;
    std::map<std::wstring, ID3D11InputLayout*> inputLayouts;
    std::map<std::wstring, ID3DBlob*> vertexShaderBlobs; // Nuevo mapa para los blobs VS
    std::map<std::wstring, ID3DBlob*> pixelShaderBlobs;  // Nuevo mapa para los blobs PS
private:
	std::shared_ptr<DeviceManager> deviceManager;
};