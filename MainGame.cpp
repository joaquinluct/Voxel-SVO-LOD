#include "MainWindow.h"
#include "MainGame.h"
#include "Resources/resource.h"

MainGame::MainGame() : g_device(nullptr), g_renderTarget(nullptr), g_camera(nullptr), g_shaderManager(nullptr), g_material(nullptr), g_triangulo(nullptr), g_matrixBuffer(nullptr), g_axis(nullptr), g_hwnd(nullptr), g_octreeDebug(nullptr) {
	g_device = new DeviceManager();
	g_renderTarget = new RenderTargetManager();
	g_camera = new Camera();
	g_shaderManager = new ShaderManager();
	g_suelo = nullptr;
	g_worldMatrixManager = new WorldMatrixManager();
	g_ui = new UI();
	g_axis = nullptr;
	//g_line = new Line(XMFLOAT3(-.1f, 0.0f, -.1f), XMFLOAT3(.1f, 0.0f, .1f));	
	g_world = new World(g_device, g_camera);

	//g_chunkManager = new ChunkManager();
	//g_chunkRenderer = new ChunkRenderer(g_chunkManager, nullptr);
}

MainGame::~MainGame() {
	Release();
}

HRESULT MainGame::InitUI() {
		
	// Cargar la textura de la fuente
	ID3D11ShaderResourceView* fontTextureView = nullptr;
	HRESULT hr = TextureLoader::LoadTextureFromFile(g_device->GetDevice(),
		"Resources/DejaVuSansMono.jpg", // Ruta a tu imagen
		&fontTextureView);
	if (FAILED(hr)) {
		// Manejar el error (por ejemplo, mostrar un mensaje y salir)
		OutputDebugStringA("Error al cargar la textura de la fuente.\n");
		return hr;
	}

	// Crear el Material para el texto
	Material* g_materialText = new Material(g_shaderManager, SHADER_TEXT);
	g_materialText->SetTexture(fontTextureView); // Establecer la textura de la fuente

	// Crear un objeto UIText
	UIText* myText = new UIText(g_materialText, "HOLA MUNDO 1234", { 10.0f, 10.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, 24.0f);
	myText->Init(g_device->GetDevice(), 16, 16); // fontAtlasWidth, fontAtlasHeight

	// Inicializar UI
	g_ui->Init(g_device, g_renderTarget);
	g_ui->AddElement(myText); // Agregar el texto a la UI
	return S_OK;
}

HRESULT MainGame::Init(HWND hwnd) {
	g_hwnd = hwnd;

	// Obtener las dimensiones de la ventana
	RECT clientRect;
	GetClientRect(hwnd, &clientRect);
	float width = static_cast<float>(clientRect.right - clientRect.left);
	float height = static_cast<float>(clientRect.bottom - clientRect.top);
	float aspectRatio = width / height;

	// Inicializar el mundo
	HRESULT result = g_device->Init(g_hwnd);
	if (FAILED(result)) return result;

	result = g_renderTarget->Init(g_device);
	if (FAILED(result)) return result;

	result = g_shaderManager->Init(g_device);
	if (FAILED(result)) return result;

	result = g_worldMatrixManager->Init(g_device);
	if (FAILED(result)) return result;

	// C�mara
	g_camera->SetPosition(-20.0f, 2.0f, -20.0f); // Eleva la c�mara y retrocede
	g_camera->SetRotation(XMConvertToRadians(45.0f), 0.0f, 0.0f); // Inclina la c�mara hacia abajo
	g_camera->SetProjectionParams(XM_PIDIV4, aspectRatio, 0.1f, 1000.0f); // Establecer los par�metros de proyecci�n

	// Material
	Material* materialBase = new Material(g_shaderManager, SHADER_BASE);
	result = materialBase->Init(g_device->GetDevice());
	if (FAILED(result)) return result;
	Material* materialVoxel = new Material(g_shaderManager, SHADER_VOXEL);
	result = materialVoxel->Init(g_device->GetDevice());
	if (FAILED(result)) return result;

	// Suelo
	//g_suelo = new Suelo(g_material);
	//g_suelo->Init(g_device->GetDevice());

	// Voxel Debug
	//g_voxelDebug = new VoxelDebug(materialBase);
	//result = g_voxelDebug->Init(g_device->GetDevice());
	//if (FAILED(result)) return result;

	// Octree Debug
	g_octreeDebug = new OctreeDebug(materialBase);
	result = g_octreeDebug->Init(g_device->GetDevice());
	if (FAILED(result)) return result;

	// Axis
	g_axis = new Axis(materialBase);
	g_axis->Init(g_device->GetDevice());

	// Mundo
	g_world->Init(materialVoxel);
	//g_line->Init(g_device->GetDevice());
	//g_chunkRenderer->SetChunkMaterial(g_material);  // Set the material
	//result = g_chunkRenderer->Init(g_device->GetDevice());
	if (FAILED(result)) return result;

	// UI
	result = InitUI();
	if (FAILED(result)) return result;

	return S_OK;
}

void MainGame::Update(float deltaTime) {
	// Actualizar mundo
	g_world->Update(deltaTime);
	UIText* uiText = dynamic_cast<UIText*>(g_ui->uiElements[0]); // Aseg�rate de que el primer elemento sea UIText
	//int numVisibleChunks = g_world->GetNumVisibleChunks();
	//int numChunks = g_world->GetNumChunks();
	//uiText->SetText("FRUSTRUM CHUNKS: (" + std::to_string(numVisibleChunks) + ") TOTAL CHUNKS: (" + std::to_string(numChunks) + ")");
	
	//XMFLOAT3 uiBoxPos = g_chunkDebug->GetBox0Origin();
	//uiText->SetText("`UIBOX: (x:" + std::to_string(uiBoxPos.x) + ", y:" + std::to_string(uiBoxPos.y) + ", z:"
	//+ std::to_string(uiBoxPos.z) + ")"); // Camera position
	//XMFLOAT3 cameraLookAt = g_camera->GetLookAtPosition();
	//uiText->SetText("`CameraPost: (x:" + std::to_string(cameraLookAt.x) + ", y:" + std::to_string(cameraLookAt.y) + ", z:" + std::to_string(cameraLookAt.z) + ")"); // Camera position
	XMFLOAT3 cameraPosition = g_camera->GetPosition();
	uiText->SetText("`CameraPost: (x:" + std::to_string(cameraPosition.x) + ", y:" + std::to_string(cameraPosition.y) + ", z:" + std::to_string(cameraPosition.z) + ")"); // Camera position
	//XMFLOAT3 pos = g_world->GetCameraChunkPosition(g_camera);
	//uiText->SetText("`Chunk: (x:" + std::to_string(pos.x) + ", y:" + std::to_string(pos.y) + ", z:" + std::to_string(pos.z) + ")"); // Actualizar texto con FPS
}

void MainGame::Render() {
	// Renderizado
	g_renderTarget->SetRenderTargets();	
	XMMATRIX worldMatrix = XMMatrixIdentity();
	g_worldMatrixManager->Render(g_device, g_renderTarget, g_camera, worldMatrix); // Actualizar matrices
	g_renderTarget->EnableClearEffect(true);
	g_renderTarget->SetClearEffectSpeed(0.5f); // Ajusta la velocidad
	g_renderTarget->Render();  // Limpiar pantalla

	// Axis
	g_axis->Render(g_device->GetContext());
	
	//worldMatrix = g_suelo->GetWorldMatrix();	
	g_worldMatrixManager->Render(g_device, g_renderTarget, g_camera, worldMatrix); // Actualizar matrices
	//g_suelo->Render(g_device->GetContext()); // Dibujar suelo	
	g_world->Render(g_device->GetContext()); // Renderizar el mundo
	//g_octreeDebug->Render(g_camera, g_device, g_worldMatrixManager, g_world);
	//g_chunkRenderer->Render(g_device->GetContext(), g_camera->GetViewMatrix() * worldMatrix);
	//g_voxelDebug->Render(g_device->GetContext(), g_device, g_worldMatrixManager, g_world);

	//g_line->Render(g_device->GetContext()); // Dibujar l�nea
	UINT screenWidth = static_cast<UINT>(g_renderTarget->GetViewport().Width);
	UINT screenHeight = static_cast<UINT>(g_renderTarget->GetViewport().Height);
	XMMATRIX orthoMatrix = XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, 0.1f, 1000.0f);
	g_ui->Render(g_device, g_shaderManager, g_worldMatrixManager, orthoMatrix);	
	g_device->Render(); // Presentar frame	
}

void MainGame::Release() {
	SafeDelete(g_shaderManager);
	SafeRelease(g_triangulo);
	SafeRelease(g_axis);
	SafeDelete(g_camera);
	SafeRelease(g_suelo);
	//SafeDelete(g_voxelDebug);
	//SafeDelete(g_chunkDebug);
	SafeDelete(g_octreeDebug);
	//SafeDelete(g_chunkManager);
	//SafeDelete(g_chunkRenderer);
	SafeDelete(g_world);
	SafeRelease(g_ui);
	SafeRelease(g_worldMatrixManager);
	SafeRelease(g_material);
	SafeRelease(g_renderTarget);
	SafeRelease(g_device);
	SafeRelease(g_matrixBuffer);	
}