#include "UpdateMeshTerrainJob.h"
#include <Defines/Types/ThreadTypes.h>
#include <Game/System/TerrainConfig.h>
#include <Game/Systems/Terrain.h>
#include <Game/Systems/World.h>

bool UpdateMeshTerrainJob::Execute(JobContext* context)
{
    Terrain* terrain = context->world->GetTerrain().get();
    if (!terrain) return false;

    // FASE 1: UPDATE - Generar geometría y actualizar chunks (puede ser async)
    if (!terrain->IsGenerating()) {
        terrain->SetGenerating(true);
        
        // Generar la geometría del terreno
        terrain->Generate();
        
        terrain->SetGenerating(false);
        
        OutputDebugStringA("[UpdateMeshTerrainJob] Terrain geometry generated successfully\n");
    }

    // FASE 2: RENDER - Renderizado inmediato (thread principal)
    // Nota: El renderizado debe ejecutarse siempre en el frame actual para mantener consistencia visual
    terrain->Render();

    return true;
}
