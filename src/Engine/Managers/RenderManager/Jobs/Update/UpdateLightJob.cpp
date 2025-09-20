#include "UpdateLightJob.h"
#include <Defines/Types/ThreadTypes.h>
#include <Game/Systems/Lighting.h>
#include <Game/Systems/Shadows.h>
#include <Services/FrameStateService.h>
#include <Managers/RenderState/FrameStates/LightFrameState.h>
#include <Managers/RenderState/FrameStates/PipelineFrameState.h>

#include <Defines/Matrix/Light.h>

bool UpdateLightJob::Execute(JobContext* context)
{
    // Ejectuar el padre, que hace una pequeña pausa
    // para dar tiempo al job a encolarse antes de resolverse
    IJob::Execute(context);

    Light::DirectionalLight light{};
    light.SetDirectionalLight(context->ligting->GetLightDirection(), context->ligting->GetLightColor());
    Light::LightSpaceMatrices lightSpaceMatrices{};
    lightSpaceMatrices.SetLightViewProjection(context->shadows->GetLightViewProjectionMatrix());

    context->frameStateService->PipelineState(false)->UpdateConstanBufferValue("DirectionalLight", light);
    context->frameStateService->PipelineState(false)->UpdateConstanBufferValue("LightSpaceMatrices", lightSpaceMatrices);


    // Update the camera's view matrix based on its position and orientation
    context->frameStateService->LightingState(false)->SetLightDirection(context->ligting->GetLightDirection());
    context->frameStateService->LightingState(false)->SetLightColor(context->ligting->GetLightColor());

    return true;
}
