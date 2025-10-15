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

// The implementations for migrated helper operations live in
// `SceneSystem.partition.cpp` to keep the migration modular.

// Provide lightweight stubs that delegate to the owning SceneManager.
// This ensures the build remains clean while the full implementations
// are migrated into the partition file incrementally.
void SceneSystem::CreatePassOperations() {
    if (!m_owner) return;
    m_owner->CreatePassOperations();
}

void SceneSystem::CreateScene() {
    if (!m_owner) return;
    m_owner->CreateScene();
}

void SceneSystem::CreateMeshOperations(RenderPassResource* pass, MeshResource* meshResource) {
    if (!m_owner) return;
    m_owner->CreateMeshOperations(pass, meshResource);
}

void SceneSystem::CreateShaderOperations(std::string_view shaderName) {
    if (!m_owner) return;
    m_owner->CreateShaderOperations(shaderName);
}

void SceneSystem::CreateShaderInitialOperations(ShaderResource* shaderResource) {
    if (!m_owner) return;
    m_owner->CreateShaderInitialOperations(shaderResource);
}

void SceneSystem::CreateMainOperations() {
    if (!m_owner) return;
    m_owner->CreateMainOperations();
}

std::vector<MeshResource*> SceneSystem::GetPassMeshes(RenderPassResource* pass) {
    if (!m_owner) return {};
    return m_owner->GetPassMeshes(pass);
}
