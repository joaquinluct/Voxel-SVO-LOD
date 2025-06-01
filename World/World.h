#pragma once
#include "../Voxel/Chunks/ChunkRenderer.h"
#include "../Voxel/Chunks/Octree.h"
#include "../Camera/Camera.h"
#include <vector>
#include <DirectXMath.h>
#include "../Managers/DeviceManager.h"
#include "../Voxel/Chunks/OctreeRenderer.h"

class World {
public:
    World(DeviceManager* deviceManager, Camera* camera);
    ~World();

    HRESULT Init(ID3D11Device* device, Material* chunkMaterial);
    void Update(float deltaTime, Camera* camera);
    void Render(ID3D11DeviceContext* context, Camera* camera);

    ChunkRenderer* GetChunkRenderer() { return m_chunkRenderer; };
    DirectX::XMFLOAT3 GetCameraChunkPosition(Camera* camera) const;
    XMFLOAT3 GetCameraLastPosition() const { return m_cameraLastPosition; };
    void SetCameraLastPosition(XMFLOAT3 position) { m_cameraLastPosition = position; };
    int GetNumChunksOnFrustrum() { return m_chunkRenderer->GetNumChunksOnFrustrum(); };
    int GetNumChunks() { return m_octree->CountChunks(); };
    int GetNumVisibleChunks();
    bool IsChunkInFrustum(const ChunkKey& key, const XMFLOAT4 planes[6]) const;
    Octree* m_octree; // Nueva instancia del Octree

private:
    ChunkRenderer* m_chunkRenderer;
	Camera* m_camera;
	OctreeRenderer* m_octreeRenderer; // Nueva instancia del OctreeRendered
    XMFLOAT3 m_cameraLastPosition;

    float m_renderDistance = 100.0f;
    int m_chunkLoadRadius = 3;

    void LoadVisibleChunks(XMFLOAT3 cameraChunkPos);
    void UnloadFarChunks(XMFLOAT3 cameraChunkPos);
    bool isOnSameChunk(XMFLOAT3 pos1, XMFLOAT3 pos2) const;
};