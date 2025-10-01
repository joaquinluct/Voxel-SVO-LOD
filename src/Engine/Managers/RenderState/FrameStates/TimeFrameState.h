#pragma once

#include <DirectXMath.h>
#include <memory>
#include <vector>
#include "FrameStateBase.h"
#include <Util/DateTime.h>
#include <Defines/TimeDefinition.h>

class TimeFrameState : public FrameStateBase {
public:
    static TimeDefinition::TimeData GetTimeData() {
        return TimeDefinition::TimeData{ .time = DateTime::GetTimeInSeconds() };
    };
};

