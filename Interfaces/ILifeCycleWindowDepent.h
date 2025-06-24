#pragma once

#include "IWindowDependentInitializable.h"
#include "IUpdatable.h"
#include "IRenderable.h"
#include "IShutdownable.h"

class ILifeCycleWindowDepent : public IWindowDependentInitializable, public IUpdatable, public IRenderable, public IShutdownable
{
public:
	~ILifeCycleWindowDepent() = default;
};