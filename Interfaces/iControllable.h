#pragma once
#include "../Managers/KeyboardManager.h"

class iControllable {
	public:
		virtual void SetKeyboardManager(KeyboardManager* keyboardManager) = 0;
};
