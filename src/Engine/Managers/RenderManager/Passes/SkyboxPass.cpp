#include "SkyboxPass.h"
#include <Assets/Base/MeshAsset.h>
#include <Game/Systems/Lighting.h>
#include <Game/Systems/Shadows.h>
#include <Locators/Registers/REGISTER_RENDER_PASS_MACRO.h>
#include <ManagerLocator/ManagerLocator.h>
#include <ServiceLocator/ServiceLocator.h>
#include <Services/FrameStateService.h>

REGISTER_RENDER_PASS_TYPE(SkyboxPass, "SkyboxPass")

void SkyboxPass::SetInitialOperations() {
    AddInitialOperation(PipelineOperationType::Device_SetViewport);
    AddInitialOperation(PipelineOperationType::Device_Init_SetRenderTargetView);
    AddInitialOperation(PipelineOperationType::Device_ClearRenderTargetView);
    AddInitialOperation(PipelineOperationType::Device_SetRasterizedState);
    AddInitialOperation(PipelineOperationType::Device_ClearDepthStencilView);
}

//void SkyboxPass::SetInitialOperations() {
//
//	//std::string cullMode = config->cull_mode;
//	float width = m_deviceManager->GetWidth();
//	float height = m_deviceManager->GetHeight();	
//	int primitiveTopology = config->primitiveTopology;
//	std::string stencilName = config->stencilState;
//
//	// 1. Configurar viewport
//	PipelineViewPortData vpData = {};
//	vpData.desc = m_initManager->GetViewport(config->viewPortState);
//	AddInitialOperation(PipelineOperationType::Device_SetViewport, vpData);
//
//	// 2. Set Render target view
//	PipelineSetRenderTargetsData srData = {};
//	srData.targetView = m_initManager->GetRenderTargetView(config->viewPortState);
//	srData.stencilView = m_initManager->GetDepthStencilView(config->stencilState);
//	srData.isColorPass = true;
//	AddInitialOperation(PipelineOperationType::Device_Init_SetRenderTargetView, srData);
//
//	// 3. Borrar Render target view y Depth stencil
//	PipelineRenderTargetViewData rtvData = {};
//	rtvData.data = m_initManager->GetRenderTargetView(config->viewPortState);
//	rtvData.clearColor = XMFLOAT4{ .4f,.4f,.0f,1.0f };
//	AddInitialOperation(PipelineOperationType::Device_ClearRenderTargetView, rtvData);
//
//	/*PipelineDepthStencilData dscData = m_initManager->GetDepthStencilData(stencilName);	
//	dscData.stencilViewData = m_initManager->GetDepthStencilView(stencilName);
//	dscData.clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;	
//	AddInitialOperation(PipelineOperationType::Device_ClearDepthStencilView, dscData);*/
//	
//	// 3. Raterized state
//	std::shared_ptr<IPipelineRasterizedState> state = PipelineStateLocator::GetPipelineState<IPipelineRasterizedState>(config->rasterizedState);
//	PipelineRasteriezeData rData = {};
//	rData.desc = {};
//	rData.desc.FillMode = static_cast<D3D11_FILL_MODE>(state->FillMode);
//	//rData.desc.CullMode = static_cast<D3D11_CULL_MODE>(state->CullMode);
//	rData.desc.CullMode = D3D11_CULL_BACK;
//	rData.desc.FrontCounterClockwise = static_cast<BOOL>(state->FrontCounterClockwise);
//	rData.desc.AntialiasedLineEnable = static_cast<BOOL>(state->AntialiasedLineEnable);
//	rData.desc.DepthClipEnable = static_cast<BOOL>(state->DepthClipEnable);
//	rData.state = m_initManager->GetRasterizerState(config->rasterizedState);	
//	/*if (cullMode == "front") params.rasterize.data.CullMode = D3D11_CULL_FRONT;
//	if (cullMode == "back") params.rasterize.data.CullMode = D3D11_CULL_BACK;*/
//	AddInitialOperation(PipelineOperationType::Device_SetRasterizedState, rData);
//
//	// 4. Depth stencil
//	PipelineDepthStencilData dscData = m_initManager->GetDepthStencilData(stencilName);	
//	dscData.stencilViewData = m_initManager->GetDepthStencilView(stencilName);
//	dscData.clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;	
//	AddInitialOperation(PipelineOperationType::Device_ClearDepthStencilView, dscData);
//
//	return;
//
//	//// 3. Raterized state
//	//std::shared_ptr<IPipelineRasterizedState> state = PipelineStateLocator::GetPipelineState<IPipelineRasterizedState>(config->rasterizedState);
//	//PipelineRasteriezeData rData = {};
//	//rData.desc = {};
//	//rData.desc.FillMode = static_cast<D3D11_FILL_MODE>(state->FillMode);
//	////rData.desc.CullMode = static_cast<D3D11_CULL_MODE>(state->CullMode);
//	//rData.desc.CullMode = D3D11_CULL_NONE;
//	//rData.desc.FrontCounterClockwise = static_cast<BOOL>(state->FrontCounterClockwise);
//	//rData.desc.AntialiasedLineEnable = static_cast<BOOL>(state->AntialiasedLineEnable);
//	//rData.desc.DepthClipEnable = static_cast<BOOL>(state->DepthClipEnable);
//	//rData.state = m_initManager->GetRasterizerState(config->rasterizedState);	
//	///*if (cullMode == "front") params.rasterize.data.CullMode = D3D11_CULL_FRONT;
//	//if (cullMode == "back") params.rasterize.data.CullMode = D3D11_CULL_BACK;*/
//	//AddInitialOperation(PipelineOperationType::Device_SetRasterizedState, rData);
//
//	//// 4. Depth stencil
//	//PipelineDepthStencilData dsData = {};
//	//dsData.data = m_initManager->GetDepthStencilView(config->stencilState);
//	//dsData.desc = {};
//	//dsData.desc.Height = static_cast<UINT>(height);
//	//dsData.desc.Width = static_cast<UINT>(width);
//	//dsData.desc.MipLevels = 1;
//	//dsData.desc.ArraySize = 1;
//	//dsData.desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	//dsData.desc.SampleDesc.Count = 1;
//	//dsData.desc.SampleDesc.Quality = 0;
//	//dsData.desc.Usage = D3D11_USAGE_DEFAULT;
//	//dsData.desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
//	//dsData.clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
//	////AddInitialOperation(PipelineOperationType::Device_SetDepthStencilState, dsData);
//	//AddInitialOperation(PipelineOperationType::Device_ClearDepthStencilView, dsData);
//
//	//// 5. Topology
//	//PipelinePrimitiveTopologyData ptData = {};
//	//ptData.data = static_cast<D3D_PRIMITIVE_TOPOLOGY>(primitiveTopology);
//	//AddInitialOperation(PipelineOperationType::Mesh_Render_SetPrimitiveToplogy, ptData);
//}

