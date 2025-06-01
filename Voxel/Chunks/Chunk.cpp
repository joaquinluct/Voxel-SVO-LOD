// Chunk.cpp
#include "Chunk.h"
#include <iostream>
#include "ChunkManager.h" // Asegúrate de incluir esto si no está ya incluido

Chunk::Chunk() : m_position(0, 0, 0), m_isVirtualChunk(true), m_meshNeedsUpdate(true), m_isModified(true) {
    Generate(false);
}

Chunk::Chunk(int x, int y, int z) : m_position(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z)), m_isVirtualChunk(true), m_meshNeedsUpdate(true), m_isModified(true) {
    Generate(false);
}

XMFLOAT3 Chunk::GetPosition() const { return m_position; }
void Chunk::SetPosition(int x, int y, int z) {
    m_position.x = static_cast<float>(x);
    m_position.y = static_cast<float>(y);
    m_position.z = static_cast<float>(z);
}

bool Chunk::GetVoxel(int x, int y, int z) const {
    if (IsValidVoxelCoordinate(x, y, z)) {
        return m_voxels[x][y][z];
    }
    else {
        //std::cerr << "Voxel coordinate out of bounds: (" << x << ", " << y << ", " << z << ")" << std::endl;
        return false;
    }
}

void Chunk::SetVoxel(int x, int y, int z, bool value) {
    if (IsValidVoxelCoordinate(x, y, z)) {
        if (m_voxels[x][y][z] != value) {
            m_voxels[x][y][z] = value;
            MarkMeshForUpdate();
            MarkModified();
        }
    }
    else {
        std::cerr << "Voxel coordinate out of bounds: (" << x << ", " << y << ", " << z << ")" << std::endl;
    }
}

bool Chunk::IsValidVoxelCoordinate(int x, int y, int z) const {
    return x >= 0 && x < Chunk::CHUNK_SIZE_X && y >= 0 && y < Chunk::CHUNK_SIZE_Y && z >= 0 && z < Chunk::CHUNK_SIZE_Z;
}

void Chunk::Generate(bool value) {
    std::fill(&m_voxels[0][0][0], &m_voxels[0][0][0] + (CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z), value);

    MarkMeshForUpdate();
    MarkModified();
}

void Chunk::GenerateOnPosition(XMFLOAT3 posistion, bool value) {
    std::fill(&m_voxels[0][0][0], &m_voxels[0][0][0] + (CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z), value);

    MarkMeshForUpdate();
    MarkModified();
}

bool Chunk::IsEmpty() const {
    const bool* voxelData = &m_voxels[0][0][0];
    const bool* end = voxelData + (CHUNK_SIZE_X * CHUNK_SIZE_Y * CHUNK_SIZE_Z);

    while (voxelData != end) {
        if (*voxelData) {
            return false;
        }
        ++voxelData;
    }
    return true;
}