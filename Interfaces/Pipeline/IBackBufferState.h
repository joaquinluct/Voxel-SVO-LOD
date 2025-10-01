#pragma once
#include <d3d11.h>      // For D3D11_BIND_FLAG, D3D11_USAGE, etc.
#include <dxgiformat.h> // For DXGI_FORMAT
#include <IPipelineState.h>
#include <Windows.h>

class IBackBufferState : public IPipelineState
{
private:
    bool active = false;
public:
    float Width;
    float Height;
    UINT MipLevels;
    UINT ArraySize;
    int Format;               // Specify the type for Format
    UINT SampleDescCount;             // Specify the type for SampleDescCount
    UINT SampleDescQuality;           // Specify the type for SampleDescQuality
    int Usage;                // Specify the type for Usage
    UINT BindFlags;                   // Specify the type for BindFlags
    UINT CPUAccessFlags;              // Specify the type for CPUAccessFlags
    UINT MiscFlags;                   // Specify the type for MiscFlagsº    

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
