#include "UITextMeshConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(UITextMeshConfig,"UITextMeshConfig");

UITextMeshConfig::UITextMeshConfig(){
	enabled = true;
	type = "MeshAsset";
	meshType = 2;
	textService = "UIText";
	name = "UIText";
	texture = "TextureTextUI";
	shader = "ShaderTextUI";
	position = {
		0.0f,
		0.0f,
		0.0f
	};
	cast_shadows = false;
	receive_shadows = false;
	drawType = 2;
	primitiveTopology = 4;
	render_passes = 8;
	dependencies = {
		"MeshAsset"
	};
};
