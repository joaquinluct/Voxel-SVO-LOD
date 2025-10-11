#pragma once
#include <Buffering/RingBuffer/Defines/RingBufferBase.h>
#include <d3d11.h>
#include <vector>
#include <wrl/client.h>

class RingBufferGPU : public RingBufferBase {
public:
    struct GPUFrameBuffers {
        Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
    };

private:
    std::vector<GPUFrameBuffers> m_buffers;

public:
    RingBufferGPU(uint32_t frameCount)
        : RingBufferBase(frameCount), m_buffers(frameCount) {
    }

    GPUFrameBuffers& GetCurrentBuffers() {
        return m_buffers[m_currentIndex];
    }

    void SetBuffers(uint32_t index, const GPUFrameBuffers& buffers) {
        if (index < m_frameCount) {
            m_buffers[index] = buffers;
        }
    }
};
