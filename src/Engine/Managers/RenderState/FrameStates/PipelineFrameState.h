#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <wrl/client.h>
#include "FrameStateBase.h"
#include <Defines/Pipeline.h>
#include <Defines/Matrix/MatrixDefinitionBase.h>
#include <RenderManager/Pipeline/PipelineState.h>
#include <Defines/Matrix/MatrixDefinition.h>

class FrameStateBase;

struct PipelineConstantBufferFrameState {
	bool updated = false;
	std::string matrixType; // "MATRIX_TYPE_VERTEX", "MATRIX_TYPE_PIXEL", "MATRIX_TYPE_MIXED"
	unsigned int slot; 
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};

class PipelineFrameState : public FrameStateBase {
public:

	// PIPELINE STATES RESOURCES CREATORS
	const void CreateBackBuffer() {
		if (m_device && m_swapChain) {
			//HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(m_backBuffer.GetAddressOf()));
			HRESULT hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&m_backBuffer));
			if (FAILED(hr)) {
				// Handle error
			}
		}
	}
	const void CreateRasterizerState(const std::string& name, const D3D11_RASTERIZER_DESC& desc) {
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
		if (m_device) {
			HRESULT hr = m_device->CreateRasterizerState(&desc, &rasterizerState);
			if (SUCCEEDED(hr)) {
				m_rasterizerStates[name] = std::make_pair(desc, rasterizerState);
			}
		}
	}
	const void CreateBlendState(const std::string& name, const D3D11_BLEND_DESC& desc) {
		ID3D11BlendState* state = nullptr;
		HRESULT hr = m_device->CreateBlendState(&desc, &state);
		if (SUCCEEDED(hr)) {
			m_blendingStates[name] = std::make_pair(desc, state);
		}
	}
	const void CreateDepthStencilState(const std::string& name, const D3D11_DEPTH_STENCIL_DESC& desc) {
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
		if (m_device) {
			HRESULT hr = m_device->CreateDepthStencilState(&desc, &depthStencilState);
			if (SUCCEEDED(hr)) {
				m_stencilStates[name] = std::make_pair(desc, depthStencilState);
			}
		}
	}
	const void CreateDepthStencilView(const std::string& name, PipelineDepthStencilData data) {
		if (m_device) {
			ID3D11Texture2D* depthStencilBuffer = nullptr;
			HRESULT hr = m_device->CreateTexture2D(&data.desc, nullptr, &depthStencilBuffer);
			if (SUCCEEDED(hr)) {
				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;								
				Microsoft::WRL::ComPtr<ID3D11DepthStencilView> stencilView;

				if (data.hasViewDesc) {
					D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc = data.viewDesc;
					hr = m_device->CreateDepthStencilView(depthStencilBuffer, &viewDesc, &stencilView);
				}
				else {
					hr = m_device->CreateDepthStencilView(depthStencilBuffer, nullptr, &stencilView);
				}

				if (SUCCEEDED(hr) && data.shaderViewDesc.Format != DXGI_FORMAT_UNKNOWN) {
					D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = data.shaderViewDesc;
					hr = m_device->CreateShaderResourceView(depthStencilBuffer, &srvDesc, &shaderResourceView);
				}
				if (FAILED(hr)) {
					return;
				}

				PipelineDepthStencilData* result = new PipelineDepthStencilData();

				result->viewTextureData = depthStencilBuffer;
				result->stencilViewData = std::move(stencilView);
				result->shaderViewData = std::move(shaderResourceView);
				result->desc = data.desc;
				result->viewDesc = data.viewDesc;
				result->shaderViewDesc = data.shaderViewDesc;
				result->hasViewDesc = data.hasViewDesc;

				m_stencilViews[name] = result;
			}
		}
	}
	const void CreateViewport(const std::string& name, const D3D11_VIEWPORT& desc) {
		//m_context->RSSetViewports(1, &desc);
		m_viewports[name] = desc;
	}
	const void CreateSamplerState(const std::string& name, const D3D11_SAMPLER_DESC& desc) {
		Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
		if (m_device) {
			HRESULT hr = m_device->CreateSamplerState(&desc, &samplerState);
			if (SUCCEEDED(hr)) {
				m_samplerStates[name] = samplerState;
			}
		}
	}
	const void SetSamplerState(const std::string& name, UINT slot) {
		auto it = m_samplerStates.find(name);
		if (it != m_samplerStates.end()) {
			ID3D11SamplerState* sampler = it->second.Get();
			m_context->PSSetSamplers(slot, 1, &sampler);
		}
	}
	const void SetTexture(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture, UINT slot) {
		m_context->PSSetShaderResources(slot, 1, texture.GetAddressOf());
	}
	const void CreateRenderTargetsView() {
		if (m_device && m_viewports.size()) {
			for (auto& vp : m_viewports) {
				Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;
				HRESULT hr = m_device->CreateRenderTargetView(m_backBuffer.Get(), nullptr, &rtv);
				if (SUCCEEDED(hr)) {
					m_renderTargetView[vp.first] = rtv;
				}
			}
		}
	}

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetCurrentRenderTargetView() const {
		if (m_viewports.empty()) return nullptr;
		std::string viewportName = m_pipelineState->GetViewport();
		if (viewportName.empty()) viewportName = m_pipelineState->GetMainViewport();
		if (viewportName.empty()) return nullptr;

		auto it = m_viewports.find(viewportName);
		if (it == m_viewports.end() && viewportName.empty()) {
			return nullptr;
		}
		auto rtvIt = m_renderTargetView.find(it->first);
		if (rtvIt != m_renderTargetView.end()) {
			return rtvIt->second;
		}
		return nullptr;
	}
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> GetCurrentDepthStencilView() const {
		if (m_stencilViews.empty()) return nullptr;
		std::string stencilName = m_pipelineState->GetStencilState();
		if (stencilName.empty()) stencilName = m_pipelineState->GetMainStencilView();
		if (stencilName.empty()) return nullptr;
		auto it = m_stencilViews.find(stencilName);
		if (it == m_stencilViews.end() && stencilName.empty()) {
			return nullptr;
		}
		auto dsvIt = m_stencilViews.find(it->first);
		if (dsvIt != m_stencilViews.end() && dsvIt->second->stencilViewData) {
			return dsvIt->second->stencilViewData;
		}
		return nullptr;
	}

	const void CreateConstantBuffer(const std::string& name, const std::string matrixType,UINT size, UINT slot) {
		if (m_device) {
			D3D11_BUFFER_DESC bd = {};
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = size;
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = 0;
			Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
			HRESULT hr = m_device->CreateBuffer(&bd, nullptr, &constantBuffer);
			if (SUCCEEDED(hr)) {

				PipelineConstantBufferFrameState bufferState{};
				bufferState.matrixType = matrixType;
				bufferState.slot = slot;
				bufferState.buffer = constantBuffer;

				m_constantBuffers[name] = bufferState;
			}
		}
	}

	const void SetConstantBuffer(const std::string& name) {
		auto it = m_constantBuffers.find(name);
		if (it != m_constantBuffers.end()) {
			//m_context->UpdateSubresource(it->second.Get(), 0, nullptr, data, 0, 0);
			PipelineConstantBufferFrameState state = it->second;
			if (state.matrixType == MATRIX_TYPE_MIXED.data() || state.matrixType == MATRIX_TYPE_VERTEX.data()) {
				m_context->VSSetConstantBuffers(state.slot, 1, &state.buffer);
			}
			if (state.matrixType == MATRIX_TYPE_MIXED.data() || state.matrixType == MATRIX_TYPE_PIXEL.data()) {
				m_context->PSSetConstantBuffers(state.slot, 1, &state.buffer);
			}			
		}
	}

	const void UpdateConstantBuffer(const std::string& name, const void* data, size_t dataSize) {
		// CON UPDATE SUBRESOURCE
		// ----------------------
		/*auto it = m_constantBuffers.find(name);
		if (it != m_constantBuffers.end() && !it->second.updated) {
			m_context->UpdateSubresource(it->second.buffer.Get(), 0, nullptr, data, dataSize, 0);
			it->second.updated = true;
		}*/
		// CON MAP UNMAP
		// -------------
		auto it = m_constantBuffers.find(name);

		// Verifica que el buffer existe.
		if (it == m_constantBuffers.end()) {
			return; // Retorna si no se encuentra el buffer.
		}

		D3D11_MAPPED_SUBRESOURCE mappedResource;

		// 1. Llama a Map para obtener un puntero a la memoria de la GPU.
		// D3D11_MAP_WRITE_DISCARD es clave para el rendimiento.
		HRESULT hr = m_context->Map(
			it->second.buffer.Get(), // El buffer que queremos mapear
			0,                        // Subrecurso (siempre 0 para buffers)
			D3D11_MAP_WRITE_DISCARD,  // Flag para un mapeo eficiente
			0,                        // Flags de async (siempre 0 para sincrónico)
			&mappedResource           // Puntero donde se guardará la información de mapeo
		);

		if (FAILED(hr)) {
			return; // Manejo de errores
		}

		// 2. Copia los datos de la CPU a la memoria mapeada de la GPU.
		memcpy(mappedResource.pData, data, dataSize);

		// 3. Llama a Unmap para liberar el puntero y permitir que la GPU acceda a los datos.
		m_context->Unmap(it->second.buffer.Get(), 0);
	}

	const void UpdateAllConstantBuffers(const std::map<std::string, std::pair<const void*, size_t>>& dataMap) {
		for (const auto& [name, dataPair] : dataMap) {
			auto it = m_constantBuffers.find(name);
			if (it != m_constantBuffers.end() && !it->second.updated) {
				m_context->UpdateSubresource(it->second.buffer.Get(), 0, nullptr, dataPair.first, 0, 0);
				it->second.updated = true;
			}
		}
	}

	const void ResetAllConstantBufferUpdateFlag() {
		for (auto& cb : m_constantBuffers) {
			cb.second.updated = false;
		}		
	}
			

	const void SetConstantsBuffers(const std::vector<std::string>& names) {
		if (names.empty()) return;
		for (size_t i = 0; i < names.size(); ++i) {
			SetConstantBuffer(names[i]);
		}
	}

	// PIPELINE STATES RESOURCES GETTERS
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> GetRasterizerState(const std::string& name) const {
		auto it = m_rasterizerStates.find(name);
		if (it != m_rasterizerStates.end()) {
			return it->second.second;
		}
		return nullptr;
	}
	Microsoft::WRL::ComPtr<ID3D11BlendState> GetBlendState(const std::string& name) const {
		auto it = m_blendingStates.find(name);
		if (it != m_blendingStates.end()) {
			return it->second.second;
		}
		return nullptr;
	}
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> GetDepthStencilState(const std::string& name) const {
		auto it = m_stencilStates.find(name);
		if (it != m_stencilStates.end()) {
			return it->second.second;
		}
		return nullptr;
	}
	PipelineDepthStencilData* GetDepthStencilView(const std::string& name) const {
		auto it = m_stencilViews.find(name);
		if (it != m_stencilViews.end()) {
			return it->second;
		}
		return nullptr;
	}
	D3D11_VIEWPORT GetViewport(const std::string& name) const {
		auto it = m_viewports.find(name);
		if (it != m_viewports.end()) {
			return it->second;
		}
		return D3D11_VIEWPORT{};
	}
	std::map<std::string, D3D11_VIEWPORT> GetAllViewports() const {
		return m_viewports;
	}
	Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSamplerState(const std::string& name) const {
		auto it = m_samplerStates.find(name);
		if (it != m_samplerStates.end()) {
			return it->second;
		}
		return nullptr;
	}

	// BASIC RESOURCES
	Microsoft::WRL::ComPtr<ID3D11Texture2D> GetBackBuffer() const { return m_backBuffer; }
	Microsoft::WRL::ComPtr<ID3D11Device> GetDevice() const { return m_device; }
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContext() const { return m_context; }
	Microsoft::WRL::ComPtr<IDXGISwapChain> GetSwapChain() { return m_swapChain; }
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRenderTargetView(const std::string& name) const {
		auto it = m_renderTargetView.find(name);
		if (it != m_renderTargetView.end()) {
			return it->second;
		}
		return nullptr;
	}
	void SetDevice(Microsoft::WRL::ComPtr<ID3D11Device> device) { m_device = device; }
	const void SetContext(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) { m_context = context; }
	void SetSwapChain(Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain) { m_swapChain = swapChain; }

private:	


	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_backBuffer;
	std::map < std::string, Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> m_renderTargetView;
	std::map<std::string, PipelineConstantBufferFrameState> m_constantBuffers;
	std::map<std::string, PipelineTextureData*> m_textures;
	
	std::map<std::string, std::pair<D3D11_RASTERIZER_DESC, Microsoft::WRL::ComPtr<ID3D11RasterizerState>>> m_rasterizerStates;
	std::map<std::string, std::pair<D3D11_BLEND_DESC, Microsoft::WRL::ComPtr<ID3D11BlendState>>> m_blendingStates;
	std::map<std::string, std::pair<D3D11_DEPTH_STENCIL_DESC, Microsoft::WRL::ComPtr<ID3D11DepthStencilState>>> m_stencilStates;
	std::map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>> m_samplerStates;
	std::map<std::string, PipelineDepthStencilData*> m_stencilViews;
	std::map<std::string, D3D11_VIEWPORT> m_viewports;
};





