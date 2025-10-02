#pragma once

#include "IEngineDependentInitializable.h"
#include "IInitializable.h"
#include "IPostInitalizable.h"
#include "IRenderable.h"
#include "IShutdownable.h"
#include "IUpdatable.h"
#include <Defines/EngineDefinition.h>

class ILifeCycle : public IInitializable, public IEngineDependentInitializable, public IPostInitializable, public IUpdatable, public IRenderable, public IShutdownable
{
protected:
    bool m_isInitialized = false;
public:
    virtual HRESULT Init() override { return E_FAIL; }
    virtual HRESULT Init(EngineContext* context) override { return S_OK; }
    virtual bool IsWindowDependent() const { return false; }
    bool IsInitialized() const { return m_isInitialized; }
    void SetInitialized(bool initialized) { m_isInitialized = initialized; }
};
