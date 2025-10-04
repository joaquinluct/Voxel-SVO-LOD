#include "UpdateTerrainJob.h"
#include <Assets/Base/MeshAssetBase.h>
#include <Defines//Context/EngineContext.h>
#include <Defines/Usings/ThreadTypes.h>
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

	if (terrain->IsGenerating() || terrain->GetTerrainMesh() != nullptr && terrain->GetTerrainMesh()->IsGenerating()) {
		return false; // No actualizar si el terreno está generando
	}

	// Actualizar el terreno
	//auto lock = context->frameStateService->LockAll();

	//context->frameStateService->Lock(FRAME_STATE_MESHES);
	/*while (context->frameStateService->IsRendering()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}*/

	terrain->Update(context->engineContext->deltaTime);

	m_isGenerating = false;

	return true;
}
