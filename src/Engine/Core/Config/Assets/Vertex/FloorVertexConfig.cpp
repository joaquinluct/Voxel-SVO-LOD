#include "FloorVertexConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(FloorVertexConfig,"FloorVertexConfig");

FloorVertexConfig::FloorVertexConfig(){
	enabled = true;
	type = "VertexAsset";
	name = "FloorVertexDef";
	dependencies = {
		"VertexAsset"
	};
};
