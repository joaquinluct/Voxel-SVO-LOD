#pragma once

#include "IInitializable.h"
#include "IWindowDependentInitializable.h"
#include "IPostInitalizable.h"
#include "IUpdatable.h"
#include "IRenderable.h"
#include "IShutdownable.h"

class ILifeCycle : public IInitializable, public IWindowDependentInitializable, public IPostInitializable, public IUpdatable, public IRenderable, public IShutdownable
{
public:
	virtual HRESULT Init() override { return E_FAIL; }
	virtual HRESULT Init(HWND* hwnd, int width, int height) override { return S_OK; }
	virtual bool IsWindowDependent() const { return false; }
};