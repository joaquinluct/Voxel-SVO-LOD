#include "ProcSkyboxMeshConfig.h"
#include <REGISTER_CONFIG_MACRO.h>

REGISTER_CONFIG_TYPE(ProcSkyboxMeshConfig,"ProcSkyboxMeshConfig");

ProcSkyboxMeshConfig::ProcSkyboxMeshConfig(){
	enabled = true;
	type = "MeshAsset";
	meshType = 1;
	name = "ProcSkyboxMesh";
	mesh_path = "c:\\Users\\joaqu\\source\\repos\\DirectX-Voxelado\\Assets\\Mesh\\skybox1.obj";
	texture = "DefaultSkybox";
	shader = "ProceduralSkyboxShader";
	cast_shadows = false;
	receive_shadows = false;
	drawType = 1;
	primitiveTopology = 4;
	render_passes = 16;
	dependencies = {
		"MeshAsset"
	};
};
