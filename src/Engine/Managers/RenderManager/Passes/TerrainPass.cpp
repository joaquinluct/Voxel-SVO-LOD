#include "TerrainPass.h"
#include <ManagerLocator/ManagerLocator.h>
#include <ServiceLocator/ServiceLocator.h>
#include <Locators/Pipeline/PipelineStateLocator.h>
#include <Game/Systems/Shadows.h>
#include <Game/Systems/Lighting.h>
#include <Game/Systems/Terrain.h>
#include <Game/Systems/World.h>
#include <Util/Text/Text.h>
#include <Assets/Base/TerrainAsset.h>
#include <Assets/Base/MeshAsset.h>
#include <Defines/Matrix/Light.h>
#include <Locators/Registers/REGISTER_RENDER_PASS_MACRO.h>
#include <Defines/VertexDefinition.h>
#include <Services/FrameStateService.h>
#include <RenderState/FrameStates/TerrainFrameState.h>
#include <RenderState/FrameStates/CommonFrameState.h>
#include <RenderState/FrameStates/CameraFrameState.h>
#include <RenderState/FrameStates/WaterFrameState.h>
#include <RenderState/FrameStates/LightFrameState.h>
#include <RenderState/FrameStates/TimeFrameState.h>
#include <RenderState/FrameStates/MeshFrameState.h>
#include <RenderState/FrameStates/MaterialFrameState.h>

REGISTER_RENDER_PASS_TYPE(TerrainPass, "TerrainPass")

// CAMBIO: Se mantiene este método en TerrainPass
using namespace MatrixDefinitionBase;

HRESULT TerrainPass::CreateDynamicBuffers() {
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DYNAMIC;
    vbDesc.ByteWidth = 1024 * 1024 * 10;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = m_deviceManager->GetDevice()->CreateBuffer(&vbDesc, nullptr, &m_vertexBuffer);
    if (FAILED(hr)) {
        return hr;
    }

    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DYNAMIC;
    ibDesc.ByteWidth = 1024 * 1024 * 5;
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = m_deviceManager->GetDevice()->CreateBuffer(&ibDesc, nullptr, &m_indexBuffer);
    if (FAILED(hr)) {
        return hr;
    }
    return S_OK;
}

HRESULT TerrainPass::InitManagers() {
    m_deviceManager = ManagerLocator::GetDeviceManager();
    if (!m_deviceManager) {
        return E_FAIL;
    }
    m_cameraManager = ManagerLocator::GetCameraManager();
    if (!m_cameraManager) {
        return E_FAIL;
    }
    m_shaderManager = ManagerLocator::GetShaderManager();
    if (!m_shaderManager) {
        return E_FAIL;
    }
    m_shadows = ServiceLocator::GetService<Shadows>();
    if (!m_shadows) {
        return E_FAIL;
    }
    m_lighthing = ServiceLocator::GetService<Lighting>();
    if (!m_lighthing) {
        return E_FAIL;
    }
	m_FrameStateService = ServiceLocator::GetService<FrameStateService>();
    if (!m_FrameStateService) {
        return E_FAIL;
	}
    m_world = ServiceLocator::GetService<World>();
    if (!m_world) {
        return E_FAIL;
    }
    m_terrain = m_world->GetTerrain();
    m_terrainAsset = m_terrain->GetTerrainAsset();
    if (!m_terrainAsset) {
        return E_FAIL;
    }
    return S_OK;
}

HRESULT TerrainPass::Init() {
    config = std::make_shared<TerrainPassConfig>(TerrainPassConfig{});
    if (!config) { return E_FAIL; };
    HRESULT hr = InitManagers();
    if (FAILED(hr)) {
        return hr;
    }

    // CAMBIO: Se llama a la función de creación de búferes local.
    hr = CreateDynamicBuffers();

    return hr;
}

void TerrainPass::SetInitialOperations(const MeshAsset* mesh, FrameStateService* renderState) {
    /*AddInitialOperation(PipelineOperationType::Device_SetViewport);
    AddInitialOperation(PipelineOperationType::Device_Init_SetRenderTargetView);*/
    AddOperation(PipelineOperationType::Device_SetViewport);
    AddOperation(PipelineOperationType::Device_Init_SetRenderTargetView);
}

