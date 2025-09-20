#include <windows.h>
#include "TestingFloor.h"
#include <ManagerLocator/ManagerLocator.h>
#include <AssetLocator/AssetLocator.h>
#include <ServiceLocator/ServiceLocator.h>
#include <REGISTER_SERVICE_MACRO.h>

REGISTER_SERVICE_TYPE(TestingFloor, "TestingFloor");

TestingFloor::TestingFloor() :
	mesh(nullptr),
	houseMesh(nullptr),
	boxMesh(nullptr),
	m_light(nullptr)
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
	m_light = ServiceLocator::GetService<Lighting>();

	/*mesh = m_renderManager->SceneManagerGet()->RegisterMesh("FloorMesh");

	if (!mesh) {
		OutputDebugStringA("TestingFloor::Init - ERROR: Mesh init.\n");
	}
	HRESULT hr = mesh->Init();
	if (FAILED(hr)) {
		OutputDebugStringA("TestingFloor::Init - ERROR: Mesh init failed.\n");
		return hr;
	}*/

	/*houseMesh = m_renderManager->SceneManagerGet()->RegisterMesh(("House1Mesh"));

	if (!houseMesh) {
		OutputDebugStringA("TestingFloor::Init - ERROR: House mesh init.\n");
	}
	houseMesh->Init();
	if (FAILED(hr)) {
		OutputDebugStringA("TestingFloor::Init - ERROR: House mesh init failed.\n");
		return hr;
	}*/

	//houseMesh->SetScale(0.5f, 0.5f, 0.5f);

	/*boxMesh = m_renderManager->SceneManagerGet()->RegisterMesh("BoxMesh");
	if (boxMesh == nullptr) {
		OutputDebugStringA("TestingFloor::Init - ERROR: Box mesh init.\n");
	}
	hr = boxMesh->Init();
	if (FAILED(hr)) {
		OutputDebugStringA("TestingFloor::Init - ERROR: Box mesh init failed.\n");
		return hr;
	}*/

	//boxMesh->SetPosition(20.0f, 0.0f, 20.0f);

	/*text = m_renderManager->SceneManagerGet()->RegisterTextMeshAsUnique("UITextMesh", "AlturaSol");
	text->SetText("HOLA");
	hr = text->Init();*/
	


	return S_OK;
}

void TestingFloor::Update(float) {
	/*float y = m_light->GetLightDirection().y;
	text->SetPosition(10, 20);
	text->SetText("ALTURA SOL " + std::to_string(y));*/
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
