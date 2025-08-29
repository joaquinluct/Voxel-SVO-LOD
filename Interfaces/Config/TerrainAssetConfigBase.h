#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <ConfigBase.h>

class TerrainAssetConfigBase : public ConfigBase
{
public:
	int meshType = -1; // None
	int render_passes = 0; // Default render pass
	int drawType = 1; // DrawIndexed
	std::string shader;
	std::string mesh_path;
	std::string texture;
	std::vector<float> texture_transforms;
	std::string vertexDef;
	std::string name;
	bool cast_shadows = false;
	bool receive_shadows = false;
	std::string shader_shadows;
	std::vector<float> position = { 0.0f, 0.0f, 0.0f };
	std::string textService;
	DirectX::XMFLOAT4 GetTextureTransform()
	{
		if (texture_transforms.size() < 4) {
			texture_transforms.resize(4, 0.0f); // Ensure we have at least 4 elements
			texture_transforms = { 1.0f, 1.0f, 0.0f, 0.0f }; // Default transform
		}
		return DirectX::XMFLOAT4(texture_transforms[0], texture_transforms[1], texture_transforms[2], texture_transforms[3]);
	}
};

