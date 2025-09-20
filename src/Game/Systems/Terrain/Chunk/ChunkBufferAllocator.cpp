#include "ChunkBufferAllocator.h"

ChunkBufferAllocator::ChunkBufferAllocator(UINT maxVertexCount, UINT maxIndexCount)
    : m_maxVertexCount(maxVertexCount),
    m_maxIndexCount(maxIndexCount),
    m_currentVertexOffset(0),
    m_currentIndexOffset(0) {
}

ChunkBufferRegion ChunkBufferAllocator::Allocate(UINT vertexCount, UINT indexCount) {
    for (auto it = m_freeList.begin(); it != m_freeList.end(); ++it) {
        if (it->vertexCount >= vertexCount && it->indexCount >= indexCount) {
            ChunkBufferRegion region = *it;
            region.active = true;
            m_regions.push_back(region);
            m_freeList.erase(it);
            return region;
        }
    }

    if (m_currentVertexOffset + vertexCount > m_maxVertexCount ||
        m_currentIndexOffset + indexCount > m_maxIndexCount) {
        return {}; // Sin espacio
    }

    ChunkBufferRegion region;
    region.vertexOffset = m_currentVertexOffset;
    region.indexOffset = m_currentIndexOffset;
    region.vertexCount = vertexCount;
    region.indexCount = indexCount;
    region.active = true;

    m_currentVertexOffset += vertexCount;
    m_currentIndexOffset += indexCount;

    m_regions.push_back(region);
    return region;
}

void ChunkBufferAllocator::Compact(std::function<void(const ChunkBufferRegion& oldRegion, ChunkBufferRegion& newRegion)> onRegionMoved) {
    std::vector<ChunkBufferRegion> compacted;
    UINT newVertexOffset = 0;
    UINT newIndexOffset = 0;

    for (auto& region : m_regions) {
        if (!region.active)
            continue;

        ChunkBufferRegion newRegion;
        newRegion.vertexOffset = newVertexOffset;
        newRegion.indexOffset = newIndexOffset;
        newRegion.vertexCount = region.vertexCount;
        newRegion.indexCount = region.indexCount;
        newRegion.active = true;

        // Callback para que el motor actualice el chunk
        onRegionMoved(region, newRegion);

        compacted.push_back(newRegion);

        newVertexOffset += newRegion.vertexCount;
        newIndexOffset += newRegion.indexCount;
    }

    m_regions = std::move(compacted);
    m_freeList.clear();
    m_currentVertexOffset = newVertexOffset;
    m_currentIndexOffset = newIndexOffset;
}

float ChunkBufferAllocator::FragmentationRatio() const {
    UINT freeVertex = 0;
    UINT freeIndex = 0;

    for (const auto& region : m_freeList) {
        freeVertex += region.vertexCount;
        freeIndex += region.indexCount;
    }

    UINT totalVertex = m_currentVertexOffset;
    UINT totalIndex = m_currentIndexOffset;

    float vertexRatio = totalVertex > 0 ? (float)freeVertex / totalVertex : 0.0f;
    float indexRatio = totalIndex > 0 ? (float)freeIndex / totalIndex : 0.0f;

    return (std::max)(vertexRatio, indexRatio);
}


void ChunkBufferAllocator::Free(const ChunkBufferRegion& region) {
    for (auto& r : m_regions) {
        if (r.vertexOffset == region.vertexOffset && r.indexOffset == region.indexOffset) {
            r.active = false;
            m_freeList.push_back(r);
            break;
        }
    }
}

void ChunkBufferAllocator::Reset() {
    m_regions.clear();
    m_freeList.clear();
    m_currentVertexOffset = 0;
    m_currentIndexOffset = 0;
}

const std::vector<ChunkBufferRegion>& ChunkBufferAllocator::GetActiveRegions() const {
    return m_regions;
}
