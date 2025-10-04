#pragma once
#include <Core/Threading/ThreadedService.h>
#include <Defines//Context/EngineContext.h>
#include <IEngineLifeCycle.h>
#include <IManager.h>
#include <windows.h>


class ManagerBase : public IManager, public IEngineLifeCycle, public ThreadedService
{
protected:
    EngineContext* m_context = nullptr;
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
};
