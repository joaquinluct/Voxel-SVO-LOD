#pragma once

#include "IManager.h"
#include <Services/Keyboard.h>
#include "IInitializable.h"
#include "IUpdatable.h"
#include "IRenderable.h"
#include "IShutdownable.h"


class KeyboardManager : public IManager, public IInitializable, public IUpdatable, public IRenderable, public IShutdownable
{
public:
	KeyboardManager();
	~KeyboardManager();

	HRESULT Init() override;
	void Render() override;
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
private:
	std::shared_ptr<Keyboard> m_keyboard;
};