//void TerrainPass::SetInitialOperations(const MeshAsset* mesh, FrameStateService* renderState) {
//    // 1. Set Viewport
//    PipelineViewPortData vpData = {};
//    vpData.desc = m_initManager->GetViewport(config->viewPortState);
//    AddInitialOperation(PipelineOperationType::Device_SetViewport, vpData);
//
//    // 2. Set RenderTargetView
//    PipelineSetRenderTargetsData srData = {};
//    srData.targetView = nullptr;
//    srData.stencilView = m_initManager->GetDepthStencilView(config->stencilState);
//    srData.isColorPass = false;
//    AddInitialOperation(PipelineOperationType::Device_Init_SetRenderTargetView, srData);
//}


// ----------------------------------------------------------
// GetMeshes 
// ----------------------------------------------------------
std::map<std::string, std::shared_ptr<MeshAsset>> TerrainPass::GetMeshes(const SceneManager* SceneManager, FrameStateService* renderState) {
    std::map<std::string, std::shared_ptr<MeshAsset>> emptyMeshes;

    m_tempVertexData.clear();
    m_tempIndexData.clear();
    //m_visibleChunks.clear();

    //const std::vector<std::shared_ptr<Chunk>>& chunks = m_terrain->GetChunks(m_cameraManager->GetCurrentCamera());	
    const std::vector<std::shared_ptr<Chunk>> chunks = renderState->TerrainState()->GetChunks();

    if (!chunks.empty()) {
        const auto& firstChunkVertices = chunks[0]->GetVertices();
        if (!firstChunkVertices.empty()) {
            m_vertexTypeSize = firstChunkVertices[0]->Size();
        }
    }

    size_t currentVertexCount = 0; // <<-- Añadir este contador
    //m_FrameStateService->Lock();
    for (const auto& chunkShared : chunks) {
		Chunk* chunk = chunkShared.get();
        if (chunk) {            
            const std::vector<std::shared_ptr<IVertex>>& vertices = chunk->GetVertices();
			if (vertices.empty()) continue; // Si no hay vértices, saltar este chunk
            const std::vector<UINT>& indexes = chunk->GetIndexes();

            m_tempVertexData.reserve(m_tempVertexData.size() + (vertices.size() * m_vertexTypeSize));
            m_tempIndexData.reserve(m_tempIndexData.size() + indexes.size());

            // Copiar los vértices tal cual
            for (const auto& vertex : vertices) {                
                const void* source_data = vertex.get()->GetRawData();
                m_tempVertexData.insert(m_tempVertexData.end(), (const uint8_t*)source_data, (const uint8_t*)source_data + m_vertexTypeSize);                
            }

            // Copiar índices
            std::vector<UINT> adjustedIndexes;
            adjustedIndexes.reserve(indexes.size());
            for (UINT index : indexes) {
                adjustedIndexes.push_back(index + static_cast<UINT>(currentVertexCount));
            }
            m_tempIndexData.insert(m_tempIndexData.end(), adjustedIndexes.begin(), adjustedIndexes.end());
                        
            // Actualizar los chunks visibles
            //m_visibleChunks.push_back(chunk);

            // Actualizar el contador de vértices para el próximo chunk
            currentVertexCount += vertices.size();
        }
    }
    //m_FrameStateService->Unlock();
    emptyMeshes["terrain"] = m_terrain->GetTerrainAsset()->GetMesh();
    return emptyMeshes;
}

// ----------------------------------------------------------
// GetMeshes (versión alternativa, JUNTANDO TODOS LOS VÉRTICES E ÍNDICES)
// ----------------------------------------------------------

