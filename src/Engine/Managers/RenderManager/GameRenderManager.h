#pragma once

#include <string>
#include <vector>
#include <memory>
#include <IManager.h>
#include <ILifeCycle.h>
#include <Game/Systems/Shadows.h>
#include <Config/Game/GameEngineConfig.h>
#include <Defines/Pipeline.h>

class MeshAsset;

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
	std::shared_ptr<MeshAsset> RegisterMesh(const std::string& meshName);

	void UnregisterMesh(const std::string& meshName) {
		m_renderMesh.erase(meshName);
	}

	std::map<std::string, std::shared_ptr<MeshAsset>> GetMeshes() {
		return m_renderMesh;
	};
private:
	std::map<std::string, std::shared_ptr<MeshAsset>> m_renderMesh;

	RenderPassType m_currentPass{ RenderPassType::ShadowPass };
	GameEngineConfig* m_gameConfig{};
	std::shared_ptr<DeviceManager> m_deviceManager;
	std::shared_ptr<Shadows> m_shadows;
};
