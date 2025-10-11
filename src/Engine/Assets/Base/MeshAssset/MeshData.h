#pragma once
#include <cstdint>
#include <vector>

template<typename TVertex, typename TIndex = uint32_t>
class MeshData {
public:
    std::vector<TVertex> vertices;
    std::vector<TIndex> indices;

    void Clear() {
        vertices.clear();
        indices.clear();
    }

    size_t GetVertexCount() const {
        return vertices.size();
    }

    size_t GetIndexCount() const {
        return indices.size();
    }

    const TVertex* GetVertexData() const {
        return vertices.data();
    }

    const TIndex* GetIndexData() const {
        return indices.data();
    }
};
