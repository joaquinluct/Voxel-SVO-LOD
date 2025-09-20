#pragma once
#include <atomic>
#include <Core/Threading/SyncFlagged.h>
#include <thread>
#include <unordered_map>

class ThreadedService : public SyncFlagged {
protected:
    std::thread m_thread;
    std::atomic<bool> m_running = false;

public:
    virtual ~ThreadedService() {
        Stop();
    }

    void Start() {
        m_running = true;
        m_thread = std::thread([this] { RunLoop(); });
    }

    void Stop() {
        m_running = false;
        if (m_thread.joinable()) m_thread.join();
    }

    void Pause() {
        m_running = false;
    }
    void Resume() {
        m_running = true;
    }

    virtual void RunLoop() {};

    bool IsRunning() const { return m_running.load(); }
};
