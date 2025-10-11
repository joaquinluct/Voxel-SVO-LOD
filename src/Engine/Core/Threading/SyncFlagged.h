#pragma once

#include <bitset>
#include <condition_variable>
#include <Defines/Contants/Flags/SyncFlag.h>
#include <mutex>

class SyncFlagged {
protected:
    mutable std::mutex m_flagMutex;
    std::condition_variable m_flagCondition;
    std::bitset<static_cast<size_t>(SyncFlagIndex::Count)> m_flags;

public:
    void SetFlag(SyncFlagIndex index, bool value) {
        {
            std::lock_guard<std::mutex> lock(m_flagMutex);
            m_flags.set(static_cast<size_t>(index), value);
        }
        m_flagCondition.notify_all();
    }

    bool GetFlag(SyncFlagIndex index) const {
        std::lock_guard<std::mutex> lock(m_flagMutex);
        return m_flags.test(static_cast<size_t>(index));
    }

    bool WaitForFlag(SyncFlagIndex index, bool expected, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(m_flagMutex);
        return m_flagCondition.wait_for(lock, timeout, [&] {
            return m_flags.test(static_cast<size_t>(index)) == expected;
            });
    }
};
