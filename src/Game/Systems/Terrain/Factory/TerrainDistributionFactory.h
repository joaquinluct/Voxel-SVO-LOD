#pragma once

#include <DirectXMath.h>
#include <Game/System/TerrainConfig.h>
#include <Game/Systems/Terrain/Defines/TerrainDistributionEngineBase.h>
#include <Game/Systems/Terrain/Defines/TerrainGeometryEngineBase.h>
#include <iCamera.h>
#include <string>
#include <unordered_map>
#include <Windows.h>

class TerrainDistributionFactory {
public:
    TerrainDistributionFactory() = default;
    ~TerrainDistributionFactory() = default;

    HRESULT Init(TerrainConfig* config);

    void RegisterEngine(const std::string& name, TerrainDistributionEngineBase* engine) {
        m_engines[name] = engine;
    }
    void UpdateTerrain(const std::string& name, const DirectX::XMFLOAT3& worldPosition) {
        const auto& it = m_engines.find(name);
        if (it != m_engines.end()) {
            it->second->UpdateTerrain(worldPosition);
        }
    }

    void Generate(const std::string& name, ICamera* camera) {
        const auto& it = m_engines.find(name);
        if (it != m_engines.end()) {
            it->second->Generate(camera);
        }
    }

    void Render(const std::string& name, ICamera* camera) {
        const auto& it = m_engines.find(name);
        if (it != m_engines.end()) {
            // Intentar hacer cast a TerrainChunkEngine para acceder al método Render
            /*if (auto chunkEngine = dynamic_cast<TerrainGeometryEngineBase*>(it->second)) {
                chunkEngine->Render(camera);
            }
            else {
                OutputDebugStringA("[TerrainDistributionFactory] Engine does not support rendering\n");
            }*/
        }
    }

private:
    std::unordered_map<std::string, TerrainDistributionEngineBase*> m_engines;
};
