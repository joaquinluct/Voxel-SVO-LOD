#include "UIDebug.h"
#include <REGISTER_SERVICE_MACRO.h>

//using namespace Microsoft::WRL;
//using namespace DirectX;

REGISTER_SERVICE_TYPE(UIDebug, "UIDebug");

struct VertexDebug {
    XMFLOAT3 position;
};

UIDebug::UIDebug() : g_materialText(nullptr), m_text1{}, m_text2{}, m_config(), m_UIManager(), m_cameraUIText() {}
UIDebug::~UIDebug() {}

HRESULT UIDebug::Init() {

	m_config = new UIDebugConfig();

	//std::shared_ptr<ITextureConfig> config = ConfigLocator::GetConfig<ITextureConfig>(m_config->texture + "Config");

	//std::string path = config->file_path;
	std::shared_ptr<TextureAsset> textureAsset = AssetLocator::GetAsset<TextureAsset>(m_config->texture);

	m_deviceManager = ManagerLocator::GetDeviceManager();
	m_shaderManager = ManagerLocator::GetShaderManager();
	m_UIManager = ManagerLocator::GetManager<UIManager>();
	m_worldMatrixManager = ManagerLocator::GetWorldMatrixManager();
	std::shared_ptr<AssetManager> assetManager = ManagerLocator::GetManager<AssetManager>();

	// Cargar la textura de la fuente
	//ID3D11ShaderResourceView* fontTextureView = assetManager->LoadTexture(m_config->texture);
	g_materialText = new Material();
	textureAsset->SetTextureView(g_materialText);
	//HRESULT hr = TextureLoader::LoadTextureFromFile(m_deviceManager->GetDevice(),
	//	//"Resources/DejaVuSansMono.jpg", // Ruta a tu imagen
	//	path,
	//	&fontTextureView);
	//if (FAILED(hr)) {
	//	// Manejar el error (por ejemplo, mostrar un mensaje y salir)
	//	OutputDebugStringA("Error al cargar la textura de la fuente.\n");
	//	return hr;
	//}

	// Crear el Material para el texto
	g_materialText->SetShaderName(SHADER_TEXT);
	g_materialText->Init();

	m_cameraManager = ManagerLocator::GetManager<CameraManager>();

	// Crear un objeto UIText
	m_text1 = new UIText(m_deviceManager, m_worldMatrixManager, g_materialText);
	m_text1->SetText("Camera Pos");
	m_text1->SetPosition(10.0f, 40.0f); // Establecer la posición del texto
	m_text1->SetColor(1.0f, .0f, 1.0f, 1.0f); // Establecer el color del texto (blanco)
	m_text1->SetFontSize(18);

	// Inicializar UI	
	m_UIManager->AddElement(m_text1); // Agregar el texto a la UI

	// Crear un objeto UIText
	m_text2 = new UIText(m_deviceManager, m_worldMatrixManager, g_materialText);
	m_text2->SetText("Hello World");
	m_text2->SetPosition(10.0f, 10.0f); // Establecer la posición del texto
	m_text2->SetColor(.0f, .0f, 1.0f, 1.0f); // Establecer el color del texto (blanco)
	m_text2->SetFontSize(18);
	
	// Inicializar UI	
	m_UIManager->AddElement(m_text2); // Agregar el texto a la UI
	return S_OK;
}

void UIDebug::Render() {
	
}

void UIDebug::Update(float deltaTime) {	
	XMFLOAT3 cameraPosition = m_cameraManager->GetCurrentCameraPosition();
	XMFLOAT3 cameraRotation = m_cameraManager->GetCurrentCameraRotation();
	UIText* uiText = dynamic_cast<UIText*>(m_UIManager->uiElements[0]);
	uiText->SetText(
		"Camera Pos (" + roundFloat(cameraPosition.x) + "/" + roundFloat(cameraPosition.y) + "/" + roundFloat(cameraPosition.z) + ")\n" +
		"Camera Rotation (" + std::to_string(cameraRotation.x) + "/" + std::to_string(cameraRotation.y) + "/" + std::to_string(cameraRotation.z) + ")\n"
	);
}



void UIDebug::Shutdown() {
    // Todos los ComPtrs se limpian automáticamente
	SafeRelease(m_text1);
	SafeRelease(m_text2);
	SafeShutDown(g_materialText);
	SafeDelete(m_config);
}