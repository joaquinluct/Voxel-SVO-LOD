#pragma once

#include "IInitializable.h" // Hereda de IInitializable para reusar el destructor virtual
#include <windows.h>      // Para HWND
#include <winnt.h> // Para HRESULT

class IWindowDependentInitializable {
public:
    virtual ~IWindowDependentInitializable() = default;
    virtual HRESULT Init(HWND* hwnd, int width, int height) = 0;
};