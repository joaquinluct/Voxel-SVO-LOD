#include <windows.h>
#include "../Interfaces/iDirectXManager.h"

class ViewportManager : public iDirectXManager
{
	HRESULT Init() override
	{
		return S_OK;
	}
	HRESULT Release() override
	{
		return S_OK;
	}

};
