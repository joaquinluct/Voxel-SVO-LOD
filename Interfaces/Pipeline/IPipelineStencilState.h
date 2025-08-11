#pragma once
#include <string>
#include <IPipelineState.h>

class IPipelineStencilState : public IPipelineState
{
private:
	bool active = false;
public:
	int DepthEnable = 0;
	int DepthWriteMask = 0;
	int DepthFunc = 0;
	int StencilEnable = 0;
	int StencilReadMask = 0;
	int StencilWriteMask = 0;

	void Activate() override {
		active = true;
	};
	void Deactivate() override {
		active = false;
	}
	bool IsActive() const override {
		return active;
	}
};