// UI.h
#pragma once
#include "../Interfaces/iRenderizable.h"
#include <vector>
#include <directxmath.h>
#include "UIElement.h"
#include "../Managers/DeviceManager.h"
#include "../Managers/RenderTargetManager.h"

using namespace DirectX;

class UI : public iRenderizable
{
public:
    UI();
    ~UI();

    HRESULT Init() override { return S_OK; };
    HRESULT Init(DeviceManager* deviceManager, RenderTargetManager* renderTargetManager);
    void Render() override {};
    void Render(DeviceManager* deviceManager, ShaderManager* m_shaderManager, WorldMatrixManager* worldMatrix, const XMMATRIX& orthoMatrix);
    void Release() override;

    void AddElement(UIElement* element);
    void RemoveElement(UIElement* element);
    std::vector<UIElement*> uiElements;

private:
    DeviceManager* m_deviceManager;
    RenderTargetManager* m_renderTargetManager;
	XMMATRIX m_orthoMatrix; // Matriz ortográfica para UI
};