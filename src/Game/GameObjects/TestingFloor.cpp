#include <windows.h>
#include "TestingFloor.h"
#include <ManagerLocator/ManagerLocator.h>
#include <AssetLocator/AssetLocator.h>
#include <REGISTER_SERVICE_MACRO.h>

REGISTER_SERVICE_TYPE(TestingFloor, "TestingFloor");

TestingFloor::TestingFloor() :
	mesh(nullptr),
	houseMesh(nullptr),
	boxMesh(nullptr)
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

	if (!mesh) {
		OutputDebugStringA("TestingFloor::Init - ERROR: Mesh init.\n");
	}
	HRESULT hr = mesh->Init();
	if (FAILED(hr)) {
		OutputDebugStringA("TestingFloor::Init - ERROR: Mesh init failed.\n");
		return hr;
	}

	/*houseMesh = m_renderManager->GameRenderManagerGet()->RegisterMesh(("House1Mesh"));

	if (!houseMesh) {
		OutputDebugStringA("TestingFloor::Init - ERROR: House mesh init.\n");
	}
	houseMesh->Init();
	if (FAILED(hr)) {
		OutputDebugStringA("TestingFloor::Init - ERROR: House mesh init failed.\n");
		return hr;
	}*/

	//houseMesh->SetScale(0.5f, 0.5f, 0.5f);

	boxMesh = m_renderManager->GameRenderManagerGet()->RegisterMesh("BoxMesh");
	if (boxMesh == nullptr) {
		OutputDebugStringA("TestingFloor::Init - ERROR: Box mesh init.\n");
	}
	hr = boxMesh->Init();
	if (FAILED(hr)) {
		OutputDebugStringA("TestingFloor::Init - ERROR: Box mesh init failed.\n");
		return hr;
	}

	//boxMesh->SetPosition(20.0f, 0.0f, 20.0f);

	text = m_renderManager->GameRenderManagerGet()->RegisterTextMesh("UITextMesh");
	text->SetText("HOLA");
	hr = text->Init();
	


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
