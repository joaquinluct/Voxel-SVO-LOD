#include "TextureBricks001Config.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TextureBricks001Config,"TextureBricks001Config");

TextureBricks001Config::TextureBricks001Config(){
	type = "TextureAsset";
	map = "TextureBricks001Map";
	dependencies = {
		"DeviceManager",
		"TextureAsset"
	};
};