//std::map<std::string, std::shared_ptr<MeshAsset>> TerrainPass::GetMeshes(const SceneManager* SceneManager, FrameStateService* renderState) {
//    std::map<std::string, std::shared_ptr<MeshAsset>> emptyMeshes;
//
//    m_combinedVertexData.clear();
//    m_combinedIndexData.clear();
//
//    const std::vector<std::shared_ptr<Chunk>>& chunks = renderData.chunks;
//
//    if (chunks.empty()) {
//        emptyMeshes["terrain"] = m_terrain->GetTerrainAsset()->GetMesh();
//        return emptyMeshes;
//    }
//
//    // Determina el tamaño del vértice a partir del primer chunk
//    const auto& firstChunkVertices = chunks[0]->GetVertices();
//    if (!firstChunkVertices.empty()) {
//        std::visit([&](auto& currentVertex) {
//            m_singleDrawVertexTypeSize = currentVertex.Size();
//            }, *firstChunkVertices[0]);
//    }
//    else {
//        emptyMeshes["terrain"] = m_terrain->GetTerrainAsset()->GetMesh();
//        return emptyMeshes;
//    }
//
//    // Calcula el tamaño total de los buffers para hacer una sola reserva
//    size_t totalVertexCount = 0;
//    size_t totalIndexCount = 0;
//    for (const auto& chunk : chunks) {
//        if (chunk) {
//            totalVertexCount += chunk->GetVertexCount();
//            totalIndexCount += chunk->GetIndexCount();
//        }
//    }
//
//    m_combinedVertexData.reserve(totalVertexCount * m_singleDrawVertexTypeSize);
//    m_combinedIndexData.reserve(totalIndexCount);
//
//    size_t currentVertexCount = 0;
//
//    for (const auto& chunk : chunks) {
//        if (chunk) {
//            const std::vector<std::shared_ptr<VertexDefinition::VertexVariant>>& vertices = chunk->GetVertices();
//            if (vertices.empty()) continue;
//
//            // Copia los vértices al buffer combinado
//            for (const auto& vertex : vertices) {
//                std::visit([&](auto& currentVertex) {
//                    const void* source_data = currentVertex.GetRawData();
//                    m_combinedVertexData.insert(m_combinedVertexData.end(), (const uint8_t*)source_data, (const uint8_t*)source_data + m_singleDrawVertexTypeSize);
//                    }, *vertex);
//            }
//
//            // Copia los índices ajustados al buffer combinado
//            const std::vector<UINT>& indexes = chunk->GetIndexes();
//            for (UINT index : indexes) {
//                m_combinedIndexData.push_back(index + static_cast<UINT>(currentVertexCount));
//            }
//
//            // Actualiza el contador de vértices para el próximo chunk
//            currentVertexCount += vertices.size();
//        }
//    }
//
//    emptyMeshes["terrain"] = m_terrain->GetTerrainAsset()->GetMesh();
//    return emptyMeshes;
//}

// ----------------------------------------------------------
// BEGIN PASS
// ----------------------------------------------------------

