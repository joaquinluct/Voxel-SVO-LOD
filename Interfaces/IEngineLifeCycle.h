#pragma once

#include "IInitializable.h"
#include "IEngineDependentInitializable.h"
#include "IPostInitalizable.h"
#include "IUpdatable.h"
#include "IRenderable.h"
#include "IShutdownable.h"

class IEngineLifeCycle : public IEngineDependentInitializable, public IPostInitializable, public IUpdatable, public IRenderable, public IShutdownable
{
public:
    virtual HRESULT Init(EngineContext* context) override { return S_OK; }
    virtual bool IsWindowDependent() const { return false; }
};
