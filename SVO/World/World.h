#pragma once
// Definición de la clase World y utilidades relacionadas.
#define NOMINMAX 
#include <DirectXMath.h>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <vector>
#include <mutex>
#include "VisibleNodeInfo.h"
#include "../SVOBase/SVO_Node.h"
#include "../LOD/LODProcessor.h"
#include "DeviceManager.h"
#include "Camera/FirstPersonCamera.h"
#include "Material/Material.h"
#include "../MarchingCubes/MarchingCubes.h"
#include "../MarchingCubes/MarchingCubesUtil.h"
#include "../../Terrain/Generators/TerrainGeneratorPerling1.h"

// Cada área cubre 1024x1024x1024 unidades
constexpr int AREA_SIZE = 512;
constexpr float AREA_SIZE_F = static_cast<float>(AREA_SIZE);
struct AreaKey {
    int x, y, z;
    bool operator==(const AreaKey& other) const { return x == other.x && y == other.y && z == other.z; }    
    DirectX::XMFLOAT3 GetPosition() {  
        return DirectX::XMFLOAT3(  
            static_cast<float>(x) * AREA_SIZE,
            static_cast<float>(y) * AREA_SIZE,
            static_cast<float>(z) * AREA_SIZE
        );  
    }
    static DirectX::XMFLOAT3 GetPositionFromAreaKey(AreaKey key) {
        return DirectX::XMFLOAT3(
            static_cast<float>(key.x) * AREA_SIZE,
            static_cast<float>(key.y) * AREA_SIZE,
            static_cast<float>(key.z) * AREA_SIZE
        );
	}
};

namespace std {
    template<>
    struct hash<AreaKey> {
        size_t operator()(const AreaKey& k) const {
            return hash<int>()(k.x) ^ (hash<int>()(k.y) << 10) ^ (hash<int>()(k.z) >> 5);
        }
    };
}

class World {
public:
    World(DeviceManager* deviceManager, FirstPersonCamera* camera);
    ~World();
	HRESULT Init(Material* material);
    void Release();
    void Update(float deltaTime);
    void Render(ID3D11DeviceContext* context);

    // Acceso a nodos visibles para renderizado
    const std::vector<VisibleNodeInfo>& GetVisibleNodes() const { return m_visibleNodesRender; };
    //std::unordered_map<AreaKey, std::unique_ptr<SVO_Node>> GetAreas() const { return m_areas; };

    AreaKey GetAreaKeyFromPosition(const DirectX::XMFLOAT3& pos) const;
    SVO_Node* GetOrCreateArea(const AreaKey& key);
    SVO_Node* GetArea(const AreaKey& key);
    MarchingCubesMesh* GetMesh(const AreaKey& key);
    float GetVoxelDensity(const DirectX::XMFLOAT3& worldPos, const SVO_Node* nodeRef, const float nodeSize);

    void SetDepth(int newDepth);


    float CubeSDF(const DirectX::XMFLOAT3& point, const DirectX::XMFLOAT3& boxMin, const DirectX::XMFLOAT3& boxMax);
    float SphereSDF(const DirectX::XMFLOAT3& point, const float nodeSize, const DirectX::XMFLOAT3& center, const float radio);

private:
    const float TERRAIN_HEIGHT_MODIFICATOR = 1.0f;
    const float MAX_HEIGHT = AREA_SIZE_F * TERRAIN_HEIGHT_MODIFICATOR;
    const float MIN_HEIGHT = -1 * AREA_SIZE_F * TERRAIN_HEIGHT_MODIFICATOR;
	int m_depth = 0;

    DeviceManager* m_deviceManager;
	Material* m_material;
    FirstPersonCamera* m_camera;
	TerrainGeneratorPerling1* m_terrainGenerator;

	DirectX::XMFLOAT3 m_lastPosition;

    std::unordered_map<AreaKey, std::unique_ptr<SVO_Node>> m_areas;
    std::unordered_map<AreaKey, std::unique_ptr<MarchingCubesMesh>> m_meshes;
    std::vector<VisibleNodeInfo> m_visibleNodesRender;
    std::vector<VisibleNodeInfo> m_visibleNodesUpdate;
    bool hasNewVisibleNodes = false;
    std::mutex worldMutex;
    LODSettings m_lodSettings;

    // --- Miembros para double buffering del mesh ---
    MarchingCubesMesh m_mainMeshUpdate;
    MarchingCubesMesh m_mainMeshRender;
    DirectX::XMFLOAT3 m_mainOriginUpdate;
    DirectX::XMFLOAT3 m_mainOriginRender;

    void UpdateVisibleNodes();

    float GetDensityFromTerrainNoise(XMFLOAT3 cornerPos);
};
