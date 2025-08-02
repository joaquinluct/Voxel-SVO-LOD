#pragma once
#include <IPipelineState.h>

class IPipelineStencilState : public IPipelineState
{
private:
	bool active = false;
public:
	int MipLevels;
	int ArraySize;
	int Format;
	int SampleCount;
	int SampleQuality;
	int Usage;
	int BindFlags;
	float Depth;
	float Stencil;

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