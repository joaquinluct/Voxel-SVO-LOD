#pragma once
#define NOMINMAX 
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include "IManager.h"
#include "IWindowDependentInitializable.h"
#include "IUpdatable.h"
#include "IRenderable.h"
#include "IShutdownable.h"
#include <string>

class DeviceManager : public IManager, public IWindowDependentInitializable, public IUpdatable, public IRenderable, public IShutdownable
{
public:
	DeviceManager();
	~DeviceManager() override;

	HRESULT Init(HWND hwnd, int width, int height) override;
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

	HRESULT					CreateDeviceAndSwapChain(HWND hwnd, int width, int height);
	HRESULT					InitBlending();
	HRESULT					InitRasterizedState();
	
	HRESULT					GetBackBuffer(ID3D11Texture2D** ppBackBuffer);
	ID3D11Device*			GetDevice();
	ID3D11DeviceContext*	GetContext();
	IDXGISwapChain*			GetSwapChain();
	//HRESULT					SetAlphaBlending(float alpha);
	void					EnableAlphaBlending();
	void					DisableBlending();
	void					SetRasterizerState();
	void					ResetContextState();
	HWND*					GetHwnd() const { return m_hwnd; };
private:
	HWND*					m_hwnd = nullptr;
	ID3D11Device*			m_device = nullptr;
	ID3D11DeviceContext*	m_context = nullptr;
	IDXGISwapChain*			m_swapChain = nullptr;
	D3D_FEATURE_LEVEL       m_featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11BlendState*		m_alphaBlendState;    // Para blending con transparencia (UI, ...)
	ID3D11BlendState*		m_defaultBlendState;  // Para objetos opacos (sin blending)
	ID3D11RasterizerState*  m_rasterizerState;
};
