#pragma once

#include "IEngineDependentInitializable.h"
#include "IUpdatable.h"
#include "IRenderable.h"
#include "IShutdownable.h"

class ILifeCycleWindowDepent : public IEngineDependentInitializable, public IUpdatable, public IRenderable, public IShutdownable
{
public:
	~ILifeCycleWindowDepent() = default;
};