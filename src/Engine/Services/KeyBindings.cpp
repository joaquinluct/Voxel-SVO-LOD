#include "KeyBindings.h"
#include <REGISTER_SERVICE_MACRO.h>
#include <ServiceLocator/ServiceLocator.h>
#include <Util/Array/ArrayUtility.h>
#include <Util/Text/Text.h>

REGISTER_SERVICE_TYPE(KeyBindings, "KeyBindings")

KeyBindings::KeyBindings() {
    // Inicializar las asociaciones de teclas con movimientos
    //m_keyBindings[KeyMoves::Forward] =
}

KeyBindings::~KeyBindings() {
    // Destructor, si es necesario
}

HRESULT KeyBindings::Init() {
	//m_keyboard = ServiceLocator::GetService<Keyboard>();
	//if (!m_keyboard) {
	//	return E_FAIL; // Error al obtener el servicio de teclado
	//}

	return S_OK;
}

static KeyMoves MapAction(const std::string& action) {
	std::string lowerAction = StrToLower(action);
	if (lowerAction == "move_forward") return KeyMoves::Forward;
	if (lowerAction == "move_backward") return KeyMoves::Backward;
	if (lowerAction == "move_left") return KeyMoves::Left;
	if (lowerAction == "move_right") return KeyMoves::Right;
	if (lowerAction == "jump") return KeyMoves::Jump;
	if (lowerAction == "crouch") return KeyMoves::Crouch;
	if (lowerAction == "sprint") return KeyMoves::Sprint;
	if (lowerAction == "interact") return KeyMoves::Interact;
    if (lowerAction == "inventory") return KeyMoves::ToggleInventory;
    if (lowerAction == "switch_camera") return KeyMoves::ToggleCamera;
	return KeyMoves::None; // Acción no reconocida
}

void KeyBindings::Bind(std::map<std::string, std::vector<std::string>> key_bindings)
{
	// Iterar sobre las asociaciones de teclas y asignarlas
	for (const auto& pair : key_bindings) {
		const std::string& action = pair.first;
		const std::vector<std::string>& keys = pair.second;
		for (const auto& key : keys) {
			unsigned char keyCode = TranslateKeyStringToCode(key);
			if (keyCode != 0) { // Verificar si el código de tecla es válido
				KeyMoves keyMove = MapAction(action);
				m_keyBindings[keyMove].push_back(keyCode);
			}
		}
	}
}

bool KeyBindings::IsKeyPressed(KeyMoves keyMove) {
	// Verificar si alguna de las teclas asociadas a keyMove está presionada
	m_keyboard = ServiceLocator::GetService<Keyboard>();
	auto it = m_keyBindings.find(keyMove);
	if (it != m_keyBindings.end()) {
		for (unsigned char keyCode : it->second) {
			if (m_keyboard->IsKeyPressed(keyCode)) {
				return true;
			}
		}
	}
	return false;
}

bool KeyBindings::IsKeyDown(KeyMoves keyMove) {
	// Verificar si alguna de las teclas asociadas a keyMove está presionada
	m_keyboard = ServiceLocator::GetService<Keyboard>();	
	auto it = m_keyBindings.find(keyMove);
	if (it != m_keyBindings.end()) {
		for (unsigned char keyCode : it->second) {
			if (m_keyboard->IsKeyDown(keyCode)) {
				return true;
			}
		}
	}
	return false;
}

bool KeyBindings::IsKeyReleased(KeyMoves keyMove) {
	// Verificar si alguna de las teclas asociadas a keyMove está presionada
	m_keyboard = ServiceLocator::GetService<Keyboard>();
	auto it = m_keyBindings.find(keyMove);
	if (it != m_keyBindings.end()) {
		for (unsigned char keyCode : it->second) {
			if (m_keyboard->IsKeyReleased(keyCode)) {
				return true;
			}
		}
	}
	return false;
}

static std::vector<std::string> keySpecials = {
	"UP", "DOWN", "LEFT", "RIGHT","SHIFT_LEFT", "SHIFT_RIGHT", "CTRL_LEFT", "CTRL_RIGHT", "SPACE", "TAB"
};

unsigned char KeyBindings::TranslateKeyStringToCode(const std::string& key) const {
	// Implementar la lógica para traducir cadenas como "KEY_W" a códigos de teclas
	std::string baseKey = key;
	StringReplace(baseKey, "KEY_", "");

	std::string spcecial = ArrayUtility::find(keySpecials, baseKey);

	if (spcecial.empty()) {
		return baseKey[0]; // Retorna el primer carácter como código de tecla
	}

	if (key == "KEY_UP") return VK_UP;
	if (key == "KEY_DOWN") return VK_DOWN;
	if (key == "KEY_LEFT") return VK_LEFT;
	if (key == "KEY_RIGHT") return VK_RIGHT;
	if (key == "KEY_SPACE") return VK_SPACE;
	if (key == "KEY_CTRL_LEFT") return 17;
	if (key == "KEY_CTRL_RIGHT") return 16;
	if (key == "KEY_SHIFT_LEFT") return 17;
	if (key == "KEY_SHIFT_RIGHT") return 16;
	if (key == "KEY_TAB") return VK_TAB;
	
	return 0; // Código de tecla inválido
}

void KeyBindings::Shutdown() {
	// Liberar recursos si es necesario
	m_keyBindings.clear();	
}
