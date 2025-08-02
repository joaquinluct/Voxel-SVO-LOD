#include "MainColorPass.h"
#include <ManagerLocator/ManagerLocator.h>
#include <ServiceLocator/ServiceLocator.h>
#include <Locators/Pipeline/PipelineStateLocator.h>
#include <Game/Systems/Shadows.h>
#include <Game/Systems/Lighting.h>
#include <Util/Text/Text.h>
#include <Defines/Light.h>
#include <Locators/Registers/REGISTER_RENDER_PASS_MACRO.h>

REGISTER_RENDER_PASS_TYPE(MainColorPass, "MainColorPass")

void MainColorPass::SetInitialOperations() {

	//std::string cullMode = config->cull_mode;
	float width = m_deviceManager->GetWidth();
	float height = m_deviceManager->GetHeight();	
	int primitiveTopology = config->primitiveTopology;
	std::string stencilName = config->stencilState;

	PipelineOperationParams params = {};

	// 1. Configurar viewport
	PipelineViewPortData vpData = {};
	vpData.desc = m_initManager->GetViewport(config->viewPortState);
	AddInitialOperation(PipelineOperationType::Device_SetViewport, vpData);

	// 2. Set Render target view
	PipelineSetRenderTargetsData srData = {};
	srData.targetView = m_initManager->GetRenderTargetView(config->viewPortState);
	srData.stencilView = m_initManager->GetDepthStencilView(config->stencilState);
	srData.isColorPass = true;
	AddInitialOperation(PipelineOperationType::Device_Init_SetRenderTargetView, srData);

	// 3. Borrar Render target view y Depth stencil
	PipelineRenderTargetViewData rtvData = {};
	rtvData.data = m_initManager->GetRenderTargetView(config->viewPortState);
	rtvData.clearColor = XMFLOAT4{ .4f,.4f,.0f,1.0f };
	AddInitialOperation(PipelineOperationType::Device_ClearRenderTargetView, rtvData);
	PipelineDepthStencilData dscData = m_initManager->GetDepthStencilData(stencilName);	
	dscData.data = m_initManager->GetDepthStencilView(stencilName);
	dscData.clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;	
	AddInitialOperation(PipelineOperationType::Device_ClearDepthStencilView, dscData);

	return;

	// 3. Raterized state
	std::shared_ptr<IPipelineRasterizedState> state = PipelineStateLocator::GetPipelineState<IPipelineRasterizedState>(config->rasterizedState);
	PipelineRasteriezeData rData = {};
	rData.desc = {};
	rData.desc.FillMode = static_cast<D3D11_FILL_MODE>(state->FillMode);
	//rData.desc.CullMode = static_cast<D3D11_CULL_MODE>(state->CullMode);
	rData.desc.CullMode = D3D11_CULL_NONE;
	rData.desc.FrontCounterClockwise = static_cast<BOOL>(state->FrontCounterClockwise);
	rData.desc.AntialiasedLineEnable = static_cast<BOOL>(state->AntialiasedLineEnable);
	rData.desc.DepthClipEnable = static_cast<BOOL>(state->DepthClipEnable);
	rData.state = m_initManager->GetRasterizerState(config->rasterizedState);	
	/*if (cullMode == "front") params.rasterize.data.CullMode = D3D11_CULL_FRONT;
	if (cullMode == "back") params.rasterize.data.CullMode = D3D11_CULL_BACK;*/
	AddInitialOperation(PipelineOperationType::Device_SetRasterizedState, rData);

	// 4. Depth stencil
	PipelineDepthStencilData dsData = {};
	dsData.data = m_initManager->GetDepthStencilView(config->stencilState);
	dsData.desc = {};
	dsData.desc.Height = static_cast<UINT>(height);
	dsData.desc.Width = static_cast<UINT>(width);
	dsData.desc.MipLevels = 1;
	dsData.desc.ArraySize = 1;
	dsData.desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsData.desc.SampleDesc.Count = 1;
	dsData.desc.SampleDesc.Quality = 0;
	dsData.desc.Usage = D3D11_USAGE_DEFAULT;
	dsData.desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dsData.clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
	//AddInitialOperation(PipelineOperationType::Device_SetDepthStencilState, dsData);
	AddInitialOperation(PipelineOperationType::Device_ClearDepthStencilView, dsData);

	// 5. Topology
	PipelinePrimitiveTopologyData ptData = {};
	ptData.data = static_cast<D3D_PRIMITIVE_TOPOLOGY>(primitiveTopology);
	AddInitialOperation(PipelineOperationType::Mesh_Render_SetPrimitiveToplogy, ptData);
}

