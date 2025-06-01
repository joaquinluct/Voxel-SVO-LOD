// ChunkManager.cpp
#include "ChunkManager.h"
#include <iostream>
#include <fstream>
#include <algorithm> // Para std::remove_if

ChunkManager::ChunkManager() : m_device(nullptr) { // Inicializa m_device a nullptr
    m_chunks.clear();
    m_chunks.reserve(1000);
}

ChunkManager::~ChunkManager() {
    for (auto& pair : m_chunks) {
        delete pair.second;
    }
    m_chunks.clear();
}

void ChunkManager::AddChunk(int x, int y, int z, Chunk* chunk) {
    ChunkKey key(x, y, z);
    if (m_chunks.find(key) == m_chunks.end()) {
        m_chunks[key] = chunk;
    }
    else {
        std::cerr << "Warning: Chunk already exists at " << x << ", " << y << ", " << z << ". Not adding duplicate." << std::endl;
        // Considerar si delete chunk es apropiado aqu� o si el caller es responsable de no a�adir duplicados.
        // Por ahora, solo evito la adici�n si ya existe.
    }
}

Chunk* ChunkManager::GetChunk(int x, int y, int z) {
    ChunkKey key(x, y, z);
    auto it = m_chunks.find(key);
    if (it != m_chunks.end()) {
        return it->second;
    }
    return nullptr;
}

void ChunkManager::RemoveChunk(int x, int y, int z) {
    ChunkKey key(x, y, z);
    auto it = m_chunks.find(key);
    if (it != m_chunks.end()) {
        delete it->second;
        m_chunks.erase(it);
    }
}

bool ChunkManager::ChunkExists(int x, int y, int z) {
    ChunkKey key(x, y, z);
    return m_chunks.count(key) > 0;
}

bool ChunkManager::LoadChunkFromDisk(Chunk* chunk) {
    std::string filename = "chunks/chunk_" +
        std::to_string(static_cast<int>(chunk->GetPosition().x)) + "_" +
        std::to_string(static_cast<int>(chunk->GetPosition().y)) + "_" +
        std::to_string(static_cast<int>(chunk->GetPosition().z)) + ".chunk";
    std::ifstream file(filename, std::ios::binary);
    if (file.is_open()) {
        file.read(reinterpret_cast<char*>(&chunk->m_voxels), sizeof(bool) * Chunk::CHUNK_SIZE_X * Chunk::CHUNK_SIZE_Y * Chunk::CHUNK_SIZE_Z);
        file.close();
        chunk->ClearModified();
        chunk->MarkMeshForUpdate();
        return true;
    }
    return false;
}

void ChunkManager::SaveChunkToDisk(Chunk* chunk) {
    std::string filename = "chunks/chunk_" +
        std::to_string(static_cast<int>(chunk->GetPosition().x)) + "_" +
        std::to_string(static_cast<int>(chunk->GetPosition().y)) + "_" +
        std::to_string(static_cast<int>(chunk->GetPosition().z)) + ".chunk";
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(&chunk->m_voxels), sizeof(bool) * Chunk::CHUNK_SIZE_X * Chunk::CHUNK_SIZE_Y * Chunk::CHUNK_SIZE_Z);
        file.close();
        chunk->ClearModified();
    }
    else {
        std::cerr << "Error saving chunk: " << filename << std::endl;
    }
}

void ChunkManager::GenerateDefaultChunk(Chunk& chunk) {
    for (int x = 0; x < Chunk::CHUNK_SIZE_X; ++x) {
        for (int y = 0; y < Chunk::CHUNK_SIZE_Y; ++y) {
            for (int z = 0; z < Chunk::CHUNK_SIZE_Z; ++z) {
                chunk.SetVoxel(x, y, z, false);
            }
        }
    }
    chunk.MarkModified();
    chunk.MarkMeshForUpdate();
}

void ChunkManager::LoadChunk(int x, int y, int z) {
    if (ChunkExists(x, y, z)) return;

    Chunk* newChunk = new Chunk(x, y, z);

    if (!LoadChunkFromDisk(newChunk)) {
        newChunk->SetVirtualChunk(true);
        newChunk->MarkMeshUpdated();
        GenerateDefaultChunk(*newChunk);
        newChunk->MarkModified();
    }
    else {
        newChunk->SetVirtualChunk(false);
    }
    m_chunks[ChunkKey(x, y, z)] = newChunk;
}

float ChunkManager::GetDensity(XMFLOAT3 globalPosition) {
    ChunkKey key(static_cast<int>(floor(globalPosition.x / Chunk::CHUNK_SIZE_X)),
        static_cast<int>(floor(globalPosition.y / Chunk::CHUNK_SIZE_Y)),
        static_cast<int>(floor(globalPosition.z / Chunk::CHUNK_SIZE_Z)));

    auto it = m_chunks.find(key);

    int localX = static_cast<int>(globalPosition.x) % Chunk::CHUNK_SIZE_X;
    int localY = static_cast<int>(globalPosition.y) % Chunk::CHUNK_SIZE_Y;
    int localZ = static_cast<int>(globalPosition.z) % Chunk::CHUNK_SIZE_Z;

    if (localX < 0) localX += Chunk::CHUNK_SIZE_X;
    if (localY < 0) localY += Chunk::CHUNK_SIZE_Y;
    if (localZ < 0) localZ += Chunk::CHUNK_SIZE_Z;

    if (it != m_chunks.end()) {
        Chunk* chunk = it->second;
        if (chunk->isVirtualChunk()) {
            if (chunk->GetPosition().y >= -2 && chunk->GetPosition().y <= 2 && localY >= -2 && localY <= 2) {
                return 1.0f;
            }
            else {
                return 0.0f;
            }
        }
        else {
            return chunk->GetVoxel(localX, localY, localZ) ? 1.0f : 0.0f;
        }
    }
    else {
        return 0.0f;
    }
}

std::unordered_map<ChunkKey, Chunk*>& ChunkManager::GetChunks() {
    return m_chunks;
}

void ChunkManager::UnloadChunk(int x, int y, int z, bool saveToDisk) {
    ChunkKey key(x, y, z);
    auto it = m_chunks.find(key);
    if (it != m_chunks.end()) {
        Chunk* chunk = it->second;
        if (!chunk->IsEmpty() && (saveToDisk || (chunk->IsModified()))) {
            SaveChunkToDisk(chunk);
        }

        delete chunk;
        m_chunks.erase(it);
    }
}

Chunk* ChunkManager::GetNeighborChunk(int x, int y, int z, int dx, int dy, int dz) {
    int neighborX = x + dx;
    int neighborY = y + dy;
    int neighborZ = z + dz;

    Chunk* neighborChunk = GetChunk(neighborX, neighborY, neighborZ);
    if (neighborChunk) {
        return neighborChunk;
    }

    return nullptr;

    // Si el chunk vecino no está cargado, devolver un chunk virtual con todos los voxeles en 'false'
    //Chunk* virtualChunk = new Chunk(neighborX, neighborY, neighborZ);
    //virtualChunk->SetVirtualChunk(true);
    //GenerateDefaultChunk(*virtualChunk); // Generar un chunk vacío
    //return virtualChunk;
}