std::vector<PipelineOperationType> TerrainPass::BeginPass(const MeshAsset* mesh, FrameStateService* renderState) {
    ClearOperations();

    SetInitialOperations(mesh, renderState);

    AddOperation(PipelineOperationType::Device_Init_SetRenderTargetView);
    AddOperation(PipelineOperationType::Device_ClearDepthStencilView);

    // ** CÓDIGO ACTUAL FUNCIONANDO CORRECTAMENTE 
    D3D11_MAPPED_SUBRESOURCE mappedVertices;
    D3D11_MAPPED_SUBRESOURCE mappedIndices;
    // CAMBIO: Se usan los buffers del TerrainPass, no del TerrainAsset
    HRESULT hr = m_deviceManager->GetContext()->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertices);
    if (FAILED(hr)) return GetOperations();
    hr = m_deviceManager->GetContext()->Map(m_indexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIndices);
    if (FAILED(hr)) {
        m_deviceManager->GetContext()->Unmap(m_vertexBuffer.Get(), 0);
        return GetOperations();
    }
    memcpy(mappedVertices.pData, m_tempVertexData.data(), m_tempVertexData.size());
    memcpy(mappedIndices.pData, m_tempIndexData.data(), m_tempIndexData.size() * sizeof(UINT));
    // ** fin CÓDIGO ACTUAL FUNCIONANDO CORRECTAMENTE 

    // ** CÓDIGO PRUEBA UNA SOLA MALLA 
    //D3D11_MAPPED_SUBRESOURCE mappedVertices;
    //D3D11_MAPPED_SUBRESOURCE mappedIndices;
    //HRESULT hr = m_deviceManager->GetContext()->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertices);
    //if (FAILED(hr)) return GetOperations();
    //hr = m_deviceManager->GetContext()->Map(m_indexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIndices);
    //if (FAILED(hr)) {
    //    m_deviceManager->GetContext()->Unmap(m_vertexBuffer.Get(), 0);
    //    return GetOperations();
    //}

    //// Copia de los buffers combinados
    //memcpy(mappedVertices.pData, m_combinedVertexData.data(), m_combinedVertexData.size());
    //memcpy(mappedIndices.pData, m_combinedIndexData.data(), m_combinedIndexData.size() * sizeof(UINT));
    // ** fin CÓDIGO PRUEBA UNA SOLA MALLA

    if (m_terrainAsset) {        
        AddOperation(PipelineOperationType::Device_SetConstantsBufferState);
        AddOperation(PipelineOperationType::Mesh_Render_SetVertexShader);
        AddOperation(PipelineOperationType::Mesh_Render_SetPixelShader);
        AddOperation(PipelineOperationType::Mesh_Render_SetInputLayout);
        AddOperation(PipelineOperationType::Mesh_Render_SetTexture);
        //if (needShadowMap) {
            AddOperation(PipelineOperationType::Mesh_Render_SetTexture);
        //}
        AddOperation(PipelineOperationType::Mesh_Render_SetSampler);
    }
    AddOperation(PipelineOperationType::Mesh_Render_SetVertexBuffer);
    AddOperation(PipelineOperationType::Mesh_Render_SetIndexBuffer);
    AddOperation(PipelineOperationType::Mesh_Render_SetPrimitiveToplogy);

    return GetOperations();
}

