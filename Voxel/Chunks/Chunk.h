// Chunk.h
#pragma once
#include <directxmath.h>
#include <vector>
#include <array>
#include <iostream>
#include "../../Util/Utils.h" // Incluye MarchingCubesVertex

using namespace DirectX;

struct ChunkKey {
    int x;
    int y;
    int z;

    // Constructor que acepta coordenadas x, y, z
    ChunkKey(int x, int y, int z) : x(x), y(y), z(z) {}

    // Constructor que acepta un puntero a Chunk
    ChunkKey(const XMFLOAT3 position)
        : x(static_cast<int>(position.x)),
        y(static_cast<int>(position.y)),
        z(static_cast<int>(position.z)) {
    }

    // Operador de igualdad para comparar con otro ChunkKey
    bool operator==(const ChunkKey& other) const {
        return x == other.x && y == other.y && z == other.z;
    }

    // Operador de igualdad para comparar con un puntero a Chunk
    bool operator==(const XMFLOAT3 position) const {
        return x == static_cast<int>(position.x) &&
            y == static_cast<int>(position.y) &&
            z == static_cast<int>(position.z);
    }
};

namespace std {
    template<>
    struct hash<ChunkKey> {
        size_t operator()(const ChunkKey& k) const {
            return hash<int>()(k.x) ^ (hash<int>()(k.y) << 10) ^ (hash<int>()(k.z) >> 5);
        }
    };
};

class Chunk {
public:
    static const int CHUNK_SIZE_X = 16;
    static const int CHUNK_SIZE_Y = 16;
    static const int CHUNK_SIZE_Z = 16;

    Chunk();
    Chunk(int x, int y, int z);

    XMFLOAT3 GetPosition() const;
    void SetPosition(int x, int y, int z);

    bool GetVoxel(int x, int y, int z) const;
    void SetVoxel(int x, int y, int z, bool value);

    bool IsValidVoxelCoordinate(int x, int y, int z) const;

    bool isVirtualChunk() const { return m_isVirtualChunk; };
    void SetVirtualChunk(bool isVirtual) { m_isVirtualChunk = isVirtual; };

    bool NeedsMeshUpdate() const { return m_meshNeedsUpdate; }
    void MarkMeshForUpdate() { m_meshNeedsUpdate = true; }
    void MarkMeshUpdated() { m_meshNeedsUpdate = false; }

    bool IsModified() const { return m_isModified; }
    void MarkModified() { m_isModified = true; }
    void ClearModified() { m_isModified = false; }

	void SetIsInitialized(bool isInitialized) { m_isIinitialized = isInitialized; }
	bool IsInitialized() const { return m_isIinitialized; };

    bool IsBorderChunk() { return m_isBorderedChunk == true; };
    void SetIsBorderChunk(bool isBorderChunk) { m_isBorderedChunk = isBorderChunk; };
    
    void GenerateOnPosition(XMFLOAT3 posistion, bool value);

    ChunkKey GetKey() const { return ChunkKey(m_position); }
    bool IsEmpty() const;
    bool m_voxels[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z]; // Datos del voxel

private:
    XMFLOAT3 m_position;
    bool m_isVirtualChunk;
	bool m_isIinitialized = false;
    bool m_isBorderedChunk = false;

    void Generate(bool value);

    bool m_meshNeedsUpdate;
    bool m_isModified;
};