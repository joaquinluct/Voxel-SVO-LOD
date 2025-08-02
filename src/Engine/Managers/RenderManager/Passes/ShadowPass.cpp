#include "ShadowPass.h"
#include <ManagerLocator/ManagerLocator.h>
#include <ServiceLocator/ServiceLocator.h>
#include <Game/Systems/Shadows.h>
#include <Util/Text/Text.h>
#include <Defines/Light.h>
#include <Locators/Registers/REGISTER_RENDER_PASS_MACRO.h>

REGISTER_RENDER_PASS_TYPE(ShadowPass, "ShadowPass")

void ShadowPass::SetInitialOperations() {
	// 0 Obtener valores de configuración
	float resolution = static_cast<float>(config->resolution);
	std::string shaderName = config->shader_name;
	//std::string cullMode = config->cull_mode;

	// 1. Limpiar Render Target
	PipelineDepthStencilData dscData = {};
	dscData.data = m_initManager->GetDepthStencilView(config->stencilState);
	AddInitialOperation(PipelineOperationType::Device_ClearDepthStencilView, dscData);

	// 1.1. Set Render target view
	PipelineSetRenderTargetsData srData = {};
	srData.targetView = m_initManager->GetRenderTargetView(config->viewPortState);
	srData.stencilView = m_initManager->GetDepthStencilView(config->stencilState);
	srData.isColorPass = false;
	AddInitialOperation(PipelineOperationType::Device_Init_SetRenderTargetView, srData);

	// 2. Configurar Viewport;
	PipelineViewPortData vpData = {};
	vpData.desc = m_initManager->GetViewport(config->viewPortState);
	AddInitialOperation(PipelineOperationType::Device_SetViewport, vpData);

	// 3. Configurar RasterizedState
	PipelineRasteriezeData rData = {};
	rData.desc = {};
	rData.state = m_initManager->GetRasterizerState(config->rasterizedState);
	//params.rasterize.desc.DepthBias = static_cast<int>(config->depth_bias);
	/*if (cullMode == "front") params.rasterize.data.CullMode = D3D11_CULL_FRONT;
	if (cullMode == "back") params.rasterize.data.CullMode = D3D11_CULL_BACK;*/
	AddInitialOperation(PipelineOperationType::Device_SetRasterizedState, rData);

	// 4. Configurar DepthStencil
	PipelineDepthStencilData dsData = {};
	dsData.data = m_initManager->GetDepthStencilView(config->stencilState);
	dsData.desc = {};
	dsData.desc.Height = config->resolution;
	dsData.desc.Width = config->resolution;
	dsData.desc.MipLevels = 1;
	dsData.desc.ArraySize = 1;
	dsData.desc.Format = DXGI_FORMAT_R32_TYPELESS;
	dsData.desc.SampleDesc.Quality = 0;
	dsData.desc.SampleDesc.Count = 1;
	dsData.desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	dsData.desc.CPUAccessFlags = 0;
	dsData.desc.MiscFlags = 0;
	dsData.clearFlags = D3D11_CLEAR_DEPTH;
	AddInitialOperation(PipelineOperationType::Device_SetDepthStencilState, dsData);

	// 5. Activar Shader
	/*PipelineVertexShaderData vsData = {};
	vsData.data = m_shaderManager->GetVertexShader(StringToWstring(shaderName));
	AddInitialOperation(PipelineOperationType::Device_Init_SetPixelShader, vsData);*/

	// 6. Configurar Input Layout
	//params.layout.desc = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	PipelineLayoutData lData = {};
	lData.numElements = 1;
	AddInitialOperation(PipelineOperationType::Device_Init_SetLayout, lData);

	// 7. Bind Constants buffers.
	PipelineMaterialBufferData mData = {};	
	mData.worldMatrix = XMMatrixIdentity();;
	mData.lightViewProjectionMatrix = m_shadows->GetLightViewProjectionMatrix();
	AddInitialOperation(PipelineOperationType::Device_Init_ConstantsBuffers, mData);
}

HRESULT ShadowPass::InitManagers() {
	m_deviceManager = ManagerLocator::GetDeviceManager();
	if (!m_deviceManager) {
		return E_FAIL;
	}
	m_renderTargetManager = ManagerLocator::GetRenderManager();
	if (!m_renderTargetManager) {
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
	m_initManager = ManagerLocator::GetManager<InitManager>();
	if (!m_initManager) {
		return E_FAIL;
	}
	return S_OK;
}

HRESULT ShadowPass::Init() {
	config = std::make_shared<ShadowPassConfig>(ShadowPassConfig{});
	if (!config) { return E_FAIL; };
	HRESULT hr = InitManagers();
	if (FAILED(hr)) {
		return hr;
	}
	SetInitialOperations();
	return hr;
}

std::vector<std::shared_ptr<PipelineOperation>> ShadowPass::BeginPass()
{
	return GetInitialOperations();
}

std::vector<std::shared_ptr<PipelineOperation>> ShadowPass::ExecPass(std::shared_ptr<MeshAsset> mesh)
{
	int primitiveTopology = config->primitiveTopology;

	ClearOperations();
	
	PipelineOperationParams params = {};
	// 1. Constants buffers
	PipelineMaterialBufferData mData = {};
	mData.worldMatrix = mesh->GetWorldMatrix();
	AddOperation(PipelineOperationType::Mesh_Render_BindConstantsBuffers, mData);
	// 2. Vertext buffer
	PipelineSetVertexBufferData vData = {};
	vData.vertexBuffer = mesh->GetVertexBuffer();
	vData.stride = mesh->GetVertexTypeSize();
	AddOperation(PipelineOperationType::Mesh_Render_SetVertexBuffer, vData);
	// 3. Index buffer
	PipelineSetIndexBufferData iData = {};
	iData.indexBuffer = mesh->GetIndexBuffer();
	AddOperation(PipelineOperationType::Mesh_Render_SetIndexBuffer, iData);
	// 4. Primitive topology
	PipelinePrimitiveTopologyData ptData = {};
	ptData.data = static_cast<D3D_PRIMITIVE_TOPOLOGY>(primitiveTopology);
	AddOperation(PipelineOperationType::Mesh_Render_SetPrimitiveToplogy, ptData);
	// 5. Draw indexes
	PipelineDrawIndexedData drawData = {};
	drawData.numIndexes = mesh->GetIndexCount();
	AddOperation(PipelineOperationType::Device_drawIndexed, drawData);
	//ServiceLocator::
	return GetOperations();
}

std::vector<std::shared_ptr<PipelineOperation>> ShadowPass::EndPass()
{
	// 1. Desvincular Render Target / Depth-Stencil View del Mapa de Sombras
	// (Opcional, pero bueno para liberar el recurso para usarlo como SRV): 
	// Device_SetRenderTargets(Establecer a nullptr para RT y nullptr para DSV).

	// 2. Restaurar Viewport a default / main
	// (Opcional, pero buena práctica si el siguiente pase tiene un viewport diferente) :
	// Mesh_Render_SetViewport(Al Viewport de la ventana principal).
	return {};

}