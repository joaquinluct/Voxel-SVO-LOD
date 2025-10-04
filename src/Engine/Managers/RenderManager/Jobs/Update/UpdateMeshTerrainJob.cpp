#include "UpdateMeshTerrainJob.h"
#include <Assets/Base/MeshAssetBase.h>
#include <cstdint>
#include <Defines/Structs/RingBuffer.h>
#include <Defines/Usings/ThreadTypes.h>
#include <Game/Systems/Terrain.h>
#include <Game/Systems/World.h>
#include <Managers/GigaBufferManager.h>
#include <memory>
#include <utility>
#include <vector>

bool UpdateMeshTerrainJob::Execute(JobContext* context)
{
	if (m_isGenerating) {
		return false;
	}
	m_isGenerating = true;

	Terrain* terrain = context->world->GetTerrain().get();

	if (terrain->IsGenerating() || (terrain->GetTerrainMesh() != nullptr && terrain->GetTerrainMesh()->IsGenerating())) {
		m_isGenerating = false;
		return false;
	}

	// 1. GENERACIÓN DE GEOMETRÍA (CPU-Bound)
	terrain->GenerateMesh();

	MeshAssetBase* terrainMesh = terrain->GetTerrainMesh();
	int index = terrainMesh->GetWriteIndex();

	// -------------------------------------------------------------------------
	// 2. COPIA, TIPADO Y ENCOLADO SEGURO
	// -------------------------------------------------------------------------

	// VÉRTICES: Copia de la fuente (IVertex) a un shared_ptr.
	// Usamos la versión de GetVertexData que devuelve std::vector<IVertex>.
	const std::vector<uint8_t> vDataLocal = terrainMesh->GetVertexData(index); // Asumimos IVertex
	auto sharedVData = std::make_shared<std::vector<uint8_t>>(vDataLocal);

	// ÍNDICES: Copia de la fuente (uint16_t) a un shared_ptr.
	const std::vector<uint16_t> iDataLocal = terrainMesh->GetIndexData(index);
	auto sharedIData = std::make_shared<std::vector<uint16_t>>(iDataLocal);

	// -------------------------------------------------------------------------
	// 3. ASIGNACIÓN ATÓMICA Y ENCOLADO
	// -------------------------------------------------------------------------
	auto gigaManager = context->bufferManager;

	// El template se tipa como <IVertex> y calcula el tamaño (size() * sizeof(IVertex))
	RingAllocation vAlloc = gigaManager->QueueUpload<uint8_t>(
		gigaManager->GetVertexBufferRing(),
		sharedVData
	);

	// El template se tipa como <uint16_t> y calcula el tamaño (size() * sizeof(uint16_t))
	RingAllocation iAlloc = gigaManager->QueueUpload<uint16_t>(
		gigaManager->GetIndexBufferRing(),
		sharedIData
	);

	// Almacenar la nueva asignación
	terrainMesh->SetVertexAllocation(vAlloc);
	terrainMesh->SetIndexAllocation(iAlloc);

	m_isGenerating = false;
	return true;
}
