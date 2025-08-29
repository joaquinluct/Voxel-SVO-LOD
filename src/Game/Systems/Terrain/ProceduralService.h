#pragma once

#include <functional>
#include <memory>
#include <IService.h>
#include <IProceduralEngine.h>
#include <Defines/Procedural.h>
//#include <Defines/WorldTerrain.h>

namespace WorldTerrain {
    // Declaración anticipada de la struct anidada
    struct TerrainProceduralDefinition;
}
//namespace Procedural {
//    // Declaración anticipada de la struct anidada
//    struct ProceduralEngine;
//}

class ProceduralEngineBase;

class ProceduralService : public IService
{
private:
    std::shared_ptr<WorldTerrain::TerrainProceduralDefinition> m_def;
    Procedural::ProceduralEngine m_engineType;
    std::shared_ptr<ProceduralEngineBase> m_engine;
public:
    ProceduralService() : IService(), m_def{}, m_engine{}, m_engineType{} {};
    ~ProceduralService() override {}
    HRESULT Init() override { return S_OK; }
    HRESULT Init(WorldTerrain::TerrainProceduralDefinition definition);
    void Update(float deltaTime) override {}
    void Render() override {}
    void Shutdown() override {}

    // Implementación de IService
    const std::string& GetServiceName() const override {
        static const std::string name = "ProceduralService";
        return name;
    }
    static const std::string& GetStaticServiceName() {
        static const std::string name = "ProceduralService";
        return name;
    }

    std::shared_ptr<ProceduralEngineBase> GetEngine() const {
        return m_engine;
	}
};
