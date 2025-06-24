// GameManager.h
#pragma once

#include "IManager.h"
#include "IInitializable.h"
#include "IRenderable.h"
#include "IUpdatable.h"
#include "IShutdownable.h"
#include <string>
#include <memory>
#include "CameraManager.h"
#include "KeyboardManager.h"
#include "Config/Game/GameEngineConfig.h"
#include "Config/Base/GameManagerConfig.h"


class GameManager : public IManager, public IInitializable, public IRenderable, public IUpdatable, public IShutdownable {
public:
    GameManager();
    ~GameManager() override;

    // Desde IManager
    const std::string& GetManagerName() const override { 
        static const std::string name = "GameManager";
        return name;
    }
    static const std::string& GetStaticManagerName()
    {
        static const std::string name = "GameManager";
        return name;
    }

    // Desde IInitializable
    HRESULT Init();

    void Update(float deltaTime) override;
    void Render() override;
	void Shutdown() override;

private:    
    HRESULT InitGame();
    HRESULT InitGameServices() const;

    GameManagerConfig::Values* m_config{}; // Configuración base del GameManager
    GameEngineConfig::Values* m_gameConfig{}; // Configuración del motor de juego

    // Punteros a otros managers que GameManager necesitará para su lógica
    std::shared_ptr<CameraManager> m_cameraManager;
    std::shared_ptr<KeyboardManager> m_keyboardManager;
  
  
};