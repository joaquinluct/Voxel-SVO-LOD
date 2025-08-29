#pragma once

#include <string>
#include <vector>
#include <memory>
//#include <Services/Material.h>
//#include <Assets/Base/MeshAsset.h>

class Material; // Forward declaration to avoid circular dependency
class MeshAsset; // Forward declaration to avoid circular dependency

class AreaMesh {
public:
	std::string area; 
	std::vector<MeshAsset*> mesh;
	Material* material;
	Material* shadowMaterial;
};