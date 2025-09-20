#include "UpdateTerrainJob.h"
#include <Assets/Base/MeshAssetBase.h>
#include <Defines/EngineDefinition.h>
#include <Defines/Matrix/MaterialMatrix.h>
//#include <Defines/Pipeline.h>
#include <Defines/Types/ThreadTypes.h>
#include <Game/Systems/Terrain.h>
#include <Game/Systems/Terrain/Chunk/Chunk.h>
#include <Game/Systems/World.h>
//#include <Managers/RenderState/FrameStates/MeshFrameState.h>
#include <Managers/RenderState/FrameStates/PipelineFrameState.h>
//#include <Managers/RenderState/FrameStates/RenderFrameState.h>
#include <Managers/RenderState/FrameStates/TerrainFrameState.h>
#include <Services/FrameStateService.h>
#include <Text/Text.h>
#include <vector>

bool UpdateTerrainJob::Execute(JobContext* context)
{
    if (m_isGenerating) {
        return false;
    }
    m_isGenerating = true;
    // -- PRUEBA OBJETO SIMPLE
    //MeshFrameState* meshState = context->frameStateService->MeshState(false);

    //if (!meshState->GetCurrentMesh()) {
    //    MeshAssetBase* simpleMesh = AssetLocator::GetMeshAsset("House1Mesh").get();
    //    if (!simpleMesh) {
    //        return; // No hay malla de terreno o no está cargada
    //    }
    //    if (FAILED(simpleMesh->Init())) {
    //        return;
    //    }
    //    RenderFrameState* renderState = context->frameStateService->RenderState(false);
    //    // Actualizar el buffer (debe ir primero si se usa D3D11_USAGE_DEFAULT)
    //    renderState->AddMeshOperation(PipelineOperationType::Device_UpdateConstantsBufferResource, "Mesh_ConstantBuffers");
    //    // Configurar los shaders
    //    renderState->AddMeshOperation(PipelineOperationType::Mesh_Render_SetVertexShader, "Mesh_VertexShader");
    //    renderState->AddMeshOperation(PipelineOperationType::Mesh_Render_SetPixelShader, "Mesh_PixelShader");
    //    // Configurar las texturas y samplers (inputs para los shaders)
    //    renderState->AddMeshOperation(PipelineOperationType::Mesh_Render_SetTexture, "Mesh_Textures");
    //    renderState->AddMeshOperation(PipelineOperationType::Mesh_Render_SetSampler, "Mesh_Render_SetSampler");
    //    // Configurar los buffers de vertices y el layout (inputs para el vertex shader)
    //    renderState->AddMeshOperation(PipelineOperationType::Mesh_Render_SetInputLayout, "Mesh_InputLayout");
    //    renderState->AddMeshOperation(PipelineOperationType::Mesh_Render_SetVertexBuffer, "Mesh_VertexBuffer");
    //    renderState->AddMeshOperation(PipelineOperationType::Mesh_Render_SetIndexBuffer, "Mesh_IndexBuffer");
    //    // Configurar la topologia de la malla
    //    renderState->AddMeshOperation(PipelineOperationType::Mesh_Render_SetPrimitiveToplogy, "TerrainMeshTopo");
    //    // Dibujar la malla
    //    renderState->AddMeshOperation(PipelineOperationType::Device_draw, "TerrainMeshDraw");
    //    meshState->AddMesh(simpleMesh);
    //    /*context->frameStateService->SwapBuffer(FRAME_STATE_MESH);
    //    context->frameStateService->SwapBuffer(FRAME_STATE_RENDER);*/


    //    MaterialMatrix::MaterialData mParams{};
    //    mParams.Albedo = { 1.0f, 1.0f, 1.0f, 1.0f };
    //    mParams.Metallic = 0.0f;
    //    mParams.Roughness = 1.0f;
    //    mParams.AO = 1.0f;
    //    context->frameStateService->PipelineState(false)->UpdateConstanBufferValue("MaterialData", mParams);
    //}
    //return;
    // -- FINAL - PRUEBA OBJETO SIMPLE




    // Obtener el terreno
    //Terrain* terrain = context->world->GetTerrain().get();

    //if (terrain->IsGenerating() || terrain->GetTerrainMesh() != nullptr && terrain->GetTerrainMesh()->IsGenerating()) {
    //    return false; // No actualizar si el terreno está generando
    //}

    //// Actualizar el terreno
    //auto lock = context->frameStateService->LockAll();
    //terrain->Update(context->engineContext->deltaTime);
    //terrain->GenerateMesh();

    ///*auto lock = context->frameStateService->LockAll();
    //context->frameStateService->UnlockAll(lock);*/

    //RenderFrameState* renderState = context->frameStateService->RenderState(false);
    //TerrainFrameState* terrainState = context->frameStateService->TerrainState(false);
    ////MeshFrameState* meshState = context->frameStateService->MeshState(false);

    //MeshAssetBase* terrainMesh = terrain->GetTerrainMesh();

    //if (!terrainMesh || terrainMesh->GetVertexCount() <= 0) {
    //    return false; // No hay malla de terreno o no está cargada
    //}

    ////lock = context->frameStateService->LockAll();
    //renderState->ClearMeshOperations();

    //// Actualizar el buffer (debe ir primero si se usa D3D11_USAGE_DEFAULT)
    //renderState->AddMeshOperation(PipelineOperationType::Device_UpdateConstantsBufferResource, "Mesh_ConstantBuffers", {});
    //// Configurar los shaders
    //renderState->AddMeshOperation(PipelineOperationType::Mesh_Render_SetVertexShader, "Mesh_VertexShader", {});

    //PipelinePixelShaderData psData = {};
    //psData.data = terrainMesh->GetMaterial()->GetPixelShader().Get();
    //renderState->AddMeshOperation(PipelineOperationType::Mesh_Render_SetPixelShader, "Mesh_PixelShader", &psData);
    //// Configurar las texturas y samplers (inputs para los shaders)
    //// Configurar los buffers de vertices y el layout (inputs para el vertex shader)
    //renderState->AddMeshOperation(PipelineOperationType::Mesh_Render_SetInputLayout, "Mesh_InputLayout", {});
    //renderState->AddMeshOperation(PipelineOperationType::Mesh_Render_SetTexture, "Mesh_Textures", {});
    //renderState->AddMeshOperation(PipelineOperationType::Mesh_Render_SetSampler, "Mesh_Render_SetSampler", {});
    //PipelineSetVertexBufferData vData{};
    ////terrainMesh->GetVertexBuffer().CopyTo(&vData.vertexBuffer);
    ////vData.vertexBufferRaw = terrainMesh->GetVertexBuffer().Get();
    //vData.SetVertexBuffer(terrainMesh->GetVertexBuffer().Get(), terrainMesh->GetVertexTypeSize());
    //vData.offset = 0;

    //renderState->AddMeshOperation(PipelineOperationType::Mesh_Render_SetVertexBuffer, "Mesh_VertexBuffer", &vData);
    //renderState->AddMeshOperation(PipelineOperationType::Mesh_Render_SetIndexBuffer, "Mesh_IndexBuffer", {});
    //// Configurar la topologia de la malla
    //renderState->AddMeshOperation(PipelineOperationType::Mesh_Render_SetPrimitiveToplogy, "TerrainMeshTopo", {});
    //// Dibujar la malla
    //std::vector<Chunk*> visibleChunks = terrain->GetVisibleChunks();
    //size_t currentIndexOffset = 0;
    //for (Chunk* chunk : visibleChunks) {
    //    PipelineDrawIndexedData* drawData = new PipelineDrawIndexedData();
    //    // Número de índices a dibujar para este chunk
    //    drawData->numIndexes = chunk->GetIndexCount();
    //    // Desfase de índices en el búfer        
    //    drawData->indexOffset = static_cast<UINT>(currentIndexOffset);    //            
    //    // Los índices ya han sido ajustados para que apunten al lugar correcto.
    //    drawData->vertexOffset = 0;
    //    //AddOperation(PipelineOperationType::Device_drawIndexedShared);
    //    renderState->AddDrawIndexedMeshOperation(PipelineOperationType::Device_draw, "TerrainMeshDraw" + ParseInt(currentIndexOffset), drawData);
    //    // Actualizar el offset para el siguiente chunk
    //    currentIndexOffset += chunk->GetIndexCount();
    //}

    //MaterialMatrix::MaterialData mParams{};
    //mParams.Albedo = { 1.0f, 1.0f, 1.0f, 1.0f };
    //mParams.Metallic = 0.0f;
    //mParams.Roughness = 1.0f;
    //mParams.AO = 1.0f;
    //context->frameStateService->PipelineState(false)->UpdateConstanBufferValue("MaterialData", mParams);

    //MaterialMatrix::Terrain2BlendBuffer mTerrainBuffer{};
    //mTerrainBuffer = {};
    //mTerrainBuffer.grassTransitionHeight = 100.0f; // La hierba empieza desde la base
    //mTerrainBuffer.grassTransitionSlope = 0.2f;  // Se mezcla a pendientes moderadas
    //mTerrainBuffer.dirtTransitionHeight = 150.0f;  // La tierra empieza a aparecer a partir de 5m de altura
    //mTerrainBuffer.dirtTransitionSlope = 0.35f;   // Se mezcla en pendientes moderadas a altas
    //mTerrainBuffer.rockTransitionHeight = 310.0f; // La roca empieza a aparecer a 10m de altura
    //mTerrainBuffer.rockTransitionSlope = 0.6f;   // Se mezcla en pendientes altas
    //mTerrainBuffer.snowTransitionHeight = 500.0f; // La nieve aparece a partir de 15m de altura
    //mTerrainBuffer.snowTransitionSlope = 0.4f;   // La nieve se acumula en pendientes bajas
    //mTerrainBuffer.beachTransitionHeight = -100.0f; // La playa empieza desde la base
    //mTerrainBuffer.beachTransitionSlope = 0.2f; // Se mezcla en pendientes suaves
    //mTerrainBuffer.terrainScale = 0.005f; // Un valor bajo para que las texturas no se vean demasiado estiradas

    //context->frameStateService->PipelineState(false)->UpdateConstanBufferValue("Terrain2BlendBuffer", mTerrainBuffer);

    //meshState->ClearMeshes();
    //meshState->AddMesh(terrainMesh);

    //context->frameStateService->UnlockAll(lock);

    ////context->frameStateService->SwapBuffer(FRAME_STATE_MESH);
    //terrainState->SetChunks(terrain->GetVisibleChunks());


    ///*context->frameStateService->SwapBuffer(FRAME_STATE_RENDER);
    //context->frameStateService->SwapBuffer(FRAME_STATE_TERRAIN);*/
    ////context->frameStateService->SwapBuffers();
    //m_isGenerating = false;
    return true;
}
