#include "UpdateMeshTerrainJob.h"
#include <Assets/Base/MeshAssetBase.h>
#include <chrono>
#include <Defines/EngineDefinition.h>
#include <Defines/Types/ThreadTypes.h>
#include <Game/Systems/Terrain.h>
#include <Game/Systems/World.h>
#include <Services/FrameStateService.h>
#include <thread>

bool UpdateMeshTerrainJob::Execute(JobContext* context)
{
    //if (m_isGenerating || context->frameStateService->IsRendering()) {
    if (m_isGenerating) {
        return false;
    }
    m_isGenerating = true;


    // Obtener el terreno
    Terrain* terrain = context->world->GetTerrain().get();

    if (terrain->IsGenerating() || terrain->GetTerrainMesh() != nullptr && terrain->GetTerrainMesh()->IsGenerating()) {
        return false; // No actualizar si el terreno está generando
    }

    // Actualizar el terreno
    //auto lock = context->frameStateService->LockAll();

    //context->frameStateService->Lock(FRAME_STATE_MESHES);
    /*while (context->frameStateService->IsRendering()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }*/

    /*while (context->frameStateService->IsRendering()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }*/
    terrain->GenerateMesh();

    MeshAssetBase* terrainMesh = terrain->GetTerrainMesh();

    //context->frameStateService->Unlock(FRAME_STATE_MESHES);
    int index = terrainMesh->GetWriteIndex();

    if (!terrainMesh || terrainMesh->GetVertexCount(index) <= 0) {
        m_isGenerating = false;
        return false; // No hay malla de terreno o no está cargada
    }

    /*while (context->frameStateService->IsRendering()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }*/
    terrainMesh->SwapBuffer();

    m_isGenerating = false;

    return true;
}
