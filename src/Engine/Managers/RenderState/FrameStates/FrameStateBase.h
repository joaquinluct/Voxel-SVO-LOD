#pragma once

#include <IFrameState.h>
#include <memory>
#include <RenderManager/Pipeline/PipelineState.h>


class FrameStateBase : public IFrameState {
public:
    bool IsInitialized() const { return m_initialized; }
    bool IsReady() const { return !m_dirty; }
    void MarkDirty() { m_dirty = true; }
    void MarkReady() { m_dirty = false; }
    void SetPipelineState(std::shared_ptr<PipelineStateManager> pipelineState) { m_pipelineState = pipelineState; }
protected:
    std::shared_ptr<PipelineStateManager> m_pipelineState = nullptr;
    bool m_dirty = false;
    bool m_initialized = false;
};
