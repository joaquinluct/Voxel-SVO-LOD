#include "TerrainGeometryFactory.h"
#include "..\Engines\Geometry\TessellationGeometryEngine.h"
#include "..\Engines\Geometry\AggregatedGeometryEngine.h"
#include <Windows.h>

// Registrar engines estáticos simples (instancia única para demo)
static TessellationGeometryEngine s_tessEngine;
static AggregatedGeometryEngine s_aggEngine;

struct TerrainGeometryFactoryRegistrator {
	TerrainGeometryFactoryRegistrator() {
		// Registrar en el factory global (si existe uno)
		// Nota: Como TerrainGeometryFactory no es singleton, esto es solo un ejemplo.
		OutputDebugStringA("[TerrainGeometryFactory] Static registrator executed\n");
	}
};
static TerrainGeometryFactoryRegistrator s_registrator;

