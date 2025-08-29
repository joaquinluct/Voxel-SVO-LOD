#include "WaterPass.h"
#include <ManagerLocator/ManagerLocator.h>
#include <ServiceLocator/ServiceLocator.h>
#include <Locators/Pipeline/PipelineStateLocator.h>
#include <Game/Systems/Shadows.h>
#include <Game/Systems/Lighting.h>
#include <Game/Systems/Terrain.h>
#include <Game/Systems/Water.h>
#include <Game/Systems/World.h>
#include <Util/Text/Text.h>
#include <Assets/Base/TerrainAsset.h>
#include <Assets/Base/MeshAsset.h>
#include <Locators/Registers/REGISTER_RENDER_PASS_MACRO.h>
#include <Defines/Matrix/Light.h>
#include <Defines/VertexDefinition.h>
#include <Defines/Matrix/WaterMatrices.h>
#include <Defines/Matrix/TimeMatrices.h>
#include <Services/FrameStateService.h>
#include <Config/Base/RenderPass/WaterPassConfig.h>
#include <Util/DateTime.h>
#include <RenderState/FrameStates/CommonFrameState.h>
#include <RenderState/FrameStates/CameraFrameState.h>
#include <RenderState/FrameStates/WaterFrameState.h>
#include <RenderState/FrameStates/LightFrameState.h>
#include <RenderState/FrameStates/TimeFrameState.h>
#include <RenderState/FrameStates/MeshFrameState.h>
#include <RenderState/FrameStates/MaterialFrameState.h>

REGISTER_RENDER_PASS_TYPE(WaterPass, "WaterPass")


