#pragma once

#include <windows.h>
#include <vector>
#include <Defines/TerrainChunk.h>

using namespace TerrainChunk;

class ChunkBufferAllocator {
public:
    ChunkBufferAllocator():
        m_maxVertexCount(0),
        m_maxIndexCount(0),
        m_currentVertexOffset(0),
		m_currentIndexOffset(0)
    {};
    ChunkBufferAllocator(UINT maxVertexCount, UINT maxIndexCount);

    ChunkBufferRegion Allocate(UINT vertexCount, UINT indexCount);
    void Compact(std::function<void(const ChunkBufferRegion& oldRegion, ChunkBufferRegion& newRegion)> onRegionMoved);
    float FragmentationRatio() const;
    void Free(const ChunkBufferRegion& region);
    void Reset();
    const std::vector<ChunkBufferRegion>& GetActiveRegions() const;

private:
    UINT m_maxVertexCount;
    UINT m_maxIndexCount;
    UINT m_currentVertexOffset;
    UINT m_currentIndexOffset;

    std::vector<ChunkBufferRegion> m_regions;
    std::vector<ChunkBufferRegion> m_freeList;
};