//std::vector<PipelineOperationType> TerrainPass::BeginPass(const MeshAsset* mesh, FrameStateService* renderState) {
//    ClearOperations();
//
//    SetInitialOperations(m_terrainAsset->GetMesh(), renderState);
//
//    PipelineSetRenderTargetsData srData = {};
//    srData.targetView = m_initManager->GetRenderTargetView(config->viewPortState);
//    srData.stencilView = m_initManager->GetDepthStencilView(config->stencilState);
//    srData.isColorPass = true;
//    AddOperation(PipelineOperationType::Device_Init_SetRenderTargetView, srData);
//
//    PipelineDepthStencilData dscData = m_initManager->GetDepthStencilData(config->stencilState);
//    dscData.stencilViewData = m_initManager->GetDepthStencilView(config->stencilState);
//    dscData.clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
//    AddOperation(PipelineOperationType::Device_ClearDepthStencilView, dscData);
//
//	// ** CÓDIGO ACTUAL FUNCIONANDO CORRECTAMENTE 
//    D3D11_MAPPED_SUBRESOURCE mappedVertices;
//    D3D11_MAPPED_SUBRESOURCE mappedIndices;
//    // CAMBIO: Se usan los buffers del TerrainPass, no del TerrainAsset
//    HRESULT hr = m_deviceManager->GetContext()->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertices);
//    if (FAILED(hr)) return GetOperations();
//    hr = m_deviceManager->GetContext()->Map(m_indexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIndices);
//    if (FAILED(hr)) {
//        m_deviceManager->GetContext()->Unmap(m_vertexBuffer.Get(), 0);
//        return GetOperations();
//    }
//    memcpy(mappedVertices.pData, m_tempVertexData.data(), m_tempVertexData.size());
//    memcpy(mappedIndices.pData, m_tempIndexData.data(), m_tempIndexData.size() * sizeof(UINT));
//	// ** fin CÓDIGO ACTUAL FUNCIONANDO CORRECTAMENTE 
//    
//    // ** CÓDIGO PRUEBA UNA SOLA MALLA 
//    //D3D11_MAPPED_SUBRESOURCE mappedVertices;
//    //D3D11_MAPPED_SUBRESOURCE mappedIndices;
//    //HRESULT hr = m_deviceManager->GetContext()->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertices);
//    //if (FAILED(hr)) return GetOperations();
//    //hr = m_deviceManager->GetContext()->Map(m_indexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIndices);
//    //if (FAILED(hr)) {
//    //    m_deviceManager->GetContext()->Unmap(m_vertexBuffer.Get(), 0);
//    //    return GetOperations();
//    //}
//
//    //// Copia de los buffers combinados
//    //memcpy(mappedVertices.pData, m_combinedVertexData.data(), m_combinedVertexData.size());
//    //memcpy(mappedIndices.pData, m_combinedIndexData.data(), m_combinedIndexData.size() * sizeof(UINT));
//    // ** fin CÓDIGO PRUEBA UNA SOLA MALLA
//    
//    if (m_terrainAsset) {
//        const Material* material = m_terrainAsset->GetMaterial();
//        shaderName = m_terrainAsset->GetMaterial()->GetShaderName();
//        bool needShadowMap = m_shaderManager->NeedsShadow(shaderName);
//        std::string shadowTextureViewName = config->shadowTextureView;
//
//        std::shared_ptr<MatrixParams> bData = std::make_shared<MatrixParams>();
//        std::shared_ptr<LightMatrixParams> lData = std::make_shared<LightMatrixParams>();
//        std::shared_ptr<MaterialMatrixParams> mData = std::make_shared<MaterialMatrixParams>();
//
//        bData->worldMatrix = renderState
//        bData->viewMatrix = DirectX::XMMatrixTranspose(renderData.viewMatrix);
//        bData->projectionMatrix = DirectX::XMMatrixTranspose(renderData.projectionMatrix);
//        //mData->viewProjectionMatrix = DirectX::XMMatrixTranspose(XMMatrixMultiply(mData->viewMatrix, mData->projectionMatrix));
//        bData->viewProjectionMatrix = DirectX::XMMatrixTranspose(XMMatrixMultiply(renderData.viewMatrix, renderData.projectionMatrix));
//        bData->cameraPosition = renderData.cameraPosition;
//        lData->lightDirection = renderData.lightDirection;
//        lData->lightColor = m_lighthing->GetLightColor();
//        mData->materialF0 = XMFLOAT3{ .4f,.4f, .4f };
//        bData->skyboxData.skyColor = XMFLOAT4(0.4f, 0.4f, 1.0f, 1.0f);
//        mData->textureTransform = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f);
//        //mData->terrainBlenderData = m_terrain->GetTerrainBlenderData();
//        mData->terrain2BlenderData = m_terrain->GetTerrain2BlenderData();
//
//        PipelineMatrixBufferData matrixData = {};
//        matrixData.params[MATRIX_PARAM_BASE.data()] = bData;
//        matrixData.params[MATRIX_PARAM_LIGHT.data()] = lData;
//        matrixData.params[MATRIX_PARAM_MATERIAL.data()] = mData;
//        matrixData.constantsBuffers = material->GetConstantBuffers();
//        m_shaderManager->GetMatrixDefinitions(shaderName, matrixData.matrices);
//        AddOperation(PipelineOperationType::Device_SetConstantsBufferState, matrixData);
//
//        PipelineVertexShaderData vsData = {};
//        vsData.data = material->GetVertexShader().Get();
//        AddOperation(PipelineOperationType::Mesh_Render_SetVertexShader, vsData);
//
//        PipelinePixelShaderData psData = {};
//        psData.data = material->GetPixelShader().Get();
//        AddOperation(PipelineOperationType::Mesh_Render_SetPixelShader, psData);
//
//        PipelineLayoutData lyData = {};
//        lyData.data = material->GetInputLayout();
//        AddOperation(PipelineOperationType::Mesh_Render_SetInputLayout, lyData);
//
//        PipelineTextureData tData = {};
//        tData.data = {};
//        tData.numTextures = material->GetNumTextures();
//        tData.data = material->GetTextures();
//        AddOperation(PipelineOperationType::Mesh_Render_SetTexture, tData);
//
//        if (needShadowMap) {
//            PipelineTextureData shadowData = {};
//            shadowData.data = {};
//            shadowData.data.push_back(m_initManager->GetShaderResourceView(shadowTextureViewName).Get());
//            shadowData.numTextures = 1;
//            shadowData.startSlot = tData.numTextures;
//            AddOperation(PipelineOperationType::Mesh_Render_SetTexture, shadowData);
//        }
//
//        PipelineSamplerSateData spData = {};
//        spData.data = m_initManager->GetSamplerStates(WstringToString(shaderName));
//        AddOperation(PipelineOperationType::Mesh_Render_SetSampler, spData);
//    }
//
//    PipelineSetVertexBufferData vData = {};
//    vData.stride = static_cast<UINT>(m_vertexTypeSize);
//    vData.offset = 0;
//    vData.vertexBuffer = m_vertexBuffer; // CAMBIO: Se usa el miembro de TerrainPass
//    AddOperation(PipelineOperationType::Mesh_Render_SetVertexBuffer, vData);
//
//    PipelineSetIndexBufferData iData = {};
//    iData.indexBuffer = m_indexBuffer; // CAMBIO: Se usa el miembro de TerrainPass
//    iData.offset = 0;
//	iData.format = DXGI_FORMAT_R32_UINT; // CAMBIO: Se asume que los índices son UINT
//    AddOperation(PipelineOperationType::Mesh_Render_SetIndexBuffer, iData);
//
//    PipelinePrimitiveTopologyData ptData = {};
//    ptData.data = static_cast<D3D_PRIMITIVE_TOPOLOGY>(config->primitiveTopology);
//    AddOperation(PipelineOperationType::Mesh_Render_SetPrimitiveToplogy, ptData);
//
//    return GetOperations();
//}


