#pragma once

#include <windows.h>
#include <DirectXMath.h>
#include <memory>
#include <vector>
#include <wrl/client.h>
#include "FrameStateBase.h"
#include <Util/DateTime.h>
#include <Defines/TimeDefinition.h>
#include <Config/PassConfigBase.h>

class PassFrameState : public FrameStateBase {
public:
	const PassConfigBase& GetConfig() const { return config; }
	void SetConfig(const PassConfigBase& cfg) { config = cfg; }
private:
	PassConfigBase config;
};

