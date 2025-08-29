#include "ProceduralService.h"
#include "Procedural/Engines/FastNoiseLiteEngine.h"
#include "Procedural/Engines/ProceduralEngineBase.h"
#include <Defines/WorldTerrain.h>
#include <REGISTER_SERVICE_MACRO.h> // Tu macro para registrar servicios

// Registrar el servicio
REGISTER_SERVICE_TYPE(ProceduralService, "ProceduralService")

HRESULT ProceduralService::Init(WorldTerrain::TerrainProceduralDefinition definition)
{
	m_def = std::make_shared<WorldTerrain::TerrainProceduralDefinition>(definition);

	m_engineType = static_cast<Procedural::ProceduralEngine>(m_def->engine);

	switch (m_engineType) {
	case Procedural::ProceduralEngine::FastNoiseLite: {
		// Opción 1: Crear el objeto directamente con make_shared
		m_engine = std::make_shared<FastNoiseLiteEngine>();

		// Opción 2: Usar un puntero crudo y asignarlo al shared_ptr
		m_engine.reset(new FastNoiseLiteEngine());

		//m_engine = std::make_shared<ProceduralEngineBase>(new FastNoiseLiteEngine());
		m_engine->Init(m_def->seed, definition);
		break;
	}
	}

	return S_OK;
}
