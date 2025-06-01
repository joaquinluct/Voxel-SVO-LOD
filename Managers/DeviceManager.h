#pragma once
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include "../Interfaces/iDirectXManager.h"

class DeviceManager : public iDirectXManager
{
public:
	DeviceManager();
	~DeviceManager() override;

	HRESULT Init(HWND hwnd);
	HRESULT Init() override;
	HRESULT	InitBlending();
	HRESULT	InitRasterizedState();
	HRESULT Release() override;
	HRESULT	Render() override;
	
	HRESULT					GetBackBuffer(ID3D11Texture2D** ppBackBuffer);
	ID3D11Device*			GetDevice();
	ID3D11DeviceContext*	GetContext();
	IDXGISwapChain*			GetSwapChain();
	//HRESULT					SetAlphaBlending(float alpha);
	void					EnableAlphaBlending();
	void					DisableBlending();
	void					SetRasterizerState();
	void					ResetContextState();
private:
	ID3D11Device*			m_device = nullptr;
	ID3D11DeviceContext*	m_context = nullptr;
	IDXGISwapChain*			m_swapChain = nullptr;
	D3D_FEATURE_LEVEL       m_featureLevel = D3D_FEATURE_LEVEL_11_0;
	ID3D11BlendState*		m_alphaBlendState;    // Para blending con transparencia (UI, ...)
	ID3D11BlendState*		m_defaultBlendState;  // Para objetos opacos (sin blending)
	ID3D11RasterizerState*  m_rasterizerState;
};
