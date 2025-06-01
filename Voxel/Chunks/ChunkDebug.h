#pragma once

#include <d3d11.h>
#include <vector>
#include <directxmath.h>
#include <unordered_map>
#include "../Chunks/Chunk.h"
#include "../../RenderObjects/UIBox/UIBox.h"
#include "../../Material/Material.h"
#include "../Chunks/ChunkRenderer.h"
#include "../../Managers/WorldMatrixManager.h"
#include "../../World/World.h"

using namespace DirectX;

class ChunkDebug {
public:
    ChunkDebug(Material* material);
    ~ChunkDebug();

    HRESULT Init(ID3D11Device* device);
    void Render(Camera* camera, DeviceManager* deviceManger, WorldMatrixManager* worldMatrixManager, World* word);
    UIBox* GetOrCreateDebugBox(const ChunkKey& key, ID3D11Device* device,
        const DirectX::XMFLOAT3& origin, float sizeX, float sizeY, float sizeZ,
        const DirectX::XMFLOAT4& color);
    void Release();
    XMFLOAT3 GetBox0Origin();

private:
    std::unordered_map<ChunkKey, UIBox*> m_debugBoxes;
    Material* m_material; // Material para las líneas
};