#pragma once

#include <windows.h>
#include <Defines//Context/EngineContext.h>

class IEngineDependentInitializable {
public:
    virtual ~IEngineDependentInitializable() = default;
    virtual HRESULT Init(EngineContext* context) = 0;
};
