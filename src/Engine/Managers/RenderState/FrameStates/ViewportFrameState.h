#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <map>
#include <memory>
#include <vector>
#include "FrameStateBase.h"

class ViewportFrameState : public FrameStateBase {
public:
	UINT GetNumViewports() const { return m_numViewports; }
	const std::map<std::string, D3D11_VIEWPORT> GetViewports() const { return m_viewports; }
	const D3D11_VIEWPORT* GetViewport(const std::string& name) const {
		auto it = m_viewports.find(name);
		if (it != m_viewports.end()) {
			return &it->second;
		}
		return nullptr; // Return nullptr if the viewport with the given name is not found
	}
	void SetViewports(const std::map<std::string, D3D11_VIEWPORT> viewports) {
		m_viewports = viewports;
	}
	void SetNumViewports(const UINT numViewports) { m_numViewports = numViewports; }    
private:
	std::map<std::string, D3D11_VIEWPORT> m_viewports;
	UINT m_numViewports = 0;
};

