#include "SceneSystem.h"
#include <Managers/SceneManager.h>

HRESULT SceneSystem::Init(SceneManager* owner) {
    if (!owner) return E_FAIL;
    m_owner = owner;
    return S_OK;
}

HRESULT SceneSystem::PostInit() {
    return S_OK;
}

void SceneSystem::Shutdown() {
    m_owner = nullptr;
}

void SceneSystem::Update(float deltaTime) {
    if (!m_owner) return;
    m_owner->UpdateImpl(deltaTime);
}

void SceneSystem::FillCommandBuffer(CommandBuffer& buffer) {
    if (!m_owner) return;
    m_owner->FillCommandBufferImpl(buffer);
}

// The actual implementations are provided in partition file(s). The
// partition file `SceneSystem.partition.cpp` provides the real
// implementations; no local forwarders are defined here to avoid
// duplicate symbol definitions at link time.
