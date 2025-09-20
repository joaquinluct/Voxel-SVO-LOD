#pragma once

#include <windows.h>
#include <Defines/EngineDefinition.h>

class IEngineDependentInitializable {
public:
    virtual ~IEngineDependentInitializable() = default;
    virtual HRESULT Init(EngineContext* context) = 0;
};