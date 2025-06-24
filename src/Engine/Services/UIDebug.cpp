#include "UIDebug.h"
#include <REGISTER_SERVICE_MACRO.h>
#include <ManagerLocator/ManagerLocator.h>
#include <d3dcompiler.h>
#include <TextureLoader/TextureLoader.h>
#include <../Resources/resource.h>

using namespace Microsoft::WRL;
using namespace DirectX;

REGISTER_SERVICE_TYPE(UIDebug, "UIDebug");

struct VertexDebug {
    XMFLOAT3 position;
};

UIDebug::UIDebug() : g_materialText(nullptr), myText{} {}
UIDebug::~UIDebug() {}

HRESULT UIDebug::Init() {

	m_config = new UIDebugConfig::Values();

	m_deviceManager = ManagerLocator::GetDeviceManager();
	m_shaderManager = ManagerLocator::GetShaderManager();
	m_UIManager = ManagerLocator::GetManager<UIManager>();
	m_worldMatrixManager = ManagerLocator::GetWorldMatrixManager();

	// Cargar la textura de la fuente
	ID3D11ShaderResourceView* fontTextureView = nullptr;
	HRESULT hr = TextureLoader::LoadTextureFromFile(m_deviceManager->GetDevice(),
		"Resources/DejaVuSansMono.jpg", // Ruta a tu imagen
		&fontTextureView);
	if (FAILED(hr)) {
		// Manejar el error (por ejemplo, mostrar un mensaje y salir)
		OutputDebugStringA("Error al cargar la textura de la fuente.\n");
		return hr;
	}

	// Crear el Material para el texto
	g_materialText = new Material(SHADER_TEXT);	
	g_materialText->Init(m_deviceManager->GetDevice());
	g_materialText->SetTexture(fontTextureView); // Establecer la textura de la fuente

	// Crear un objeto UIText
	myText = new UIText(m_deviceManager, m_worldMatrixManager, g_materialText);
	myText->SetText("Hello World");
	myText->SetPosition(100.0f, 100.0f); // Establecer la posición del texto
	myText->SetColor(.0f, .0f, 1.0f, 1.0f); // Establecer el color del texto (blanco)
	myText->SetFontSize(14);
	
	// Inicializar UI	
	m_UIManager->AddElement(myText); // Agregar el texto a la UI
	return S_OK;
}

void UIDebug::Render() {
	
}

void UIDebug::Update(float deltaTime) {	
	UIText* uiText = dynamic_cast<UIText*>(m_UIManager->uiElements[0]);
	uiText->SetText("CameraPost");
}



void UIDebug::Shutdown() {
    // Todos los ComPtrs se limpian automáticamente
}