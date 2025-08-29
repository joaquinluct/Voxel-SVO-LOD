#include "ShadowPass.h"
#include <Util/Text/Text.h>
#include <ManagerLocator/ManagerLocator.h>
#include <ServiceLocator/ServiceLocator.h>
#include <Locators/Pipeline/PipelineStateLocator.h>
#include <Game/Systems/Shadows.h>
#include <Game/Systems/Lighting.h>
#include <Assets/Base/MeshAsset.h>
//#include <Defines/Matrix/Shadow.h> // Asumo que tienes una definición para matrices de sombras
#include <Locators/Registers/REGISTER_RENDER_PASS_MACRO.h>

REGISTER_RENDER_PASS_TYPE(ShadowPass, "ShadowPass")

void ShadowPass::SetInitialOperations() {

	AddInitialOperation(PipelineOperationType::Device_SetViewport);
	AddInitialOperation(PipelineOperationType::Device_Init_SetRenderTargetView);	
	AddInitialOperation(PipelineOperationType::Device_ClearDepthStencilView);
	AddInitialOperation(PipelineOperationType::Device_SetRasterizedState);
	AddInitialOperation(PipelineOperationType::Device_SetDepthStencilState);
	AddInitialOperation(PipelineOperationType::Device_DisabledBledingState);
}

//void ShadowPass::SetInitialOperations() {
//
//    std::string stencilName = config->stencilState;
//
//	// 1. Set Viewport
//    PipelineViewPortData vpData = {};
//    vpData.desc = m_initManager->GetViewport(config->viewPortState);
//    AddInitialOperation(PipelineOperationType::Device_SetViewport, vpData);
//
//	// 2. Set RenderTargetView
//    PipelineSetRenderTargetsData srData = {};
//    srData.targetView = nullptr;
//    srData.stencilView = m_initManager->GetDepthStencilView(config->stencilState);
//    srData.isColorPass = false;
//    AddInitialOperation(PipelineOperationType::Device_Init_SetRenderTargetView, srData);
//
//    // Borrar el Depth Stencil view
//    // El color claro de profundidad es 1.0f (la distancia más lejana)
//    /*PipelineRenderTargetViewData rtvData = {};
//    rtvData.data = m_initManager->GetRenderTargetView(config->viewPortState);
//    rtvData.clearColor = XMFLOAT4{ .4f,.4f,.0f,1.0f };
//    AddInitialOperation(PipelineOperationType::Device_ClearRenderTargetView, rtvData);*/
//
//	// 3. Clear DepthStencilView
//    PipelineDepthStencilData dscData = m_initManager->GetDepthStencilData(stencilName);
//    dscData.stencilViewData = m_initManager->GetDepthStencilView(stencilName);
//    dscData.clearFlags = D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL;
//    AddInitialOperation(PipelineOperationType::Device_ClearDepthStencilView, dscData);
//
//	// 4. Raterized state
//	PipelineRasteriezeData rData = {};
//	rData.desc = {};
//	rData.state = m_initManager->GetRasterizerState(config->rasterizedState);
//	AddInitialOperation(PipelineOperationType::Device_SetRasterizedState, rData);
//
//	// 5. Stencil state
//	PipelineSetencilStateData pData = {};
//	pData.state = m_initManager->GetStencilState(config->stencilDef);
//	AddInitialOperation(PipelineOperationType::Device_SetDepthStencilState, pData);
//
//	// 6. Disable blending	
//	Microsoft::WRL::ComPtr<ID3D11BlendState> bData = m_initManager->GetBlendState(config->blendState);
//	PipelineBledingData bdData = {};
//	bdData.state = bData;
//	AddInitialOperation(PipelineOperationType::Device_DisabledBledingState, bdData);
//}

