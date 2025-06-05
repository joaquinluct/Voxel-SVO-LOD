#pragma once
#include <d3d11.h>
#include <vector>
#include <DirectXMath.h>
#include <unordered_map>
#include "../../Material/Material.h"
#include "../../RenderObjects/UIBox/UIBox.h"
#include "../../Managers/WorldMatrixManager.h"
#include "../../Camera/Camera.h"
#include "../SVOBase/SVO_Node.h"
#include "../World/World.h"
#include "../../Managers/KeyboardManager.h"

using namespace DirectX;

class OctreeDebug: public iControllable, iUpdatable {
public:
    OctreeDebug(Material* material, KeyboardManager* keyboardManager);
    ~OctreeDebug();

    HRESULT Init(ID3D11Device* device);
    void Render(Camera* camera, DeviceManager* deviceManager, WorldMatrixManager* worldMatrixManager, World* world);
    UIBox* GetOrCreateDebugBox(const XMFLOAT3& origin, float size, const XMFLOAT4& color, ID3D11Device* device);
	void Update(float deltaTime) override;
    void Update(float deltaTime, World* world);
    void Release();
    // Nuevo: Render recursivo con posición y tamaño real
    void RenderNodeRecursive(SVO_Node* node, const XMFLOAT3& origin, float size, int lodLevel, ID3D11Device* device, const XMFLOAT4& colorBase);
    void SetKeyboardManager(KeyboardManager* keyboardManager) override;

private:
	bool m_isEnabled;
    KeyboardManager* m_keyboardManager;
    std::vector<UIBox*> m_debugBoxes;
    Material* m_material;
};