HRESULT WaterPass::InitManagers() {
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
    /*m_renderTargetManager = ManagerLocator::GetRenderManager();
    if (!m_renderTargetManager) {
        return E_FAIL;
    }*/
    m_initManager = ManagerLocator::GetManager<InitManager>();
    if (!m_initManager) {
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
	m_water = ServiceLocator::GetService<Water>();
	if (!m_water) {
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

HRESULT WaterPass::Init() {
    config = std::make_shared<WaterPassConfig>(WaterPassConfig{});
    if (!config) { return E_FAIL; };
    HRESULT hr = InitManagers();
    if (FAILED(hr)) {
        return hr;
    }
	SetInitialOperations(nullptr, {});
    return hr;
}

void WaterPass::SetInitialOperations(std::shared_ptr<MeshAsset> mesh, const std::shared_ptr<FrameStateService> renderState) {	
	AddInitialOperation(PipelineOperationType::Device_Init_SetRenderTargetView);
	AddInitialOperation(PipelineOperationType::Device_ClearDepthStencilView);
}

//void WaterPass::SetInitialOperations(std::shared_ptr<MeshAsset> mesh, const std::shared_ptr<FrameStateService> renderState) {
//    //std::string cullMode = config->cull_mode;
//    float width = m_deviceManager->GetWidth();
//    float height = m_deviceManager->GetHeight();
//    int primitiveTopology = config->primitiveTopology;
//    std::string stencilName = config->stencilState;
//
//    // 1. Configurar viewport
//    /*PipelineViewPortData vpData = {};
//    vpData.desc = m_initManager->GetViewport(config->viewPortState);
//    AddInitialOperation(PipelineOperationType::Device_SetViewport, vpData);*/
//
//    // 2. Set Render target view
//    PipelineSetRenderTargetsData srData = {};
//    srData.targetView = m_initManager->GetRenderTargetView(config->viewPortState);
//    srData.stencilView = m_initManager->GetDepthStencilView(config->stencilState);
//    srData.isColorPass = true;
//    AddInitialOperation(PipelineOperationType::Device_Init_SetRenderTargetView, srData);
//
//    // 3. Borrar Render target view y Depth stencil
//    /*PipelineRenderTargetViewData rtvData = {};
//    rtvData.data = m_initManager->GetRenderTargetView(config->viewPortState);
//    rtvData.clearColor = XMFLOAT4{ .4f,.4f,.0f,1.0f };
//    AddInitialOperation(PipelineOperationType::Device_ClearRenderTargetView, rtvData);*/
//
//    PipelineDepthStencilData dscData = m_initManager->GetDepthStencilData(stencilName);
//    dscData.stencilViewData = m_initManager->GetDepthStencilView(stencilName);
//    dscData.clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
//    AddInitialOperation(PipelineOperationType::Device_ClearDepthStencilView, dscData);
//}


// ----------------------------------------------------------
// GetMeshes 
// ----------------------------------------------------------
std::map<std::string, std::shared_ptr<MeshAsset>> WaterPass::GetMeshes(const SceneManager* SceneManager, FrameStateService* renderState) {
    return {};
    //return SceneManager->GetMeshesByRenderPass(RenderPassType::WaterPass);
}

// ----------------------------------------------------------
// GetMeshes (versión alternativa, JUNTANDO TODOS LOS VÉRTICES E ÍNDICES)
// ----------------------------------------------------------

//std::map<std::string, std::shared_ptr<MeshAsset>> WaterPass::GetMeshes(const SceneManager* SceneManager, FrameStateService* renderState) {
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
std::vector<PipelineOperationType> WaterPass::BeginPass(const MeshAsset* mesh, FrameStateService* renderState) {
	// 1. Operacines fijas definidas al inicio
	std::vector<PipelineOperationType> init = GetInitialOperations();

	return init;
}

WaterDefinition::WaterData GetTemporalWaterData() {
	return WaterDefinition::WaterData{
		.waveHeight = 0.3f,
		.waveSpeed = 0.3f,
		.waveLength = 25.0f,
		.waveFrequency = 0.8f,
		.waterDensity = 1.0f,
		.waterViscosity = 1.0f,
		.reflectionIntensity = 1.0f, // Valores por defecto para futuros efectos
		.refractionIndex = 1.333f, // El índice de refracción del agua
		.foamAmount = 0.0f, // Desactivado por ahora
		.causticsIntensity = 0.0f, // Desactivado por ahora
		.waterColor = { 0.0f, 0.5f, 0.8f, 1.0f },
		.isUnderwater = FALSE,
		.hasFoam = FALSE,
		.hasCaustics = FALSE,
		.isReflective = FALSE,
		.isRefractive = FALSE,
	};
}

// ----------------------------------------------------------
// EXEC PASS
// ----------------------------------------------------------
std::vector<PipelineOperationType> WaterPass::ExecPass(const MeshAsset* mesh, FrameStateService* renderState) {
	ClearOperations();
	AddOperation(PipelineOperationType::Device_SetConstantsBufferState);
	AddOperation(PipelineOperationType::Mesh_Render_SetVertexShader);
	AddOperation(PipelineOperationType::Mesh_Render_SetPixelShader);
	AddOperation(PipelineOperationType::Mesh_Render_SetInputLayout);
	//if (needShadowMap) {
		//AddOperation(PipelineOperationType::Mesh_Render_SetTexture);
	//}
	AddOperation(PipelineOperationType::Mesh_Render_SetSampler);
	AddOperation(PipelineOperationType::Mesh_Render_SetVertexBuffer);
	AddOperation(PipelineOperationType::Mesh_Render_SetIndexBuffer);
	AddOperation(PipelineOperationType::Mesh_Render_SetPrimitiveToplogy);
	AddOperation(PipelineOperationType::Device_UpdateConstantsBufferResource);
    AddOperation(PipelineOperationType::Device_draw);
	//Draw(mesh, drawData);
	return GetOperations();
}

//std::vector<PipelineOperationType> WaterPass::ExecPass(const MeshAsset* mesh, FrameStateService* renderState) {
//	ClearOperations();
//
//	const Material* material = mesh->GetMaterial();
//	shaderName = StringToWstring(mesh->GetShaderName());
//	bool needShadowMap = m_shaderManager->NeedsShadow(shaderName);
//	std::string shadowTextureViewName = config->shadowTextureView;
//
//	// 1. Set Constant buffers
//	std::shared_ptr<MatrixParams> bData = std::make_shared<MatrixParams>();
//	std::shared_ptr<LightMatrixParams> lData = std::make_shared<LightMatrixParams>();
//	std::shared_ptr<MaterialMatrixParams> mData = std::make_shared<MaterialMatrixParams>();
//	std::shared_ptr<WaterMatrixParams> wData = std::make_shared<WaterMatrixParams>();
//
//	if (shaderName == L"SkyboxShader") {
//		OutputDebugStringA("Error: UIText::Render - no UIManager available.\n");
//	}
//
//	bData->worldMatrix = renderState->CommonState()->GetWorldMatrix();
//	bData->viewMatrix = DirectX::XMMatrixTranspose(renderState->CameraState()->GetViewMatrix());
//	bData->projectionMatrix = DirectX::XMMatrixTranspose(renderState->CameraState()->GetProjectionMatrix());
//	bData->cameraPosition = renderState->CameraState()->GetCameraPosition();
//	lData->lightDirection = renderState->LightState()->GetLightDirection();
//	lData->lightColor = renderState->LightState()->GetLightColor();
//	wData->waterData = renderState->WaterState()->GetWaterData();
//	bData->timeData = renderState->TimeState()->GetTimeData();
//	// bData->timeData = TimeDefinition::TimeData{ .time = DateTime::GetTimeInSeconds() };
//	wData->waterInstancingData = {};
//	static const int MAX_INSTANCES = 256;
//	std::vector<DirectX::XMMATRIX> sourceMatrices = renderState->WaterState()->GetWaterTileTransformMatrices();
//
//	DirectX::XMMATRIX instanceWorldMatrix[MAX_INSTANCES]{};    // destino
//	size_t count = std::min(sourceMatrices.size(), size_t(255));
//	for (size_t i = 0; i < count; ++i) {
//		wData->waterInstancingData.instanceWorldMatrix[i] = sourceMatrices[i];
//	}
//		
//	PipelineMatrixBufferData matrixData = {};
//	matrixData.params[MATRIX_PARAM_BASE.data()] = bData;
//	matrixData.params[MATRIX_PARAM_LIGHT.data()] = lData;
//	matrixData.params[MATRIX_PARAM_MATERIAL.data()] = mData;
//	matrixData.params[MATRIX_PARAM_WATER.data()] = wData;
//	matrixData.constantsBuffers = material->GetConstantBuffers();
//	m_shaderManager->GetMatrixDefinitions(shaderName, matrixData.matrices);
//
//	AddOperation(PipelineOperationType::Device_SetConstantsBufferState, matrixData);
//
//	// 2. Set VertexShader
//	PipelineVertexShaderData vsData = {};
//	vsData.data = material->GetVertexShader().Get();
//	AddOperation(PipelineOperationType::Mesh_Render_SetVertexShader, vsData);
//
//	// 3. Set PixelShader
//	PipelinePixelShaderData psData = {};
//	psData.data = material->GetPixelShader().Get();
//	AddOperation(PipelineOperationType::Mesh_Render_SetPixelShader, psData);
//
//	// 4. Set InputLayout
//	PipelineLayoutData lyData = {};
//	lyData.data = material->GetInputLayout();
//	AddOperation(PipelineOperationType::Mesh_Render_SetInputLayout, lyData);
//
//	//// 5. Set Textures
//	//PipelineTextureData tData = {};
//	//tData.data = {};
//	//tData.numTextures = material->GetNumTextures();
//	//tData.data = material->GetTextures();
//	//AddOperation(PipelineOperationType::Mesh_Render_SetTexture, tData);
//
//	// 5.1 Set Shadow map texture if exists
//	if (needShadowMap) {
//		PipelineTextureData shadowData = {};
//		shadowData.data = {};
//		shadowData.data.push_back(m_initManager->GetShaderResourceView(shadowTextureViewName).Get());
//		shadowData.numTextures = 1;
//		shadowData.startSlot = 1;
//		AddOperation(PipelineOperationType::Mesh_Render_SetTexture, shadowData);
//	}
//
//	// 6. Set Sampler
//	PipelineSamplerSateData spData = {};
//	spData.data = m_initManager->GetSamplerStates(WstringToString(shaderName));
//	AddOperation(PipelineOperationType::Mesh_Render_SetSampler, spData);
//
//	// 7. Set VertexBuffer
//	PipelineSetVertexBufferData vData = {};
//	vData.stride = mesh->GetVertexTypeSize();
//	vData.vertexBuffer = mesh->GetVertexBuffer();
//	AddOperation(PipelineOperationType::Mesh_Render_SetVertexBuffer, vData);
//
//	// 8. Set IndexBuffer
//	PipelineSetIndexBufferData iData = {};
//	iData.indexBuffer = mesh->GetIndexBuffer();
//	AddOperation(PipelineOperationType::Mesh_Render_SetIndexBuffer, iData);
//
//	// 9. Primitive topology
//	PipelinePrimitiveTopologyData ptData = {};
//	ptData.data = static_cast<D3D_PRIMITIVE_TOPOLOGY>(config->primitiveTopology);
//	AddOperation(PipelineOperationType::Mesh_Render_SetPrimitiveToplogy, ptData);
//
//	// 10. DrawIndexed
//	
//	//std::map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>> buffers = material->GetConstantBuffers();
//
//	PipelineUpdateMatrixBufferData updateMatrixData = {};
//	//Microsoft::WRL::ComPtr<ID3D11Buffer> buffer = buffers["WaterInstance"];
//	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer = renderState->MaterialState()->GetConstantBuffer("WaterInstance");
//
//	updateMatrixData.constantsBuffer = buffer;
//	updateMatrixData.data = wData;
//	AddOperation(PipelineOperationType::Device_UpdateConstantsBufferResource, updateMatrixData);
//
//	PipelineDrawInstancedData drawData = {};
//	drawData.instanceCount = renderState->MeshState()->GetNumberOfInstances();
//	drawData.vertexCountPerInstance = mesh->GetVertexCount();
//	drawData.indexCountPerInstance = mesh->GetIndexCount();
//	/*drawData.instanceCount = renderData.waterMatrices.size();
//	drawData.vertexCountPerInstance = mesh->GetVertexCount();
//	drawData.indexCountPerInstance = mesh->GetIndexCount();*/
//
//	Draw(mesh, drawData);
//
//	/*PipelineDrawIndexedData drawData = {};
//	drawData.numIndexes = mesh->GetIndexCount();
//	AddOperation(PipelineOperationType::Device_drawIndexed, drawData);*/
//
//	return GetOperations();
//}

std::vector<PipelineOperationType> WaterPass::EndPass() {
	// Operaciones de finalización del pase: Limpieza de buffers y reciclaje de chunks    
    // Liberar los buffers dinámicos
    /*AddOperation(PipelineOperationType::Mesh_Render_ResetVertexBuffer);
    AddOperation(PipelineOperationType::Mesh_Render_ResetIndexBuffer);
    AddOperation(PipelineOperationType::Mesh_Render_Reset_Textures);    */
    return {};
}