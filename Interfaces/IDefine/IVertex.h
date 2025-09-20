#pragma once

#include <DirectXMath.h>
#include <Windows.h>

#include <d3d11.h>
#include <IDefine.h>
#include <string>

class IVertex : public IDefine
{
public:
    virtual UINT GetByteWidth(size_t numVertex) { return 0; }
    virtual UINT Size() { return 0; }
    virtual const void* GetRawData() const noexcept { return nullptr; }
    virtual const DirectX::XMFLOAT3 GetPosition() const { return {}; }
    virtual const DirectX::XMFLOAT3 GetNormal() const { return {}; }
    virtual const DirectX::XMFLOAT4 GetDebugColor() {
        return DirectX::XMFLOAT4{};
    }
    virtual const void SetDebugColor(const DirectX::XMFLOAT4& debugColor) {}
    virtual void SetNormal(const DirectX::XMFLOAT3& normal) {};
    //virtual const void* GetRawData(const std::vector<std::shared_ptr<VertexDefinition::VertexVariant>> vertex) const noexcept = 0;
    virtual D3D11_INPUT_ELEMENT_DESC* GetInputLayout(unsigned int& numElements) { return nullptr; }
    virtual std::string ToOBJLine() const { return ""; }
    virtual void SetData(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 texCoord, DirectX::XMFLOAT3 normals, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 tangent, DirectX::XMFLOAT4 debugColor) {};
};
