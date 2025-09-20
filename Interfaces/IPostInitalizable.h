// IPostInitializable.h (para managers que necesitan post-inicialización)
#pragma once
#include <Windows.h>

class IPostInitializable {
public:
    virtual ~IPostInitializable() = default;
    virtual HRESULT PostInit() { return S_OK; };
};
