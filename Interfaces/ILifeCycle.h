#pragma once

#include "IInitializable.h"
#include "IUpdatable.h"
#include "IRenderable.h"
#include "IShutdownable.h"

class ILifeCycle : public IInitializable, public IUpdatable, public IRenderable, public IShutdownable
{};