#include "OctreeDebug.h"
#include <algorithm>

OctreeDebug::OctreeDebug(Material* material) : m_material(material) {}

OctreeDebug::~OctreeDebug() {
    Release();
}

HRESULT OctreeDebug::Init(ID3D11Device* device) {
    return S_OK;
}

void OctreeDebug::Release() {
    for (auto* box : m_debugBoxes) {
        delete box;
    }
    m_debugBoxes.clear();
}

static bool XMFLOAT3Equal(const XMFLOAT3& a, const XMFLOAT3& b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

UIBox* OctreeDebug::GetOrCreateDebugBox(const XMFLOAT3& origin, float size, const XMFLOAT4& color, ID3D11Device* device) {
    auto it = std::find_if(m_debugBoxes.begin(), m_debugBoxes.end(), [&](UIBox* box) {
        return XMFLOAT3Equal(box->GetOrigin(), origin) && box->GetSize().x == size;
    });
    if (it != m_debugBoxes.end()) {
        (*it)->SetColor(color);
        return *it;
    }
    UIBox* box = new UIBox(m_material, origin, size, size, size, color);
    box->Init(device);
    m_debugBoxes.push_back(box);
    return box;
}

void OctreeDebug::Render(Camera* camera, DeviceManager* deviceManager, WorldMatrixManager* worldMatrixManager, World* world) {
    Release();
    const auto& visibleNodes = world->GetVisibleNodes();
    // Para cada nodo visible, calcular su posición y tamaño real

    for (const VisibleNodeInfo& visibleNodeInfo : visibleNodes) {
        SVO_Node* node = visibleNodeInfo.node; // Assuming VisibleNodeInfo has a member `node` of type SVO_Node*
        if (node) {
            // Suponemos que cada nodo tiene asociado su origen y tamaño
            XMFLOAT3 origin = {0, 0, 0};
            float size = 1024.0f;
            int lodLevel = 0; // Si tienes info de LOD, úsala aquí
            XMFLOAT4 color = {1.0f, 1.0f, 0.0f, 1.0f};
            color.x *= (1.0f - 0.2f * lodLevel); // Más oscuro según LOD
            UIBox* box = GetOrCreateDebugBox(origin, size, color, deviceManager->GetDevice());
            box->Render(deviceManager->GetContext());
        }
    }        
    
    // Si quieres mostrar el árbol completo, llama a RenderNodeRecursive para cada área
    
    /*for (const auto& areaPair : world->GetAreas()) {
        XMFLOAT3 areaOrigin = {
            static_cast<float>(areaPair.first.x) * AREA_SIZE,
            static_cast<float>(areaPair.first.y) * AREA_SIZE,
            static_cast<float>(areaPair.first.z) * AREA_SIZE
        };
        RenderNodeRecursive(areaPair.second.get(), areaOrigin, AREA_SIZE, 0, deviceManager->GetDevice(), {0.0f, 1.0f, 0.0f, 0.5f});
    }*/
    
}

void OctreeDebug::RenderNodeRecursive(SVO_Node* node, const XMFLOAT3& origin, float size, int lodLevel, ID3D11Device* device, const XMFLOAT4& colorBase) {
    if (!node) return;
    XMFLOAT4 color = colorBase;
    color.x *= (1.0f - 0.2f * lodLevel);
    UIBox* box = GetOrCreateDebugBox(origin, size, color, device);
    // Render recursivo para hijos
    if (!node->IsLeaf()) {
        float childSize = size / 2.0f;
        for (int i = 0; i < 8; ++i) {
            if (node->HasChild(i)) {
                XMFLOAT3 childOrigin = origin;
                if (i & 1) childOrigin.x += childSize;
                if (i & 2) childOrigin.y += childSize;
                if (i & 4) childOrigin.z += childSize;
                RenderNodeRecursive(node->GetChild(i), childOrigin, childSize, lodLevel + 1, device, colorBase);
            }
        }
    }
}
