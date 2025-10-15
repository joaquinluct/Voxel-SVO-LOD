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

// Forward additional methods that are implemented on SceneManager and now
// callable via the SceneSystem migration facade.

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

void SceneSystem::CreateMeshOperations(RenderPassResource* pass, MeshResource* meshResource) {
    if (!m_owner || !meshResource) return;

    MeshAssetBase* mesh = meshResource->mesh;

    if (!pass->hasShader) {
        m_owner->CreateShaderOperations(mesh->GetShaderAssetName());
    }

    int index = mesh->GetReadIndex();

    ID3D11Buffer* vBuffer = mesh->GetVertexBuffer(index).Get();
    if (vBuffer == nullptr) return;

    ID3D11Buffer* iBuffer = mesh->GetIndexBuffer(index).Get();
    UINT stride = mesh->GetVertexTypeSize();
    UINT indexCount = mesh->GetIndexCount(index);

    if (meshResource->textures.size() > 0) {
        std::unique_ptr<SetTextureOperation> ssOper = std::make_unique<SetTextureOperation>(meshResource->textures, 6);
        m_owner->m_frameStateService->RenderState(false)->AddOperation(std::move(ssOper));
    }

    std::unique_ptr<SetVertexBufferOperation> vbOper = std::make_unique<SetVertexBufferOperation>(vBuffer, stride, 0);
    m_owner->m_frameStateService->RenderState(false)->AddOperation(std::move(vbOper));
    std::unique_ptr<SetIndexBufferOperation> ibOper = std::make_unique<SetIndexBufferOperation>(iBuffer, DXGI_FORMAT_R16_UINT, 0);
    m_owner->m_frameStateService->RenderState(false)->AddOperation(std::move(ibOper));

    std::unique_ptr<SetPrimitiveTopologyOperation > primiOper = std::make_unique<SetPrimitiveTopologyOperation>(mesh->GetPrimitiveTopology());
    m_owner->m_frameStateService->RenderState(false)->AddOperation(std::move(primiOper));

    std::unique_ptr<DrawIndexedOperation> drawOper = std::make_unique<DrawIndexedOperation>(indexCount, 0, 0);
    m_owner->m_frameStateService->RenderState(false)->AddOperation(std::move(drawOper));
}

void SceneSystem::CreateShaderInitialOperations(ShaderResource* shaderResource) {
    if (!m_owner || !shaderResource) return;
    std::vector<PipelineConstantBufferResource> buffers = shaderResource->GetConstantsBuffers();
    std::unique_ptr<BindConstantsBuffersOperation> cbOper = std::make_unique<BindConstantsBuffersOperation>(buffers, 0);
    m_owner->m_frameStateService->RenderState(false)->AddOperation(std::move(cbOper));
}

void SceneSystem::CreateShaderOperations(std::string_view shaderName) {
    if (!m_owner) return;
    ShaderResource* shader = m_owner->m_resources->GetShader(shaderName.data());

    if (!shader || !shader->shader || (shader && !shader->flags.GetFlag(SyncFlagIndex::HasConstsBufferDefined))) {
        shader = m_owner->m_resources->CreateShaderResource(shaderName.data());
        m_owner->m_resources->SetShaderResource(shaderName.data(), shader);
    }

    if (shader == nullptr || !shader->shader) return;

    CreateShaderInitialOperations(shader);

    ShaderAsset* currentShader = shader->shader;
    ID3D11VertexShader* vShader = currentShader->GetVertexShader();
    ID3D11PixelShader* pShader = currentShader->GetPixelShader();
    const std::vector<ID3D11SamplerState*>& samplers = shader->samplers;

    std::unique_ptr<SetSamplerOperation> samplerOper = std::make_unique<SetSamplerOperation>(samplers, 0);
    m_owner->m_frameStateService->RenderState(false)->AddOperation(std::move(samplerOper));

    std::unique_ptr<SetVertexShaderOperation> vsOper = std::make_unique<SetVertexShaderOperation>(vShader);
    m_owner->m_frameStateService->RenderState(false)->AddOperation(std::move(vsOper));

    std::unique_ptr<SetPixelShaderOperation> psOper = std::make_unique<SetPixelShaderOperation>(pShader);
    m_owner->m_frameStateService->RenderState(false)->AddOperation(std::move(psOper));

    std::unique_ptr<SetInputLayoutOperation> ilOper = std::make_unique<SetInputLayoutOperation>(currentShader->GetInputLayout());
    m_owner->m_frameStateService->RenderState(false)->AddOperation(std::move(ilOper));

    m_owner->config.shaderName = shaderName.data();
}

void SceneSystem::CreateMainOperations() {
    if (!m_owner) return;
    PipelineMainInitialResources* mainResources = m_owner->m_resources->GetInitialResources();
    std::unique_ptr<ClearOperation> clearOper = std::make_unique<ClearOperation>(mainResources->renderTargetView.Get(), mainResources->depthStencilResource->stencilViewData.Get(), mainResources->clearColor);
    m_owner->m_frameStateService->RenderState(false)->AddInitialOperation(std::move(clearOper));
}

std::vector<MeshResource*> SceneSystem::GetPassMeshes(RenderPassResource* pass) {
    if (!m_owner) return {};
    const auto& meshes = m_owner->m_resources->GetMeshes();
    std::vector<MeshResource*> result;
    for (const auto& mesh : meshes) {
        if (mesh == nullptr) continue;
        int index = mesh->mesh->GetReadIndex();
        if (mesh->mesh->GetVertexCount(index) <= 0) continue;
        int passes = mesh->mesh->GetRenderPassesValue();
        bool isInPass = pass->id & passes;
        if (!isInPass) continue;
        result.push_back(mesh);
    }
    return result;
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
