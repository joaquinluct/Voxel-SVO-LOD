#pragma once
#include <string>
#include <IPipelineState.h>

class IPipelineStencilView: public IPipelineState
{
private:
	bool active = false;
public:
	std::string Type;
	int MipLevels;
	int ArraySize;
	int Format;
	int SampleCount;
	int SampleQuality;
	int Usage;
	int BindFlags;
	float Width;
	float Height;
	float Depth;
	float Stencil;
	bool ViewDesc;
	int ViewFormat;
	int ViewDimension;
	int ViewMipSlice;
	bool StencilEnable;
	bool DepthEnable;
	int DepthWriteMask;
	std::string ShaderView;

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