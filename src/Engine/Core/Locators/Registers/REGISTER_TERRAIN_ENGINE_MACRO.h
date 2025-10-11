#pragma once
#include <Game/Systems/Terrain/Defines/TerrainEngineBase.h>
#include <Locators/TerrainEngineLocator/TerrainEngineLocator.h>

#define REGISTER_TERRAIN_ENGINE_TYPE(TerrainEngineClassName, TerrainEngineNameString) \
    namespace { \
        class TerrainEngine ## TerrainEngineClassName ## Registrar { \
        public: \
            TerrainEngine ## TerrainEngineClassName ## Registrar() { \
                TerrainEngineLocator::RegisterTerrainEngineCreator( \
                    TerrainEngineNameString, \
                    /* Lambda de creación: Devuelve std::shared_ptr<TerrainEngineClassName> que se convierte implícitamente a std::shared_ptr<ITerrainEngine> */ \
                    []() -> std::shared_ptr<TerrainEngineBase> {  \
                        return std::make_shared<TerrainEngineClassName>(); \
                    } \
                ); \
            } \
        }; \
        static TerrainEngine ## TerrainEngineClassName ## Registrar s_register_ ## TerrainEngineClassName; \
    }
