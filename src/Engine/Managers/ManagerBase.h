#pragma once
#include <Core/Threading/ThreadedService.h>
#include <Core/Threading/SyncFlagged.h>
#include <Defines/EngineDefinition.h>
#include <IEngineLifeCycle.h>
#include <IManager.h>
#include <windows.h>


class ManagerBase : public IManager, public IEngineLifeCycle, public SyncFlagged
{
protected:
    EngineContext* m_context = nullptr;
    // Internal thread helper kept for API compatibility. Managers no longer
    // inherit from ThreadedService; thread ownership is centralized in Engine.
    ThreadedService m_threadService;
    // Backwards-compatible running flag used by many managers.
    std::atomic<bool> m_running{false};
public:
    ManagerBase() = default;
    virtual ~ManagerBase() = default;
    virtual HRESULT Init(EngineContext* context) override {
        m_context = context;
        return S_OK;
    }
    bool IsRunnig() const { return !m_context ? false : m_context->isRunning; }
    bool IsInitialized() const { return !m_context ? false : m_context->isInitialized; }
    void SetInitialized(bool initialized) { if (m_context) m_context->isInitialized = initialized; }

    EngineContext* Context() const { return m_context; }

    // Backwards-compatible RunLoop hook. Managers may still override this
    // method during the migration; Engine will not start per-manager threads
    // by default in the AAA model.
    virtual void RunLoop() {}

    // Thread control API (delegated). Kept to avoid changing callers while
    // moving to AAA threading model where Engine orchestrates threads.
    void Start() { m_running = true; m_threadService.Start(); }
    void Stop() { m_running = false; m_threadService.Stop(); }
    void Pause() { m_running = false; m_threadService.Pause(); }
    void Resume() { m_running = true; m_threadService.Resume(); }
    bool IsRunning() const { return m_running.load(); }

};
