#include "TextureBricks001Config.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TextureBricks001Config,"TextureBricks001Config");

TextureBricks001Config::TextureBricks001Config(){
	type = "TextureAsset";
	map = "TextureBricks001Map";
	x_scale = 1.0f;
	y_scale = 1.0f;
	x_offset = 0.0f;
	y_offset = 0.0f;
	dependencies = {
		"DeviceManager",
		"TextureAsset"
	};
};
