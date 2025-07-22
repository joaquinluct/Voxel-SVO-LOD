#include "MaterialConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(MaterialConfig,"MaterialConfig");

MaterialConfig::MaterialConfig(){
	enabled = true;
	dependencies = {
		"ShaderManager"
	};
};
