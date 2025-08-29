#pragma once
#include <windows.h>
#include <IManager.h>
#include <ILifeCycle.h>

class ManagerBase : public IManager, public ILifeCycle
{
public:
	ManagerBase() = default;
	virtual ~ManagerBase() = default;			
	virtual HRESULT Init() override { return E_FAIL; }
	virtual HRESULT Init(HWND* hwnd, int width, int height) override { return S_OK; }
};