// ----------------------------------------------------------
// EXEC PASS
// ----------------------------------------------------------

std::vector<PipelineOperationType> TerrainPass::ExecPass(const MeshAsset* mesh, FrameStateService* renderState) {
    ClearOperations();
    size_t currentIndexOffset = 0;
    const std::vector<std::shared_ptr<Chunk>> chunks = renderState->TerrainState()->GetChunks();
    for (const auto& visibleChunk : chunks) {
        Chunk* rawChunk = visibleChunk.get();

        PipelineDrawIndexedData drawData = {};
        // Número de índices a dibujar para este chunk
        drawData.numIndexes = rawChunk->GetIndexCount();
        // Desfase de índices en el búfer        
        drawData.indexOffset = static_cast<UINT>(currentIndexOffset);    //            
        // Los índices ya han sido ajustados para que apunten al lugar correcto.
        drawData.vertexOffset = 0;
        //AddOperation(PipelineOperationType::Device_drawIndexedShared);
        AddOperation(PipelineOperationType::Device_draw);
        // Actualizar el offset para el siguiente chunk
        currentIndexOffset += rawChunk->GetIndexCount();
    }
    return GetOperations();
    // ** fin CÓDIGO ACTUAL FUNCIONANDO CORRECTAMENTE

    // ** CÓDIGO PRUEBA UNA SOLA MALLA
    /*ClearOperations();
    if (!m_combinedIndexData.empty()) {
        PipelineDrawIndexedData drawData = {};
        drawData.numIndexes = static_cast<UINT>(m_combinedIndexData.size());
        drawData.indexOffset = 0;
        drawData.vertexOffset = 0;
        AddOperation(PipelineOperationType::Device_drawIndexed, drawData);
    }
    return GetOperations();*/
    // ** fin CÓDIGO PRUEBA UNA SOLA MALLA
}

