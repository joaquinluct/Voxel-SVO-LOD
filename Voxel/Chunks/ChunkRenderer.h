// ChunkRenderer.h
#pragma once
#include <d3d11.h>
#include <vector>
#include <directxmath.h>
#include "../../Camera/Camera.h"
#include "../MarchingCubes/MarchingCubes.h"
#include "../../Managers/DeviceManager.h" // Para DeviceManager
#include "../../Material/Material.h"
#include "../VoxelMesh/VoxelMesh.h"
#include "ChunkManager.h"
#include "Octree.h"

using namespace DirectX;

class ChunkRenderer {
public:
    ChunkRenderer(DeviceManager* deviceMananger, ChunkManager* chunkManager, Material* chunkMaterial);
    ~ChunkRenderer();

    void SetChunkMaterial(Material* chunkMaterial) { m_chunkMaterial = chunkMaterial; }
    HRESULT Init(ID3D11Device* device);
    // Render toma ID3D11DeviceContext* directamente, y las matrices separadas
    void Render(Camera* camera, ID3D11DeviceContext* context, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix);
    void Release();

    void ReleaseChunkMesh(const ChunkKey& key);

    ChunkManager* GetChunkManager() const { return m_chunkManager; };

    int GetNumChunksOnFrustrum() { return static_cast<int>(m_chunkMeshesOnFrustrum.size()); };
    bool IsChunkInFrustum(const XMFLOAT3& chunkPosition, float chunkSize, const XMFLOAT4 planes[6]);

private:
	DeviceManager* m_deviceManager;
    ChunkManager* m_chunkManager;
    MarchingCubes m_marchingCubes;
    Octree m_octree;

    std::unordered_map<ChunkKey, VoxelMesh*> m_chunkMeshes;
    std::unordered_map<ChunkKey, Chunk*> m_chunkMeshesOnFrustrum;
    Material* m_chunkMaterial;

    std::vector<VoxelMesh*> GetChunkNeighbors(const ChunkKey& chunkKey);

};
