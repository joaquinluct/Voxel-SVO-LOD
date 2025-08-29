#pragma once

#include <Windows.h>
#include <DirectXMath.h>

#include <IDefine.h>

class IVertex : public IDefine
{
public:
	virtual UINT GetByteWidth(size_t numVertex) = 0;
    virtual UINT Size() = 0;
	virtual const void* GetRawData() const noexcept = 0;
	virtual const DirectX::XMFLOAT3 GetPosition() const = 0;
    virtual const DirectX::XMFLOAT3 GetNormal() const = 0;
    virtual const DirectX::XMFLOAT4 GetDebugColor() {
        return DirectX::XMFLOAT4{};
    }
    virtual const void SetDebugColor(const DirectX::XMFLOAT4& debugColor) {}
    virtual void SetNormal(const DirectX::XMFLOAT3& normal) = 0;
    //virtual const void* GetRawData(const std::vector<std::shared_ptr<VertexDefinition::VertexVariant>> vertex) const noexcept = 0;
    virtual D3D11_INPUT_ELEMENT_DESC* GetInputLayout(unsigned int& numElements) = 0;
    virtual std::string ToOBJLine() const = 0;
    virtual void SetData(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 texCoord, DirectX::XMFLOAT3 normals, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 tangent, DirectX::XMFLOAT4 debugColor) = 0;
};
