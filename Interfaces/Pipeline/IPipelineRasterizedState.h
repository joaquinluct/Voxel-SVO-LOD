#pragma once
#include <IPipelineState.h>

class IPipelineRasterizedState : public IPipelineState
{
private:
	bool active = false;
public:
	int FillMode;
	int CullMode;
	bool FrontCounterClockwise;
	int DepthBias;
	float DepthBiasClamp;
	float SlopeScaledDepthBias;
	bool AntialiasedLineEnable;
	bool DepthClipEnable;
	bool ScissorEnable;
	bool MultisampleEnable;

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
