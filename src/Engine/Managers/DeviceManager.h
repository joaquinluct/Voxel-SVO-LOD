#pragma once

#define NOMINMAX 

#include <Config/Base/RenderStates/SwapChainMain.h>
#include <d3d11.h>
#include <dxgi.h>
#include <ManagerBase.h>
#include <mutex>
#include <string>
#include <windows.h>
#include <wrl/client.h>

struct IUnknownReleaser {
    void operator()(IUnknown* ptr) const {
        if (ptr) {
            ptr->Release();
        }
    }
};

class DeviceManager : public ManagerBase
{
public:
    DeviceManager();
    ~DeviceManager() override;

    HRESULT Init(EngineContext* context) override;
    void Render() override;
    void Update(float deltaTime) override {};
    void Shutdown() override;
    const std::string& GetManagerName() const override {
        static const std::string name = "DeviceManager"; // Esta cadena se crea una sola vez y vive durante toda la ejecución del programa.
        return name;
    }
    static const std::string& GetStaticManagerName()
    {
        static const std::string name = "DeviceManager"; // Esta cadena se crea una sola vez y vive durante toda la ejecución del programa.
        return name;
    }

    void                    OnResize(int width, int height) { m_width = static_cast<float>(width); m_height = static_cast<float>(height); };

    HRESULT					CreateDeviceAndSwapChain(HWND* hwnd, int width, int height);
    /*HRESULT					InitBlending();
    HRESULT					InitRasterizedState();
    HRESULT					InitRasterizedShadowsState();*/

    //HRESULT					GetBackBuffer(ID3D11Texture2D** ppBackBuffer);
    Microsoft::WRL::ComPtr<ID3D11Device> GetDevice();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>	GetContext();
    //HRESULT					SetAlphaBlending(float alpha);
    //void					EnableAlphaBlending();
    //void					DisableBlending();
    /*void					SetRasterizerState();
    void					SetRasterizerShadowsState();*/
    void					ResetContextState();
    HWND* GetHwnd() const { return m_hwnd; };
    float					GetWidth() { return m_width; };
    float					GetHeight() { return m_height; };
    Microsoft::WRL::ComPtr<IDXGISwapChain> GetSwapChain() { return m_swapChain; };
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> GetRasterizedState() { return m_rasterizerState; };
    ID3D11RasterizerState* GetRasterizedShadowState() { return m_rasterizerShadowsState; };
private:
    std::mutex m_mutex;
    float					m_width = .0f;
    float					m_height = .0f;
    HWND* m_hwnd = nullptr;
    Microsoft::WRL::ComPtr<ID3D11Device> m_device = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context = nullptr;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain = nullptr;
    D3D_FEATURE_LEVEL       m_featureLevel = D3D_FEATURE_LEVEL_11_0;
    ID3D11BlendState* m_alphaBlendState;    // Para blending con transparencia (UI, ...)
    ID3D11BlendState* m_defaultBlendState;  // Para objetos opacos (sin blending)
    ID3D11RasterizerState* m_rasterizerState;
    ID3D11RasterizerState* m_rasterizerShadowsState;

    std::shared_ptr<SwapChainMain> m_swapChainMainConfig;

};
