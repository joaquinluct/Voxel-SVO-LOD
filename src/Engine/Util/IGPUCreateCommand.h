// IGPUCreateCommand.h
#pragma once
#include <d3d11.h>
#include <memory>

class IGPUCreateCommand {
public:
    virtual ~IGPUCreateCommand() = 0;
    virtual HRESULT Execute(ID3D11Device* pDevice, ID3D11DeviceContext* pImmediateContext) = 0;
};