HRESULT ShadowPass::InitManagers() {
    m_deviceManager = ManagerLocator::GetDeviceManager();
    if (!m_deviceManager) {
        return E_FAIL;
    }
    m_shaderManager = ManagerLocator::GetShaderManager();
    if (!m_shaderManager) {
        return E_FAIL;
    }
	m_cameraManager = ManagerLocator::GetManager<CameraManager>();
	if (!m_cameraManager) {
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

HRESULT ShadowPass::Init() {
    config = std::make_shared<ShadowPassConfig>(ShadowPassConfig{}); // Asumo ShadowPassConfig
    if (!config) { return E_FAIL; };
    HRESULT hr = InitManagers();
    if (FAILED(hr)) {
        return hr;
    }
    SetInitialOperations();
    return hr;
}

std::map<std::string, std::shared_ptr<MeshAsset>> ShadowPass::GetMeshes(const SceneManager* SceneManager, FrameStateService* renderState) { 
	return {};
	////return SceneManager->GetCastShadowMeshes();
	//return SceneManager->GetMeshesByRenderPass(RenderPassType::ShadowPass);
}

std::vector<PipelineOperationType> ShadowPass::BeginPass(const MeshAsset* mesh, FrameStateService* renderState)
{
    return GetInitialOperations();
}

std::vector<PipelineOperationType> ShadowPass::ExecPass(const MeshAsset* mesh, FrameStateService* renderState)
{
	ClearOperations();

	AddOperation(PipelineOperationType::Device_ResetConstantsBuffers);
	AddOperation(PipelineOperationType::Device_SetConstantsBufferState);
	AddOperation(PipelineOperationType::Mesh_Render_SetVertexShader);
	AddOperation(PipelineOperationType::Mesh_Render_SetInputLayout);
	AddOperation(PipelineOperationType::Mesh_Render_Reset_Textures);
	AddOperation(PipelineOperationType::Mesh_Render_Reset_Sampler);
	AddOperation(PipelineOperationType::Mesh_Render_SetVertexBuffer);
	AddOperation(PipelineOperationType::Mesh_Render_SetIndexBuffer);
	AddOperation(PipelineOperationType::Mesh_Render_SetPrimitiveToplogy);
	AddOperation(PipelineOperationType::Device_draw);

	return GetOperations();
}

//std::vector<PipelineOperationType> ShadowPass::ExecPass(const MeshAsset* mesh, FrameStateService* renderState)
//{
//	ClearOperations();
//
//	const Material* material = mesh->GetShadowMaterial();
//	std::wstring shaderName = StringToWstring(config->shader_name);  //StringToWstring(mesh->GetShaderName());
//	
//	// 0. Vaciar constantes del shader actual
//	AddOperation(PipelineOperationType::Device_ResetConstantsBuffers);
//
//	// 1. Set Constant buffers
//	std::shared_ptr<MatrixParams> bData = std::make_shared<MatrixParams>();
//
//	//mData.worldMatrix = mesh->GetWorldMatrix();
//	/*mData.worldMatrix = XMMatrixIdentity();
//	mData.viewMatrix = DirectX::XMMatrixTranspose(m_cameraManager->GetCurrentViewMatrix());
//	mData.projectionMatrix = DirectX::XMMatrixTranspose(m_cameraManager->GetCurrentProjectionMatrix());
//	mData.cameraPosition = m_cameraManager->GetCurrentCameraPosition();
//	mData.lightDirection = m_lighthing->GetLightDirection();
//	mData.lightColor = m_lighthing->GetLightColor();
//	mData.materialAO = 0.4f;
//	mData.textureTransform = mesh->GetTextureTransforms();*/
//	bData->worldMatrix = mesh->GetWorldMatrix();
//	/*DirectX::XMFLOAT3 lightDirection = DirectX::XMFLOAT3{ 0.77f, 0.577f, 0.577f };
//	m_shadows->UpdateLightMatrices(lightDirection, m_cameraManager->GetCurrentCameraPosition(), 1000.0f);*/
//	m_shadows->UpdateLightMatrices(m_lighthing->GetLightDirection(), m_cameraManager->GetCurrentCameraPosition(), 1000.0f);
//	bData->lightViewProjectionMatrix = m_shadows->GetLightViewProjectionMatrix();
//
//	PipelineMatrixBufferData matrixData = {};
//	matrixData.params[MATRIX_PARAM_BASE.data()] = bData;
//	matrixData.constantsBuffers = material->GetConstantBuffers();
//	m_shaderManager->GetMatrixDefinitions(shaderName, matrixData.matrices);
//	AddOperation(PipelineOperationType::Device_SetConstantsBufferState, matrixData);
//
//	// 2. Set VertexShader
//	PipelineVertexShaderData vsData = {};
//	vsData.data = material->GetVertexShader().Get();
//	AddOperation(PipelineOperationType::Mesh_Render_SetVertexShader, vsData);
//
//	// 3. Set PixelShader --> No necesario en el pase de las sombras
//	/*PipelinePixelShaderData psData = {};
//	psData.data = material->GetPixelShader().Get();
//	AddOperation(PipelineOperationType::Mesh_Render_SetPixelShader, psData);*/
//
//	// 4. Set InputLayout
//	PipelineLayoutData lData = {};
//	lData.data = material->GetInputLayout();
//	AddOperation(PipelineOperationType::Mesh_Render_SetInputLayout, lData);
//
//	// 5. Reset Textures
//	AddOperation(PipelineOperationType::Mesh_Render_Reset_Textures);
//
//	// 6. Reset Samplers
//	AddOperation(PipelineOperationType::Mesh_Render_Reset_Sampler);
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
//	//AddOperation(PipelineOperationType::Device_drawIndexed, drawData);º
//
//	return GetOperations();
//}

std::vector<PipelineOperationType> ShadowPass::EndPass()
{
    // Limpiar los estados para el siguiente pase
	// 
	// Importante:
	// Este pase utiliza un Shader que genera un textura de profundidad (shadow map) y
	// ahora debe liberar en RenderTargetView y dejar solo el DepthStencilView para los siguientes pases, 
	// que la leer de esa textura, si no se desvincula, los siguientes pases no funcionan correctamente.

	AddOperation(PipelineOperationType::Device_Unbind_RenderTargetView);

	
    return {};
}