std::vector<PipelineOperationType> TerrainPass::EndPass() {
    // Operaciones de finalización del pase: Limpieza de buffers y reciclaje de chunks    
    // Liberar los buffers dinámicos
    /*AddOperation(PipelineOperationType::Mesh_Render_ResetVertexBuffer);
    AddOperation(PipelineOperationType::Mesh_Render_ResetIndexBuffer);
    AddOperation(PipelineOperationType::Mesh_Render_Reset_Textures);*/

    // CAMBIO: Limpieza de los datos temporales
    m_tempVertexData.clear();
    m_tempIndexData.clear();

    // Limpieza de los buffers dinámicos
    m_deviceManager->GetContext()->Unmap(m_vertexBuffer.Get(), 0);
    m_deviceManager->GetContext()->Unmap(m_indexBuffer.Get(), 0);
    // Limpieza de los chunks pendientes de borrado
    m_terrain->EmptyRecycleBin();

    /*if (m_terrain->GetChunkService()->IsDirty()) {
        m_terrain->SetDirty(true);
    }*/

    return {};
}

//std::vector<PipelineOperationType> TerrainPass::ExecPass(const MeshAsset* mesh, FrameStateService* renderState) {
//	// ** CÓDIGO ACTUAL FUNCIONANDO CORRECTAMENTE
//    ClearOperations();
//    size_t currentIndexOffset = 0;
//    const std::vector<std::shared_ptr<Chunk>> chunks = renderState->TerrainState()->GetChunks();
//    //for (const auto& visibleChunk : m_visibleChunks) {
//    for (const auto& visibleChunk : chunks) {
//        Chunk* rawChunk = visibleChunk.get();
//
//        PipelineDrawIndexedData drawData = {};
//        // Número de índices a dibujar para este chunk
//        drawData.numIndexes = rawChunk->GetIndexCount();
//        // Desfase de índices en el búfer        
//        drawData.indexOffset = static_cast<UINT>(currentIndexOffset);    //            
//        // Los índices ya han sido ajustados para que apunten al lugar correcto.
//        drawData.vertexOffset = 0;
//        AddOperation(PipelineOperationType::Device_drawIndexed, drawData);
//        // Actualizar el offset para el siguiente chunk
//        currentIndexOffset += rawChunk->GetIndexCount();
//    }
//    return GetOperations();
//	// ** fin CÓDIGO ACTUAL FUNCIONANDO CORRECTAMENTE
//
//	// ** CÓDIGO PRUEBA UNA SOLA MALLA
//    /*ClearOperations();
//    if (!m_combinedIndexData.empty()) {
//        PipelineDrawIndexedData drawData = {};
//        drawData.numIndexes = static_cast<UINT>(m_combinedIndexData.size());
//        drawData.indexOffset = 0;
//        drawData.vertexOffset = 0;
//        AddOperation(PipelineOperationType::Device_drawIndexed, drawData);
//    }
//    return GetOperations();*/
//	// ** fin CÓDIGO PRUEBA UNA SOLA MALLA
//}

//std::vector<PipelineOperationType> TerrainPass::EndPass() {
//	// Operaciones de finalización del pase: Limpieza de buffers y reciclaje de chunks    
//    // Liberar los buffers dinámicos
//    /*AddOperation(PipelineOperationType::Mesh_Render_ResetVertexBuffer);
//    AddOperation(PipelineOperationType::Mesh_Render_ResetIndexBuffer);
//    AddOperation(PipelineOperationType::Mesh_Render_Reset_Textures);*/
//        
//    // CAMBIO: Limpieza de los datos temporales
//    m_tempVertexData.clear();
//	m_tempIndexData.clear();
//
//	// Limpieza de los buffers dinámicos
//    m_deviceManager->GetContext()->Unmap(m_vertexBuffer.Get(), 0);
//    m_deviceManager->GetContext()->Unmap(m_indexBuffer.Get(), 0);
//    // Limpieza de los chunks pendientes de borrado
//    m_terrain->EmptyRecycleBin();
//
//    /*if (m_terrain->GetChunkService()->IsDirty()) {
//        m_terrain->SetDirty(true);
//    }*/
//
//    return {};
//}