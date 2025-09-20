#pragma once

#include <d3d11.h>
#include <map>
#include <string>
#include <wrl/client.h>

using SamplerStates = std::map<std::string, Microsoft::WRL::ComPtr<ID3D11SamplerState>>;
