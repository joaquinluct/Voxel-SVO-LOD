#include "TerrainDistributionFactory.h"
#include <debugapi.h>
#include <Game/System/TerrainConfig.h>
#include <Game/Systems/Terrain/Defines/TerrainDistributionEngineBase.h>
#include <Locators/TerrainEngineLocator/TerrainEngineLocator.h>
#include <memory>
#include <string>
#include <Windows.h>

HRESULT TerrainDistributionFactory::Init(TerrainConfig* config) {
    if (config == nullptr) {
        return E_FAIL;
    }

    const std::string& engineName = config->terrain_engine;

    if (engineName.empty()) {
        return E_FAIL;
    }

    std::shared_ptr<TerrainDistributionEngineBase> engine = TerrainEngineLocator::GetEngine<TerrainDistributionEngineBase>(engineName);

    if (!engine) {
        OutputDebugStringA(("**TerrainDistributionFactory::Init: No se ha podido obtener el engine de distribución " + engineName + "\n").c_str());
        return E_FAIL;
    }

    engine->Init(config);

    RegisterEngine(engineName, engine.get());

    return S_OK;
}

