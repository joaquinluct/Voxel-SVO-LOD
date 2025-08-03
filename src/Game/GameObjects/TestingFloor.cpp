#include <windows.h>
#include "TestingFloor.h"
#include <ManagerLocator/ManagerLocator.h>
#include <AssetLocator/AssetLocator.h>
#include <REGISTER_SERVICE_MACRO.h>

REGISTER_SERVICE_TYPE(TestingFloor, "TestingFloor");

TestingFloor::TestingFloor() :
	mesh(nullptr),
	houseMesh(nullptr)
{
}

TestingFloor::~TestingFloor()
{
}

HRESULT TestingFloor::Init()
{
	m_renderManager = ManagerLocator::GetManager<RenderManager>();
	if (!m_renderManager) {
		OutputDebugStringA("TestingFloor::Init - ERROR: RenderManager not found.\n");
		return E_FAIL;
	}

	mesh = m_renderManager->GameRenderManagerGet()->RegisterMesh("FloorMesh");

	// mesh = AssetLocator::GetAsset<MeshAsset>("FloorMesh");

	if (!mesh) {
		OutputDebugStringA("TestingFloor::Init - ERROR: Mesh init.\n");
	}
	HRESULT hr = mesh->Init();
	if (FAILED(hr)) {
		OutputDebugStringA("TestingFloor::Init - ERROR: Mesh init failed.\n");
		return hr;
	}

	houseMesh = m_renderManager->GameRenderManagerGet()->RegisterMesh(("House1Mesh"));

	////houseMesh = AssetLocator::GetAsset<MeshAsset>("House1Mesh");

	if (!houseMesh) {
		OutputDebugStringA("TestingFloor::Init - ERROR: House mesh init.\n");
	}
	houseMesh->Init();
	if (FAILED(hr)) {
		OutputDebugStringA("TestingFloor::Init - ERROR: House mesh init failed.\n");
		return hr;
	}

	houseMesh->SetScale(0.5f, 0.5f, 0.5f);

	return S_OK;
}

void TestingFloor::Render() {
	// Comentar esto para el nuevo sistema de renderizado
	/*if (mesh) {
		mesh->Render();
	}*/	
	/*if (houseMesh) {
		houseMesh->Render();
	}*/
}
