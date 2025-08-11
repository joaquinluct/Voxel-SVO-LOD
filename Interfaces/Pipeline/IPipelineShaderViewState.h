#pragma once
#include <string>
#include <IPipelineState.h>

class IPipelineShaderViewState : public IPipelineState
{
private:
	bool active = false;
public:	
	std::string Type;
	int Format;
	int ViewDimension;
	int MipLevels;
	int MostDetailedMip;

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