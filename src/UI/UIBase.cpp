#include "UIBase.h"
#include <REGISTER_SERVICE_MACRO.h>

REGISTER_SERVICE_TYPE(UIBase, "UIBase")

UIBase::UIBase() {
	// Constructor implementation
}

UIBase::~UIBase() {
	// Destructor implementation
}

HRESULT UIBase::Init() {
	// Initialization code
	return S_OK;
}

void UIBase::Render() {

}

void UIBase::Update(float deltatime) {
	// Update logic
}

void UIBase::Shutdown() {
	// Cleanup code
}