HRESULT SkyboxPass::InitManagers() {
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
    if (!m_shaderManager) {
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
    return S_OK;
}

HRESULT SkyboxPass::Init() {
    config = std::make_shared<SkyboxPassConfig>(SkyboxPassConfig{});
    if (!config) { return E_FAIL; };
    HRESULT hr = InitManagers();
    if (FAILED(hr)) {
        return hr;
    }
    SetInitialOperations();
    return hr;
}

std::map<std::string, std::shared_ptr<MeshAsset>> SkyboxPass::GetMeshes(const SceneManager* SceneManager, FrameStateService* renderState) {
    return {};
    ////return SceneManager->GetMeshes();
    //return SceneManager->GetMeshesByRenderPass(RenderPassType::Skybox);
}

std::vector<PipelineOperationType> SkyboxPass::BeginPass(const MeshAsset* mesh, FrameStateService* renderState)
{
    // 1. Operacines fijas definidas al inicio
    return GetInitialOperations();
}

std::vector<PipelineOperationType> SkyboxPass::ExecPass(const MeshAsset* mesh, FrameStateService* renderState)
{
    ClearOperations();

    AddOperation(PipelineOperationType::Device_SetConstantsBufferState);
    AddOperation(PipelineOperationType::Mesh_Render_SetVertexShader);
    AddOperation(PipelineOperationType::Mesh_Render_SetPixelShader);
    AddOperation(PipelineOperationType::Mesh_Render_SetInputLayout);
    AddOperation(PipelineOperationType::Mesh_Render_SetTexture);
    //if (needShadowMap) {
        //AddOperation(PipelineOperationType::Mesh_Render_SetTexture);
    //}
    AddOperation(PipelineOperationType::Mesh_Render_SetSampler);
    AddOperation(PipelineOperationType::Mesh_Render_SetVertexBuffer);
    AddOperation(PipelineOperationType::Mesh_Render_SetIndexBuffer);
    AddOperation(PipelineOperationType::Mesh_Render_SetPrimitiveToplogy);
    AddOperation(PipelineOperationType::Device_draw);

    return GetOperations();
}

//std::vector<PipelineOperationType> SkyboxPass::ExecPass(const MeshAsset* mesh, FrameStateService* renderState)
//{
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
//
//	if (shaderName == L"SkyboxShader") {
//		OutputDebugStringA("Error: UIText::Render - no UIManager available.\n");
//	}
//
//	bData->worldMatrix = renderState->CommonState()->GetWorldMatrix();
//
//	// 1. Obtener la matriz de vista sin transponer
//	XMMATRIX viewMatrix = renderData.viewMatrix;
//	XMMATRIX projectionMatrix = renderData.projectionMatrix;
//
//	// 2. Eliminar la traslación de la matriz (la 4ª fila, o viewMatrix.r[3])
//	viewMatrix.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
//
//	// 3. Transponer la matriz ya sin traslación
//	bData->viewMatrix = DirectX::XMMatrixTranspose(viewMatrix);
//
//	// La matriz de proyección no necesita cambios
//	bData->projectionMatrix = DirectX::XMMatrixTranspose(projectionMatrix);
//
//	// Calcular la matriz view-projection
//	bData->viewProjectionMatrix = DirectX::XMMatrixTranspose(XMMatrixMultiply(viewMatrix, projectionMatrix));
//	//bData->viewProjectionMatrix = DirectX::XMMatrixTranspose(XMMatrixMultiply(bData->viewMatrix, bData->projectionMatrix));
//	bData->cameraPosition = renderData.cameraPosition;
//	lData->lightDirection = renderData.lightDirection;
//	lData->lightColor = m_lighthing->GetLightColor();
//	mData->materialAO = 0.4f;
//	bData->skyboxData.skyColor = XMFLOAT4(0.4f, 0.4f, 0.9f, 1.0f);
//	bData->skyboxData.sunColor = XMFLOAT4(0.9f, 0.9f, 0.5f, 1.0f);
//	mData->textureTransform = mesh->GetTextureTransforms();
//
//	PipelineMatrixBufferData matrixData = {};
//	matrixData.params[MATRIX_PARAM_BASE.data()] = bData;
//	matrixData.params[MATRIX_PARAM_LIGHT.data()] = lData;
//	matrixData.params[MATRIX_PARAM_MATERIAL.data()] = mData;
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
//	// 5. Set Textures
//	PipelineTextureData tData = {};
//	tData.data = {};
//	tData.numTextures = material->GetNumTextures();
//	tData.data = material->GetTextures();
//	AddOperation(PipelineOperationType::Mesh_Render_SetTexture, tData);
//
//	// 5.1 Set Shadow map texture if exists
//	if (needShadowMap) {
//		PipelineTextureData shadowData = {};
//		shadowData.data = {};
//		shadowData.data.push_back(m_initManager->GetShaderResourceView(shadowTextureViewName).Get());
//		shadowData.numTextures = 1;
//		shadowData.startSlot = tData.numTextures;
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
//	Draw(mesh);
//	/*PipelineDrawIndexedData drawData = {};
//	drawData.numIndexes = mesh->GetIndexCount();
//	AddOperation(PipelineOperationType::Device_drawIndexed, drawData);*/
//
//	return GetOperations();
//}

std::vector<PipelineOperationType> SkyboxPass::EndPass()
{
    return {};
}
