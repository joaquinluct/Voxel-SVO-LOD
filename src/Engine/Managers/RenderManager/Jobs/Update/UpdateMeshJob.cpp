#include "UpdateMeshJob.h"
#include <Defines/Contants/FrameState.h>
#include <Defines/Structs/Pipeline/PipelineResources.h>
#include <Defines/Usings/ThreadTypes.h>
#include <Managers/RenderState/FrameStates/MeshesFrameState.h>
#include <Services/FrameStateService.h>
#include <vector>

static bool isFirstTime = true;

bool UpdateMeshJob::Execute(JobContext* context)
{
    if (m_isGenerating) {
        return false;
    }
    m_isGenerating = true;

    std::vector<MeshResource*> meshes = context->meshes;

    context->frameStateService->MeshesState(false)->AddMeshesOperation(meshes);

    if (isFirstTime) {
        context->frameStateService->SwapBufferContent(FRAME_STATE_MESHES);
        isFirstTime = false;
    }
    else {
        context->frameStateService->SwapBuffer(FRAME_STATE_MESHES);
    }

    m_isGenerating = false;

    return true;
}
