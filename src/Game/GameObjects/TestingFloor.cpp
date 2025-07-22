#include "TestingFloor.h"
#include <AssetLocator/AssetLocator.h>
#include <REGISTER_SERVICE_MACRO.h>

REGISTER_SERVICE_TYPE(TestingFloor, "TestingFloor");

TestingFloor::TestingFloor() :
	mesh(nullptr)
{
}

TestingFloor::~TestingFloor()
{
}

HRESULT TestingFloor::Init()
{
	mesh = std::static_pointer_cast<MeshAsset>(AssetLocator::GetAsset("FloorMesh"));
	mesh->Init();

	return S_OK; // Return success
}

void TestingFloor::Render() {
	if (mesh) {
		mesh->Render();
	}	
}
