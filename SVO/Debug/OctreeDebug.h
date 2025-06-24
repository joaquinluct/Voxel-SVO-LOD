#pragma once
#define NOMINMAX
#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>
#include <unordered_map>
#include "KeyboardManager.h"
#include "Material/Material.h"
#include "../../RenderObjects/UIBox/UIBox.h"
#include "WorldMatrixManager.h"
#include "Camera/FirstPersonCamera.h"
#include "../SVOBase/SVO_Node.h"
#include "../World/World.h"
#include <ManagerLocator/ManagerLocator.h>

using namespace DirectX;

class OctreeDebug: public IUpdatable {
public:
    OctreeDebug(Material* material);
    ~OctreeDebug();

    HRESULT Init(ID3D11Device* device);
    void Render(FirstPersonCamera* camera, DeviceManager* deviceManager, WorldMatrixManager* worldMatrixManager, World* world);
    UIBox* GetOrCreateDebugBox(const XMFLOAT3& origin, float size, const XMFLOAT4& color, ID3D11Device* device);
	void Update(float deltaTime) override;
    void Update(float deltaTime, World* world);
    void Release();
    // Nuevo: Render recursivo con posición y tamaño real
    void RenderNodeRecursive(SVO_Node* node, const XMFLOAT3& origin, float size, int lodLevel, ID3D11Device* device, const XMFLOAT4& colorBase);
    // void SetKeyboardManager(KeyboardManager* keyboardManager) override;

private:
	bool m_isEnabled;
    std::shared_ptr<KeyboardManager> m_keyboardManager;
    std::vector<UIBox*> m_debugBoxes;
    Material* m_material;
};
