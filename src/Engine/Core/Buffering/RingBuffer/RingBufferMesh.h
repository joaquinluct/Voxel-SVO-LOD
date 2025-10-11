#pragma once
#include <Buffering/RingBuffer/Defines/RingBufferBase.h>
#include <cstdint>
#include <vector>

template<typename VertexType, typename IndexType>
class RingBufferMesh : public RingBufferBase {
public:
    struct MeshFrameData {
        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
        std::vector<VertexType> vertices;
        std::vector<IndexType> indices;
        size_t vertexSize = sizeof(VertexType);
        size_t indexSize = sizeof(IndexType);
    };

private:
    std::vector<MeshFrameData> m_meshFrames;

public:
    RingBufferMesh(uint32_t frameCount)
        : RingBufferBase(frameCount), m_meshFrames(frameCount) {
    }

    MeshFrameData& GetCurrentMesh() {
        return m_meshFrames[m_currentIndex];
    }

    void SetMesh(uint32_t index, const MeshFrameData& mesh) {
        if (index < m_frameCount) {
            m_meshFrames[index] = mesh;
        }
    }
};
