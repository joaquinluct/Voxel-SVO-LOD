#pragma once
#include <cstdint>

class RingBufferBase {
protected:
    uint32_t m_frameCount = 0;
    uint32_t m_currentIndex = 0;

public:
    RingBufferBase(uint32_t frameCount)
        : m_frameCount(frameCount), m_currentIndex(0) {
    }

    virtual ~RingBufferBase() = default;

    void Advance() {
        m_currentIndex = (m_currentIndex + 1) % m_frameCount;
    }

    uint32_t GetCurrentIndex() const {
        return m_currentIndex;
    }

    uint32_t GetFrameCount() const {
        return m_frameCount;
    }
};

