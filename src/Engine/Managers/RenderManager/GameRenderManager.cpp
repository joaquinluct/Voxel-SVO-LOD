#include "GameRenderManager.h"
#include <AssetLocator/AssetLocator.h>
#include <ConfigLocator/ConfigLocator.h>
#include <ServiceLocator/ServiceLocator.h>
#include <ManagerLocator/ManagerLocator.h>
#include <Assets/Base/MeshAsset.h>
#include <UI/UIText.h>

GameRenderManager::GameRenderManager() : m_shadows{nullptr}, m_deviceManager{nullptr} {
	m_gameConfig = new GameEngineConfig();
}

GameRenderManager::~GameRenderManager() {

}

HRESULT GameRenderManager::Init() {

	m_deviceManager = ManagerLocator::GetDeviceManager();
	if (!m_deviceManager) {
		return E_FAIL; // Device manager service not available
	}

	m_shadows = ServiceLocator::GetService<Shadows>();
	if (!m_shadows) {
		return E_FAIL; // Shadows service not available
	}

	return S_OK;
}

void GameRenderManager::BeginRender() {
}

void GameRenderManager::Render() {
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context = m_deviceManager->GetContext();

	// PASE DE SOMBRAS
	// ---------------
	/*if (m_shadows) {
		m_currentPass = RenderPass::Shadows;
		m_deviceManager->SetRasterizerShadowsState();
		m_shadows->BeginShadowPass();
		ServiceLocator::RenderShadowPassServices(m_gameConfig->services_render_order);
		m_shadows->EndShadowPass();
	}*/

	// PASE DE COLORES
	// ---------------
	m_deviceManager->SetRasterizerState();
	
	for(const auto& meshPair : m_renderMesh) {
		auto mesh = meshPair.second;
		if (mesh) {
			RenderMesh(context, mesh);
		}
	}
}

void GameRenderManager::EndRender() {
	// Cleanup or finalize rendering here
}

void GameRenderManager::Update(float deltaTime) {
	// Update game-specific rendering logic here
}

void GameRenderManager::Shutdown() {
	// Release game-specific rendering resources here
}

std::shared_ptr<MeshAsset> GameRenderManager::RegisterMesh(const std::string meshName) {
	auto mesh = std::static_pointer_cast<MeshAsset>(AssetLocator::GetAsset(meshName));
	m_renderMesh[meshName] = mesh;
	return mesh;
}

std::shared_ptr<UIText> GameRenderManager::RegisterTextMesh(const std::string& meshName) {
	std::shared_ptr<MeshAsset> mesh = RegisterMesh(meshName);
	mesh->Init();
	std::shared_ptr<MeshAssetConfigBase> meshConfig = mesh->GetConfig();
	if (meshConfig && meshConfig->textService.size() > 0) {
		// If the mesh is a text mesh, we can register it as a UIText
		std::shared_ptr<IService> textMesh = ServiceLocator::GetService(meshConfig->textService);
		std::shared_ptr<UIText> textService = std::dynamic_pointer_cast<UIText>(textMesh);
		m_renderTextMesh[meshName] = { mesh, textService };
		return textService;
	}
	return nullptr;
}

void GameRenderManager::SetConstantBuffers() {

}

std::map<std::string, std::shared_ptr<MeshAsset>> GameRenderManager::GetMeshesByRenderPass(RenderPassType renderPassType) {
	std::map<std::string, std::shared_ptr<MeshAsset>> meshesByPass;
	for(const auto& mesh : m_renderMesh) {
		if (mesh.second->IsRenderPassEnabled(renderPassType)) {
			meshesByPass[mesh.first] = mesh.second;
		}
	}
	return meshesByPass;
};

std::map<std::string, std::shared_ptr<MeshAsset>> GameRenderManager::GetCastShadowMeshes() {
	std::map<std::string, std::shared_ptr<MeshAsset>> m_castShadowMeshes;
	for (const auto& mesh : m_renderMesh) {
		if (mesh.second->CastShadows()) {
			m_castShadowMeshes[mesh.first] = mesh.second;
		}
	}
	return m_castShadowMeshes;
};

void GameRenderManager::RenderMesh(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, const std::shared_ptr<MeshAsset> mesh) {
	Microsoft::WRL::ComPtr<ID3D11Buffer> vb = mesh->GetVertexBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer> ib = mesh->GetIndexBuffer();
	UINT vertexTypeSize = mesh->GetVertexTypeSize();
	UINT indexCount = mesh->GetIndexCount();

	UINT stride = vertexTypeSize;
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	context->IASetIndexBuffer(ib.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(indexCount, 0, 0);
}