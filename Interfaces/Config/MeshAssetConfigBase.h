#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <vector>
#include <ConfigBase.h>

class MeshAssetConfigBase : public ConfigBase
{
public:
	std::string shader;
	std::string mesh_path;
	std::string texture;
	std::vector<float> texture_transforms;
	std::string vertexDef;
	std::string name;
	bool cast_shadows = false;
	bool receive_shadows = false;
	std::vector<float> position = {0.0f, 0.0f, 0.0f};
	DirectX::XMFLOAT4 GetTextureTransform()
	{
		if (texture_transforms.size() < 4) {
			texture_transforms.resize(4, 0.0f); // Ensure we have at least 4 elements
			texture_transforms = { 1.0f, 1.0f, 0.0f, 0.0f }; // Default transform
		}
		return DirectX::XMFLOAT4(texture_transforms[0], texture_transforms[1], texture_transforms[2], texture_transforms[3]);
	}
};
