// MainController.h
#pragma once

#include <windows.h>
#include <vector>
#include <memory>
#include <ManagerLocator/ManagerLocator.h> // Para registrar y obtener servicios
#include <ServiceLocator/ServiceLocator.h> // Para registrar y obtener servicios
#include "ISubsystem.h"     // Para gestionar subsistemas
// Incluye los encabezados de todos tus managers/servicios
#include "IRenderable.h"
#include "IUpdatable.h"
#include "IShutdownable.h"
#include "DeviceManager.h"
#include "AssetManager.h"
#include "RenderTargetManager.h"
#include "ShaderManager.h"
#include "WorldMatrixManager.h"
#include "Devices/Mouse.h"    // El raton crudo
#include "KeyboardManager.h" // Tu manager de teclado
#include "UIManager.h" // Si tienes un manager de UI
#include <Services/AxisXYZ.h>
#include <Services/TestingBasic.h>
#include <Services/UIDebug.h>
#include <Services/Keyboard.h> // Si tienes servicios de prueba avanzados
#include <Config/Base/EngineConfig.h>
#include <Config/Base/ServiceConfig.h>

// Incluye el encabezado de tu lógica de juego
#include "GameManager.h"
//#include "UI.h" // Si la UI es un subsistema aparte

class MainController: public IRenderable, public IUpdatable, public IShutdownable {
public:
    MainController(HWND hwnd, int width, int height);
    ~MainController();

    HRESULT Initialize(HWND hwnd, int width, int height);
    void Update(float deltaTime) override;
    void Render() override;
    void Shutdown() override; // Nuevo método para liberar todos los recursos

    // Métodos para manejar eventos de ventana, si es necesario pasarlos a subsistemas
    void HandleKeyboardInput(WPARAM wParam, LPARAM lParam);
    void HandleMouseInput(WPARAM wParam, LPARAM lParam);
    void ToggleFullscreen();

	// Métodos para registrar y obtener subsistemas
    std::shared_ptr<Keyboard>& GetKeyboard() {
        return m_keyboard;
    };

private:
    HWND m_hwnd;
    int m_width;
    int m_height;
    EngineConfig::Values* m_config = new EngineConfig::Values();
    ServiceConfig::Values* m_serviceConfig = new ServiceConfig::Values();

    // Lista de subsistemas que el MainController debe orquestar
    std::vector<std::shared_ptr<ISubsystem>> m_subsystems;
	std::shared_ptr<Keyboard> m_keyboard;

    // Puedes tener un puntero directo a GameManager si es el subsistema principal
    std::shared_ptr<GameManager> m_GameManager;
    std::shared_ptr<UIManager> m_UIManager;
};