#include "UpdateTerrainJob.h"
#include <Defines/EngineDefinition.h>
#include <Defines/Types/ThreadTypes.h>
#include <Game/Systems/Terrain.h>
#include <Game/Systems/World.h>

bool UpdateTerrainJob::Execute(JobContext* context)
{
    //if (m_isGenerating || context->frameStateService->IsRendering()) {
    if (m_isGenerating) {
        return false;
    }
    m_isGenerating = true;

    // Obtener el terreno
    Terrain* terrain = context->world->GetTerrain().get();

    if (terrain->IsGenerating()) {
        return false; // No actualizar si el terreno está generando
    }

    terrain->Update(context->engineContext->deltaTime);

    m_isGenerating = false;

    return true;
}
