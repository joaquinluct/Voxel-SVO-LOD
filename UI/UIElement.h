// UIElement.h
#pragma once
#include <directxmath.h>
#include "../Interfaces/iRenderizable.h"
#include "../Managers/DeviceManager.h"
#include "../Managers/ShaderManager.h"
#include "../Managers/WorldMatrixManager.h"

using namespace DirectX;

class UIElement : public iRenderizable
{
public:
    UIElement();
    virtual ~UIElement() = default;

    XMFLOAT2 GetPosition() const { return m_position; }
    void SetPosition(const XMFLOAT2& position) { m_position = position; }

    XMFLOAT2 GetSize() const { return m_size; }
    void SetSize(const XMFLOAT2& size) { m_size = size; }
            
    //void Render() override = 0;
    virtual void Render(DeviceManager* device, ShaderManager* m_shaderManager, WorldMatrixManager* worldMatrix, const XMMATRIX& orthoMatrix) = 0;

protected:
    XMFLOAT2 m_position;
    XMFLOAT2 m_size;
};