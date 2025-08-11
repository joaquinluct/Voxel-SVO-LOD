#pragma once

#include <string>
#include <map>
#include <vector>
#include <variant>
#include <memory>
#include <wrl/client.h>
#include <IManager.h>
#include <ILifeCycle.h>
#include <Game/Systems/Shadows.h>
#include <Config/Game/GameEngineConfig.h>
#include <Defines/Pass.h>
#include <Defines/Pipeline.h>

class MeshAsset;
class UIText;

class GameRenderManager : public IManager, public ILifeCycle {
public:
	GameRenderManager();
	~GameRenderManager() override;
	HRESULT Init() override;
	void Shutdown() override;
	void BeginRender();
	void Render() override;
	void EndRender();
	void Update(float deltaTime) override;

	void RenderMesh(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, const std::shared_ptr<MeshAsset> mesh);
	void SetConstantBuffers();

	const std::string& GetManagerName() const override {
		static const std::string name = "GameRenderManager";
		return name;
	}
	static const std::string& GetStaticManagerName()
	{
		static const std::string name = "GameRenderManager";
		return name;
	}

	bool IsRenderColourPassActive() const {
		return m_currentPass == RenderPassType::MainColorPass;
	}
	bool IsRenderShadowsPassActive() const {
		return m_currentPass == RenderPassType::ShadowPass;
	}

	void RegisterMesh(const std::string& meshName, std::shared_ptr<MeshAsset> meshAsset) {
		m_renderMesh[meshName] = meshAsset;
	}
	std::shared_ptr<MeshAsset> RegisterMesh(const std::string meshName);
	std::shared_ptr<UIText> RegisterTextMesh(const std::string& meshName);

	void UnregisterMesh(const std::string& meshName) {
		m_renderMesh.erase(meshName);
	}

	std::map<std::string, std::shared_ptr<MeshAsset>> GetMeshes() {
		return m_renderMesh;
	};

	std::map<std::string, std::shared_ptr<MeshAsset>> GetMeshesByRenderPass(RenderPassType renderPassType);

	std::map<std::string, std::shared_ptr<MeshAsset>> GetTextMeshes() {
		std::map<std::string, std::shared_ptr<MeshAsset>> meshes;
		for (const auto& [name, pair] : m_renderTextMesh) {
			meshes[name] = pair.first;
		}
		return meshes;
	};

	std::map<std::string, std::shared_ptr<MeshAsset>> GetCastShadowMeshes();

private:
	std::map<std::string, std::shared_ptr<MeshAsset>> m_renderMesh;
	std::map<std::string, std::pair<std::shared_ptr<MeshAsset>, std::shared_ptr<UIText>>> m_renderTextMesh;

	RenderPassType m_currentPass{ RenderPassType::ShadowPass };
	GameEngineConfig* m_gameConfig{};
	std::shared_ptr<DeviceManager> m_deviceManager;
	std::shared_ptr<Shadows> m_shadows;
};