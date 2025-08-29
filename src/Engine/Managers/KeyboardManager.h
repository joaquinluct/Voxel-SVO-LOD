#pragma once

#include <ManagerBase.h>
#include <Services/Keyboard.h>
#include <Config/Base/Managers/KeyboardManagerConfig.h>
#include <Services/KeyBindings.h>

class KeyboardManager : public ManagerBase
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

	void HandleMessage(UINT message, WPARAM wParam, LPARAM lParam);
	
private:
	std::shared_ptr<Keyboard> m_keyboard;
	std::shared_ptr<KeyBindings> m_keyBindings;
	std::shared_ptr<KeyboardManagerConfig> m_config;
};
