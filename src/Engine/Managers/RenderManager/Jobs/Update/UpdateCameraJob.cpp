#include "UpdateCameraJob.h"
#include <Defines/Matrix/CameraMatrix.h>
#include <Defines/Matrix/MatrixDefinition.h>
#include <Defines/Types/ThreadTypes.h>
#include <DirectXMathMatrix.inl>
#include <Managers/CameraManager.h>
#include <Managers/RenderState/FrameStates/CameraFrameState.h>
#include <Managers/RenderState/FrameStates/PipelineFrameState.h>
#include <Services/FrameStateService.h>

bool UpdateCameraJob::Execute(JobContext* context)
{
    // Ejectuar el padre, que hace una pequeña pausa
    // para dar tiempo al job a encolarse antes de resolverse
    IJob::Execute(context);

    // Actualizar el valor para los constantsBuffers
    MatrixDefinition::MatrixBufferType matrixBuffer{};
    CameraMatrix::CameraData cameraBuffer{};

    matrixBuffer.SetViewMatrix(DirectX::XMMatrixTranspose(context->cameraManager->GetCurrentViewMatrix()));
    matrixBuffer.SetProjectionMatrix(DirectX::XMMatrixTranspose(context->cameraManager->GetCurrentProjectionMatrix()));
    matrixBuffer.SetWorldMatrix(XMMatrixIdentity());

    cameraBuffer.SetCameraPosition(context->cameraManager->GetCurrentCameraPosition());

    context->frameStateService->PipelineState(false)->UpdateConstanBufferValue("MatrixBufferType", matrixBuffer);
    context->frameStateService->PipelineState(false)->UpdateConstanBufferValue("CameraData", cameraBuffer);

    // Actualizar el valor en el commonState
    context->frameStateService->CameraState(false)->SetCameraPosition(context->cameraManager->GetCurrentCameraPosition());
    context->frameStateService->CameraState(false)->SetViewMatrix(context->cameraManager->GetCurrentViewMatrix());
    context->frameStateService->CameraState(false)->SetProjectionMatrix(context->cameraManager->GetCurrentProjectionMatrix());

    return true;
}
