#pragma once
#include <d3d11.h>
#include <windows.h>
#include "../Interfaces/iDirectXManager.h"
#include "../Util/Utils.h"
#include "../Managers/DeviceManager.h"

using namespace DirectX;

class RenderTargetManager : public iDirectXManager {
public:
    RenderTargetManager();
    ~RenderTargetManager() override;

    HRESULT Init(DeviceManager* deviceManager);
    HRESULT Init() override;
    HRESULT Release() override;
    HRESULT Render() override;
    void Clear(const FLOAT clearColor[4]);
    void SetRenderTargets();
    void SetViewport();

    D3D11_VIEWPORT GetViewport() const { return m_viewport; }

    // --- Nuevas funciones para el efecto de limpieza gradual ---
    void EnableClearEffect(bool enable);
    void SetClearEffectSpeed(float speed);
    void ResetClearEffect();

private:
    ID3D11RenderTargetView* m_pRenderTargetView;
    ID3D11DepthStencilView* m_pDepthStencilView;
    ID3D11DeviceContext* m_pContext;
    UINT m_width;
    UINT m_height;
    D3D11_VIEWPORT m_viewport;

    // --- Miembros para el efecto de limpieza gradual ---
    bool m_enableClearEffect;
    float m_clearEffectFactor;
    float m_clearEffectSpeed;
};