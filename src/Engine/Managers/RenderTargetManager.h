#pragma once
#define NOMINMAX 
#include <d3d11.h>
#include <windows.h>
#include <memory>
#include "IManager.h"
#include <InitManager.h>
#include "IWindowDependentInitializable.h"
#include "IRenderable.h"
#include "IUpdatable.h"
#include "IShutdownable.h"
#include "Utils.h"
#include "DeviceManager.h"

using namespace DirectX;

class RenderTargetManager : public IManager, public IWindowDependentInitializable, public IRenderable, public IUpdatable, public IShutdownable {
public:
    RenderTargetManager();
    ~RenderTargetManager();
        
    HRESULT Init(HWND hwnd, int width, int height) override;
    void Shutdown() override;
    void Render() override;
    void Update(float deltaTime) override {};
    const std::string& GetManagerName() const override { 
        static const std::string name = "RenderTargetManager";
        return name;
    }
    static const std::string& GetStaticManagerName()
    {
        static const std::string name = "RenderTargetManager";
        return name;
    }

    void Clear(const FLOAT clearColor[4]);
    void SetRenderTargets();
    void SetViewport();

    D3D11_VIEWPORT GetViewport() const { return m_viewport; }

    // --- Nuevas funciones para el efecto de limpieza gradual ---
    void EnableClearEffect(bool enable);
    void SetClearEffectSpeed(float speed);
    void ResetClearEffect();
    void GetViewPortDimensions(UINT& width, UINT& height) const {
        width = m_width;
        height = m_height;
	}

    void SetClearColor(const FLOAT clearColor[4]) {
        for (int i = 0; i < 4; ++i) {
            this->clearColor[i] = clearColor[i];
        }
	}

    ID3D11DepthStencilView* GetDepthStencilView() {
        return m_pDepthStencilView;
    
    }

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRenderTargetView() {
        return m_pRenderTargetView ? m_pRenderTargetView : nullptr;
    }

private:
	std::shared_ptr<InitManager> m_initManager;

    //FLOAT clearColor[4] = { .7f, 0.7f, 0.7f, 1.0f };
    FLOAT clearColor[4] = { .9f, 0.0f, 0.0f, 0.8f };
    std::shared_ptr<DeviceManager>  m_deviceManager;
    //Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;
    ID3D11RenderTargetView* m_pRenderTargetView;
    ID3D11DepthStencilView* m_pDepthStencilView;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext;
    UINT m_width;
    UINT m_height;
    D3D11_VIEWPORT m_viewport;

    // --- Miembros para el efecto de limpieza gradual ---
    bool m_enableClearEffect;
    float m_clearEffectFactor;
    float m_clearEffectSpeed;
};