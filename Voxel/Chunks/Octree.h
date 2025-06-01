#pragma once
#include <vector>
#include <memory>
#include <DirectXMath.h>
#include "Chunk.h"
#include <unordered_map>

using namespace DirectX;

class Octree {
public:
    Octree(const XMFLOAT3& center, float size, int depth);
    ~Octree();

    void Insert(Chunk* chunk);
    void Remove(Chunk* chunk);
    void QueryFrustum(const XMFLOAT4 planes[6], std::unordered_map<ChunkKey, Chunk*>& visibleChunks);
    void QueryChunksOutsideRadius(const XMFLOAT3& center, float radius, std::unordered_map<ChunkKey, Chunk*> chunksToRemove);
    bool IsChunkLoaded(const ChunkKey& key);
    bool IsChunkInNode(const ChunkKey& key) const;
    bool IsChunkInNode(const Chunk* chunk) const;
    //bool IsChunkLoadedInNode(const ChunkKey& key) const;
    int CountChunks() const;
    int CountVisibleChunks(const XMFLOAT4 planes[6]) const;
    std::vector<Chunk*> GetChunks(const XMFLOAT4 planes[6]) const;

private:
    XMFLOAT3 m_center; // Centro del nodo
    float m_size;      // Tamaño del nodo (longitud de un lado del cubo)
    int m_depth;       // Profundidad actual del nodo

	int numVisibleChunks = 0; // Contador de chunks visibles en este nodo

    std::unordered_map<ChunkKey, Chunk*> m_chunks{};
    std::unique_ptr<Octree> m_children[8]; // Hijos del nodo (8 octantes)

    bool IsNodeInFrustum(const XMFLOAT4 planes[6]) const;
    void Subdivide(); // Divide el nodo en 8 subnodos
};