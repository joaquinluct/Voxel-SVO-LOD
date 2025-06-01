#pragma once
// Definición de la clase World y utilidades relacionadas.
#include "../SVOBase/SVO_Node.h"
#include "../LOD/LODProcessor.h"
#include <DirectXMath.h>
#include <unordered_map>
#include <memory>
#include <vector>
#include "../../Managers/DeviceManager.h"
#include "../../Camera/Camera.h"
#include "../../Material/Material.h"

// Cada área cubre 1024x1024x1024 unidades
constexpr int AREA_SIZE = 1024;

struct AreaKey {
    int x, y, z;
    bool operator==(const AreaKey& other) const { return x == other.x && y == other.y && z == other.z; }
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
    World(DeviceManager* deviceManager, Camera* camera);
    ~World();
	HRESULT Init(Material* material);
    void Update(float deltaTime);
    void Render(ID3D11DeviceContext* context);

    // Acceso a nodos visibles para renderizado
    const std::vector<VisibleNodeInfo>& GetVisibleNodes() const { return m_visibleNodes; };
    //std::unordered_map<AreaKey, std::unique_ptr<SVO_Node>> GetAreas() const { return m_areas; };

    AreaKey GetAreaKeyFromPosition(const DirectX::XMFLOAT3& pos) const;
    SVO_Node* GetOrCreateArea(const AreaKey& key);
    SVO_Node* GetArea(const AreaKey& key);

private:
    DeviceManager* m_deviceManager;
	Material* m_material;
    Camera* m_camera;
    std::unordered_map<AreaKey, std::unique_ptr<SVO_Node>> m_areas;
    std::vector<VisibleNodeInfo> m_visibleNodes;
    LODSettings m_lodSettings;

    void UpdateVisibleNodes();
};
