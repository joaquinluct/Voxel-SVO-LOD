#pragma once

#include "IManager.h"
#include <Services/Keyboard.h>
#include "IInitializable.h"
#include "IUpdatable.h"
#include "IRenderable.h"
#include "IShutdownable.h"
#include <Config/Base/KeyboardManagerConfig.h>
#include <Services/KeyBindings.h>

class KeyboardManager : public IManager, public IInitializable, public IUpdatable, public IRenderable, public IShutdownable
{
public:
	KeyboardManager();
	~KeyboardManager();

	HRESULT Init() override;
	HRESULT InitKeyBindings();
	void Render() override {};
	void Update(float deltaTime) override {};
	void Shutdown() override;
	const std::string& GetManagerName() const override { 
		static const std::string name = "KeyboardManager";
		return name;
	}
	static const std::string& GetStaticManagerName()
	{
		static const std::string name = "KeyboardManager";
		return name;
	}

	bool IsKeyDown(int keyCode) const;
	bool IsKeyPressed(int keyCode) const;
	bool IsKeyReleased(int keyCode) const;
	bool OnKeyReleased(int keyCode) const;

	bool IsKeyDown(KeyMoves keyMove) const;
	bool IsKeyPressed(KeyMoves keyMove) const;
	bool IsKeyReleased(KeyMoves keyMove) const;

	
private:
	std::shared_ptr<Keyboard> m_keyboard;
	std::shared_ptr<KeyBindings> m_keyBindings;
	std::shared_ptr<KeyboardManagerConfig> m_config;
};
