#pragma once
#include <atomic>
#include <Core/Threading/SyncFlagged.h>
#include <thread>
#include <unordered_map>

// Deprecated: ThreadedService helper kept for reference. After migration
// managers should not spawn dedicated threads; main Engine loop will
// call their Update()/Render() methods. This file remains to avoid
// breaking includes but contains no active threading primitives.
class ThreadedService : public SyncFlagged {
protected:
    std::thread m_thread;
    std::atomic<bool> m_running = false;

public:
    virtual ~ThreadedService() {
        Stop();
    }

    void Start() {
        // NOTE: For AAA threading refactor we disable automatic creation
        // of dedicated threads inside ThreadedService::Start().
        // This allows a safe, reversible migration to the model where
        // the main thread runs game logic and only a dedicated render
        // thread exists. To rollback simply run: `git checkout .`.
        m_running = true;
        // m_thread = std::thread([this] { RunLoop(); });
        // If you need to re-enable per-manager threads, uncomment the
        // line above. Prefer creating explicit threads from Engine
        // (or another orchestrator) instead of relying on Start().
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
