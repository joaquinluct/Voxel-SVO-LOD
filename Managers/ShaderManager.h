#pragma once
#include <d3d11.h>
#include <string>
#include <map>
#include "../Interfaces/iDirectXManager.h"
#include "DeviceManager.h"

class ShaderManager : public iDirectXManager
{
public:
	ShaderManager();
	~ShaderManager();
       
	HRESULT Init(DeviceManager* device);
    HRESULT Init() override;
    HRESULT Release() override;
    HRESULT Render() override { return S_OK; };

    HRESULT LoadShader(ID3D11Device* device, std::wstring shaderName, std::wstring vsPath, std::wstring psPath, D3D11_INPUT_ELEMENT_DESC layoutDesc[], UINT numElements);
    ID3D11VertexShader* GetVertexShader(std::wstring shaderName);
    ID3D11PixelShader* GetPixelShader(std::wstring shaderName);
    ID3D11InputLayout* GetInputLayout(std::wstring shaderName);
    ID3DBlob* GetVertexShaderBytecode(std::wstring shaderName); // Nuevo método
    UINT GetVertexShaderBytecodeLength(std::wstring shaderName);
    std::map<std::wstring, ID3D11VertexShader*> vertexShaders;
    std::map<std::wstring, ID3D11PixelShader*> pixelShaders;
    std::map<std::wstring, ID3D11InputLayout*> inputLayouts;
    std::map<std::wstring, ID3DBlob*> vertexShaderBlobs; // Nuevo mapa para los blobs VS
    std::map<std::wstring, ID3DBlob*> pixelShaderBlobs;  // Nuevo mapa para los blobs PS
};