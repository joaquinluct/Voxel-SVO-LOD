#include "SceneSystem.h"
#include <Managers/SceneManager.h>
#include <Managers/RenderManager/Pipeline/ConcreteOperations.h>
#include <Managers/RenderState/FrameStates/RenderFrameState.h>
#include <Defines/Contants/FrameState.h>
#include <Managers/RenderManager/Jobs/Update/UpdateConstantBuffersJob.h>
#include <Managers/RenderManager/Jobs/Update/UpdateMeshJob.h>
#include <Game/Systems/World.h>
#include <wrl/client.h>
#include <d3d11.h>
#include <memory>
#include <vector>

void SceneSystem::CreatePassOperations() {
    if (!m_owner) return;

    // Moved from SceneManager::CreatePassOperations
    std::vector<RenderPassResource*> passes = m_owner->m_resources->GetPasses();
    for (const auto& pass : passes) {
        if (pass == nullptr || !pass->enabled) continue;

        std::vector<MeshResource*> meshes = m_owner->GetPassMeshes(pass);
        if (!meshes.size()) continue;

        PipelineMainInitialResources* mainResources = m_owner->m_resources->GetInitialResources();
        std::unique_ptr<SetViewportOperation> setViewOper = std::make_unique<SetViewportOperation>(mainResources->viewport);
        m_owner->m_frameStateService->RenderState(false)->AddOperation(std::move(setViewOper));

        std::unique_ptr<SetRenderTargetOperation> setRTVOper = std::make_unique<SetRenderTargetOperation>(mainResources->renderTargetView.Get(), mainResources->depthStencilResource->stencilViewData.Get());
        m_owner->m_frameStateService->RenderState(false)->AddOperation(std::move(setRTVOper));

        D3D11_RASTERIZER_DESC desc = pass->rasterizedDesc;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> state = pass->rasterizerState;
        std::unique_ptr<CreateRasterizerStateOperation> rsOper = std::make_unique<CreateRasterizerStateOperation>(m_owner->m_deviceManager->GetDevice().Get(), desc, state);
        m_owner->m_frameStateService->RenderState(false)->AddOperation(std::move(rsOper));

        if (pass->hasShader) {
            m_owner->CreateShaderOperations(pass->shaderName);
        }

        for (const auto& mesh : meshes) {
            if (mesh == nullptr) continue;
            int index = mesh->mesh->GetReadIndex();
            if (mesh->mesh->GetVertexCount(index) <= 0) continue;
            m_owner->CreateMeshOperations(pass, mesh);
        }
    }

    // Update constant buffers and mesh jobs as in original migration step
    m_owner->m_jobContext->shaders = m_owner->m_resources->GetShaders();
    m_owner->CreateJob<UpdateConstantBuffersJob>(FRAME_STATE_CONSTANT_BUFFERS.data(), m_owner->u_constantBuffer);

    m_owner->m_jobContext->meshes = m_owner->m_resources->GetMeshes();
    m_owner->CreateJob<UpdateMeshJob>(FRAME_STATE_MESHES.data(), m_owner->u_mesh);
}

void SceneSystem::CreateScene() {
    if (!m_owner) return;

    // Moved from SceneManager::CreateScene
    if (m_owner->isGeneratingScene) return;
    m_owner->isGeneratingScene = true;

    m_owner->m_frameStateService->RenderState(false)->Initialize(m_owner->m_deviceManager);

    std::vector<MeshAssetBase*> meshes = m_owner->m_jobContext->world->GetMeshes();
    if (m_owner->m_uiManager->IsRunnig()) {
        const auto& uiMesh = m_owner->m_uiManager->GetUIMesh();
        if (uiMesh) {
            meshes.push_back(uiMesh);
        }
    }

    m_owner->m_resources->SetMeshes(meshes);

    if (meshes.size() > 0) {
        CreatePassOperations();
    }

    bool hasOperations = false;
    if (m_owner->m_frameStateService->RenderState(false)->HasOperations()) {
        while (m_owner->m_frameStateService->IsRendering()) {}
        m_owner->m_frameStateService->RenderState(false)->SetCommandList(m_owner->m_frameStateService->RenderState(false)->FinalizeCommandList());
        hasOperations = true;
    }

    if (!hasOperations) {
        m_owner->isGeneratingScene = false;
        return;
    }

    if (m_owner->config.isRenderStateInitialized) {
        m_owner->m_frameStateService->SwapBuffer(FRAME_STATE_RENDER);
    } else {
        m_owner->m_frameStateService->SwapBufferContent(FRAME_STATE_RENDER);
        m_owner->config.isRenderStateInitialized = true;
    }
    m_owner->SetFlag(SyncFlagIndex::HasScene, true);

    m_owner->isGeneratingScene = false;
}
