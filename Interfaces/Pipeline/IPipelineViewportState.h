#pragma once
#include <IPipelineState.h>

class IPipelineViewportState : public IPipelineState	
{
private:
	bool active = false;
public:
	float TopLeftX;
	float TopLeftY;
	float MinDepth;
	float MaxDepth;
	float Width;
	float Height;

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
