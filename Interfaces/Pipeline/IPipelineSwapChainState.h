#pragma once
#include <string>
#include <IPipelineState.h>

class IPipelineSwapChainState : public IPipelineState
{
private:
	bool active = false;
public:
	int BufferCount;
    float Width;
	float Height;
    int Format;
    int Numerator;
    int Denominator;
    std::string BufferUsage;
    int SampleCount;
    int SampleQuality;
    bool Windowed;
    int SwapEffect;

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

