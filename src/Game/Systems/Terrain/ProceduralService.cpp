#include "Procedural/Engines/DoubleLayerNoiseEngine.h"
#include "Procedural/Engines/FastNoiseLiteEngine.h"
#include "Procedural/Engines/ProceduralBiomaEngine.h"
#include "Procedural/Engines/ProceduralEngineBase.h"
#include "ProceduralService.h"
#include <Defines/Procedural.h>
#include <Defines/WorldTerrain.h>
#include <memory>
#include <REGISTER_SERVICE_MACRO.h> // Tu macro para registrar servicios

// Registrar el servicio
REGISTER_SERVICE_TYPE(ProceduralService, "ProceduralService")

HRESULT ProceduralService::Init(WorldTerrain::TerrainProceduralDefinition definition)
{
	m_def = std::make_shared<WorldTerrain::TerrainProceduralDefinition>(definition);

	m_engineType = static_cast<Procedural::ProceduralEngine>(m_def->engine);

	switch (m_engineType) {
	case Procedural::ProceduralEngine::FastNoiseLite: {
		m_engine.reset(new FastNoiseLiteEngine());

		//m_engine = std::make_shared<ProceduralEngineBase>(new FastNoiseLiteEngine());
		break;
	}
	case Procedural::ProceduralEngine::DoubleLayerNoise: {
		m_engine.reset(new DoubleLayerNoiseEngine());
		break;
	}
	case Procedural::ProceduralEngine::BiomeMapping: {
		m_engine.reset(new ProceduralBiomaEngine());
		break;
	}
	}
	m_engine->Init(m_def->seed, definition);

	return S_OK;
}
