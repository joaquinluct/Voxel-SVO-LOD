#pragma once
#include <windows.h>

class iRenderizable {
public:
    virtual HRESULT Init() = 0;     // Inicialización del objeto
    virtual void Render() = 0;      // Renderizar el objeto
    virtual void Release() = 0;     // Liberar recursos
};