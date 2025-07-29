#pragma once
#include <IPipelineState.h>

class IPipelineBlendingState: public IPipelineState
{
private:
	bool active = false;
public:
	~IPipelineBlendingState() {};
	bool AlphaToCoverageEnable;
	bool IndependentBlendEnable;
	bool BlendEnable;
	int SrcBlend;
	int DestBlend;
	int BlendOp;
	int SrcBlendAlpha;
	int DestBlendAlpha;
	int BlendOpAlpha;
	int RenderTargetWriteMask;

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