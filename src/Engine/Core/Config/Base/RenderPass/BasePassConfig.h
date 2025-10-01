#pragma once

#include <string>
#include <vector>
#include <map>
#include <PassConfigBase.h>

class BasePassConfig : public PassConfigBase {
public:
	BasePassConfig();
	static const std::string& GetStaticConfigName()
	{
		static const std::string className = "BasePassConfig";
		return className;
	}
	std::string SwapChain;
	std::string BackBuffer;
	std::string DepthBuffer;
	std::string DepthStencil;
	std::string RenderTarget;
	std::string StencilView;
	std::vector<std::string> passes;
};
