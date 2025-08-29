#pragma once
#include <IPipelineState.h>

class IPipelineTexture2DDesc : public IPipelineState
{
private:
	bool active = false;
public:
	~IPipelineTexture2DDesc() {};
	int MipLevels;
	int ArraySize;
	int Format;
	int SampleDescCount;
	int SampleDescQuality;
	int Usage;
	int BindFlags;
	int CPUAccessFlags;
	int MiscFlags;

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