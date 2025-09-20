#include "UpdateCommonJob.h"
#include <Defines/Matrix/TimeMatrices.h>
#include <Defines/Types/ThreadTypes.h>
#include <Game/Systems/Chronos.h>
#include <Managers/RenderState/FrameStates/CommonFrameState.h>
#include <Managers/RenderState/FrameStates/PipelineFrameState.h>
#include <Services/FrameStateService.h>
#include <Util/Chronos.h>

bool UpdateCommonJob::Execute(JobContext* context)
{
    // Ejectuar el padre, que hace una pequeña pausa
    // para dar tiempo al job a encolarse antes de resolverse
    IJob::Execute(context);

    // Actualizar el valor para los constantsBuffers
    TimeMatrices::TimeData timeData{};
    timeData.SetTime(ConvertTimeToFloat(context->chronos->GetCurrentSimulatedDateTime()));
    context->frameStateService->PipelineState(false)->UpdateConstanBufferValue("TimeData", timeData);


    // Actualizar el valor en el commonState
    context->frameStateService->CommonState(false)->SetSimulatedTime(context->chronos->GetCurrentSimulatedDateTime());

    return true;
}
