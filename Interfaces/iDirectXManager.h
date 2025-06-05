#pragma once
#include <windows.h>

class iDirectXManager
{
public:
    virtual HRESULT Init()      = 0;
    virtual HRESULT Release()   = 0;
    virtual HRESULT Render()    = 0;
    virtual ~iDirectXManager()  = default;
};
