// ChunkManager.h
#pragma once
#include <string>
#include <DirectXMath.h> // For XMFLOAT3
#include <unordered_map>
#include <vector>
#include <d3d11.h> // Para ID3D11Device*
#include "Chunk.h" // Incluye la definici�n de Chunk y ChunkKey

class ChunkManager {
public:
    ChunkManager();
    ~ChunkManager();

    void AddChunk(int x, int y, int z, Chunk* chunk);

    Chunk* GetChunk(int x, int y, int z);
    void RemoveChunk(int x, int y, int z);
    void UnloadChunk(int x, int y, int z, bool saveToDisk = false);
    bool ChunkExists(int x, int y, int z);
    void LoadChunk(int x, int y, int z);
    bool LoadChunkFromDisk(Chunk* chunk);
	bool HasChunks() const { return !m_chunks.empty(); }
    void SaveChunkToDisk(Chunk* chunk);
    float GetDensity(DirectX::XMFLOAT3 globalPosition);
    void GenerateDefaultChunk(Chunk& chunk);

    std::unordered_map<ChunkKey, Chunk*>& GetChunks();

    void SetDevice(ID3D11Device* device) { m_device = device; }
    ID3D11Device* GetDevice() const { return m_device; }

    Chunk* GetNeighborChunk(int x, int y, int z, int dx, int dy, int dz); // Nuevo método para obtener chunks vecinos

private:
    std::unordered_map<ChunkKey, Chunk*> m_chunks{};
    ID3D11Device* m_device; // Puntero al dispositivo D3D11
};
