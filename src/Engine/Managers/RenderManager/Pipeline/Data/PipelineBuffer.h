#pragma once
#include <d3d11.h>
#include <string>
#include <string_view>
#include <map>
#include <vector>
#include <variant>
#include <functional>
#include <memory>
#include <wrl/client.h>

class PipelineBuffer
{
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
public:
	Microsoft::WRL::ComPtr<ID3D11Buffer> Get() { return buffer; }
	void Set(Microsoft::WRL::ComPtr<ID3D11Buffer> buf) { buffer = buf; }

};