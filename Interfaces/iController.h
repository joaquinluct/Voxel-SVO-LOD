#pragma once
#include <windows.h>

class iController
{
	virtual HRESULT Init();
	virtual void	Cleanup();
	virtual void	Render();
};
