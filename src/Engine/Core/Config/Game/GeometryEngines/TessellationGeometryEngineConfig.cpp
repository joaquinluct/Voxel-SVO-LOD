#include "TessellationGeometryEngineConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(TessellationGeometryEngineConfig,"TessellationGeometryEngineConfig");

TessellationGeometryEngineConfig::TessellationGeometryEngineConfig(){
	name = "TessellationGeometryEngine";
	enabled = true;
	version = "1.0.0";
	terrain_asset = "ProceduralTerrain";
	shader_name = "TerrainTessellation";
	description = "Motor de geometría que utiliza tessellation en GPU (HS/DS).";
	dependencies = {
		"DeviceManager",
		"ShaderManager",
		"TerrainAsset"
	};
};
