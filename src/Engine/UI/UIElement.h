// UIElement.h
#pragma once
#include <directxmath.h>
#include <DeviceManager.h>
#include <ShaderManager.h>
#include <WorldMatrixManager.h>
#include <IService.h>

using namespace DirectX;

class UIElement

{
public:
    UIElement();
    virtual ~UIElement() = default; // El destructor virtual es una buena práctica para clases base

    virtual HRESULT Init() = 0;
    virtual void Render(const XMMATRIX& orthoMatrix) = 0;
    virtual void Release() = 0;

    XMFLOAT2 GetPosition() const { return m_position; }
    void SetPosition(const XMFLOAT2& position) { m_position = position; }

    XMFLOAT2 GetSize() const { return m_size; }
    void SetSize(const XMFLOAT2& size) { m_size = size; }


protected:
    XMFLOAT2 m_position;
    XMFLOAT2 m_size;

    std::shared_ptr<DeviceManager> m_device;
    std::shared_ptr<ShaderManager> m_shaderManager;
    std::shared_ptr<WorldMatrixManager> m_woldMatrix;
};