HRESULT MainColorPass::InitManagers() {
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
	m_renderTargetManager = ManagerLocator::GetRenderManager();
	if (!m_shaderManager) {
		return E_FAIL;
	}
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

HRESULT MainColorPass::Init() {
	config = std::make_shared<MainColorPassConfig>(MainColorPassConfig{});
	if (!config) { return E_FAIL; };
	HRESULT hr = InitManagers();
	if (FAILED(hr)) {
		return hr;
	}
	SetInitialOperations();
	return hr;
}

std::vector<std::shared_ptr<PipelineOperation>> MainColorPass::BeginPass()
{
	PipelineOperationParams params = {};

	// 1. Constants buffers generales
	params.matrices.cameraPosition = m_cameraManager->GetCurrentCameraPosition();
	params.matrices.viewMatrix = m_cameraManager->GetCurrentViewMatrix();
	params.matrices.projectionMatrix = m_cameraManager->GetCurrentProjectionMatrix();
	params.matrices.lightColor = m_lighthing->GetLightColor();
	params.matrices.lightDirection = m_lighthing->GetLightDirection();

	// 2. Operacines fijas definidas al inicio
	std::vector<std::shared_ptr<PipelineOperation>> init = GetInitialOperations();

	return init;
}

std::vector<std::shared_ptr<PipelineOperation>> MainColorPass::ExecPass(std::shared_ptr<MeshAsset> mesh)
{
	return {};
	shaderName = StringToWstring(mesh->GetShaderName());

	ClearOperations();

	PipelineOperationParams params = {};

	// 1. Set VertexShader;	
	PipelineVertexShaderData vsData = {};
	vsData.data = m_shaderManager->GetVertexShader(shaderName);
	AddOperation(PipelineOperationType::Mesh_Render_SetVertexShader, vsData);
	// 2. Set PixelShader;
	PipelinePixelShaderData psData = {};
	psData.data = m_shaderManager->GetPixelShader(shaderName);
	AddOperation(PipelineOperationType::Mesh_Render_SetPixelShader, psData);
	// 3. Set Layout;
	PipelineLayoutData lData = {};
	lData.data = mesh->GetMaterial()->GetInputLayout();
	AddOperation(PipelineOperationType::Mesh_Render_SetInputLayout, lData);
	// 4. Set Sampler
	PipelineSamplerSateData spData = {};
	spData.data = mesh->GetMaterial()->GetSamplerState();
	AddOperation(PipelineOperationType::Mesh_Render_SetSampler, spData);
	// 5. Constants buffers
	MatrixDefinitionBase::MatrixParams mData = {};	

	mData.worldMatrix = mesh->GetWorldMatrix();
	mData.viewMatrix = XMMatrixTranspose(m_cameraManager->GetCurrentViewMatrix());
	mData.projectionMatrix = XMMatrixTranspose(m_cameraManager->GetCurrentProjectionMatrix());
	mData.cameraPosition = m_cameraManager->GetCurrentCameraPosition();
	mData.lightDirection = m_lighthing->GetLightDirection();
	mData.lightColor = m_lighthing->GetLightColor();
	mData.materialAO = 0.4f;
	/*mesh->GetMaterial()->SetConstantBuffers(m_deviceManager->GetContext(), mData);
	mesh->GetMaterial()->Apply(m_deviceManager->GetContext());*/
	/*unsigned int oper = static_cast<unsigned int>(PipelineMatrixBufferType::WorldMatrix) | static_cast<unsigned int>(PipelineMatrixBufferType::ViewMatrix) | static_cast<unsigned int>(PipelineMatrixBufferType::ProjectionMatrix) | static_cast<unsigned int>(PipelineMatrixBufferType::CameraPosition) | static_cast<unsigned int>(PipelineMatrixBufferType::LightDirection) | static_cast<unsigned int>(PipelineMatrixBufferType::LightColor) | static_cast<unsigned int>(PipelineMatrixBufferType::MaterialAO);*/
	//mData.oper = oper;
	PipelineMatrixBufferData matrixData = {};
	matrixData.data = mData;
	matrixData.material = mesh->GetMaterial();

	AddOperation(PipelineOperationType::Device_SetConstantsBufferState, matrixData);

	// 6. Set Textures
	PipelineTextureData tData = {};
	tData.data = {};
	tData.numTextures = mesh->GetMaterial()->GetNumTextures();
	tData.data = mesh->GetMaterial()->GetTextures();
	AddOperation(PipelineOperationType::Mesh_Render_SetTexture, tData);
	// 7. Set VertexBuffer
	PipelineSetVertexBufferData vData = {};	
	vData.stride = mesh->GetVertexTypeSize();
	vData.vertexBuffer = mesh->GetVertexBuffer();
	AddOperation(PipelineOperationType::Mesh_Render_SetVertexBuffer, vData);
	// 8. Set IndexBuffer
	PipelineSetIndexBufferData iData = {};	
	iData.indexBuffer = mesh->GetIndexBuffer();
	AddOperation(PipelineOperationType::Mesh_Render_SetIndexBuffer, iData);
	// 9. Primitive topology
	PipelinePrimitiveTopologyData ptData = {};
	ptData.data = static_cast<D3D_PRIMITIVE_TOPOLOGY>(config->primitiveTopology);
	AddOperation(PipelineOperationType::Mesh_Render_SetPrimitiveToplogy, ptData);
	// 10. DrawIndexed
	PipelineDrawIndexedData drawData = {};
	drawData.numIndexes = mesh->GetIndexCount();
	AddOperation(PipelineOperationType::Device_drawIndexed, drawData);

	return GetOperations();
}

std::vector<std::shared_ptr<PipelineOperation>> MainColorPass::EndPass()
{
	return {};
}