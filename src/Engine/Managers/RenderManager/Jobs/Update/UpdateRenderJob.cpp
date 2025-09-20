#include "UpdateRenderJob.h"
#include <Defines/Types/ThreadTypes.h>
#include <Game/Systems/Chronos.h>
#include <Managers/RenderState/FrameStates/CommonFrameState.h>
#include <Services/FrameStateService.h>

bool UpdateRenderJob::Execute(JobContext* context)
{
    context->frameStateService->CommonState(false)->SetSimulatedTime(context->chronos->GetCurrentSimulatedDateTime());
    return true;
}
