// MainController.h
#pragma once

#include <Defines/Components.h>
#include "InitController.h"
#include <Services/Mouse.h>
#include <windows.h>
#include <vector>
#include <memory>
#include <ManagerLocator/ManagerLocator.h>
#include <ServiceLocator/ServiceLocator.h>
#include "ISubsystem.h"
#include "IRenderable.h"
#include "IUpdatable.h"
#include "IShutdownable.h"
#include "DeviceManager.h"
#include "AssetManager.h"
#include "RenderTargetManager.h"
#include "ShaderManager.h"
#include "WorldMatrixManager.h"
#include <InitManager.h>
#include <RenderManager/RenderManager.h>
#include <KeyboardManager.h>
//#include "RenderObjects/Line/Line.h"
//#include "RenderObjects/UIBox/UIBox.h"
#include <UIManager.h>
#include <Services/Keyboard.h>
#include <Config/Base/EngineConfig.h>
#include <Config/Services/ServiceConfig.h>
#include <Assets/Base/TextureAsset.h>
//#include <Game/GameObjects/AxisXYZ.h>
//#include <Game/GameObjects/TestingBasic.h>
//#include <Game/GameObjects/UIDebug.h>
#include <Camera/FirstPersonCamera.h>
#include <GameManager.h>

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

    std::shared_ptr<Mouse>& GetMouse() {
        return m_mouse;
	};

private:
    HWND m_hwnd;
    int m_width;
    int m_height;
    std::shared_ptr<EngineConfig> m_config;
    std::shared_ptr<ServiceConfig> m_serviceConfig;

	InitController* m_initController;

    // Lista de subsistemas que el MainController debe orquestar
    std::vector<std::shared_ptr<ISubsystem>> m_subsystems;
    std::shared_ptr<Keyboard> m_keyboard;
    std::shared_ptr<Mouse> m_mouse;

    // Puedes tener un puntero directo a GameManager si es el subsistema principal
    std::shared_ptr<GameManager> m_GameManager;
    std::shared_ptr<UIManager> m_UIManager;
    std::shared_ptr<RenderManager> m_renderManager;
    std::shared_ptr<InitManager> m_initManager;
};