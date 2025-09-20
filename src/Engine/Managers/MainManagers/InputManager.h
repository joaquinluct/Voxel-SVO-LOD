#pragma once

#include <ManagerBase.h>
#include <memory>
#include <MouseManager.h>
#include <KeyboardManager.h>

class InputManager : public ManagerBase
{
public:
    //--------------------------------------------------------------------------------------
    // Constructor y destructor
    //--------------------------------------------------------------------------------------
    InputManager();
    ~InputManager() override;

    //--------------------------------------------------------------------------------------
    // Inicialización del manager de entrada
    //--------------------------------------------------------------------------------------
    HRESULT Init(EngineContext* context) override;

    //--------------------------------------------------------------------------------------
    // Maneja los mensajes de la ventana y los delega a los servicios apropiados.
    //--------------------------------------------------------------------------------------
    void HandleInput(UINT message, WPARAM wParam, LPARAM lParam, float deltaTime);

    //--------------------------------------------------------------------------------------
    // Obtiene el nombre del manager.
    //--------------------------------------------------------------------------------------
    const std::string& GetManagerName() const override {
        static const std::string name = "InputManager";
        return name;
    }
    static const std::string& GetStaticManagerName() {
        static const std::string name = "InputManager";
        return name;
    }

private:
    //--------------------------------------------------------------------------------------
    // Servicios de entrada.
    //--------------------------------------------------------------------------------------
    std::shared_ptr<MouseManager> m_mouseManager;
    std::shared_ptr<KeyboardManager> m_keyboardManager;
};
