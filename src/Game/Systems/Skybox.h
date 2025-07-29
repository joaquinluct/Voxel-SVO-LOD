#pragma once
#include <windows.h>
#include <string>
#include <memory>
#include <IService.h>
#include <RenderManager/RenderManager.h>
#include <Game/Systems/Lighting.h>
#include <Assets/Base/MeshAsset.h>

class Skybox : public IService
{
public:
	Skybox();
	~Skybox() override;

	HRESULT Init() override;
	void Render() override;
	void Update(float deltaTime) override;
	void Shutdown() override;
	const std::string& GetServiceName() const override {
		static const std::string name = "Skybox";
		return name;
	}
	static const std::string& GetStaticServiceName()
	{
		static const std::string name = "Skybox";
		return name;
	}

private:
	std::shared_ptr<RenderManager> m_renderManager;
	std::shared_ptr<Lighting> lighting;

	std::shared_ptr<MeshAsset> mesh;

	float m_eventTimer;
	float m_eventInterval;
	bool sign;
	float m_rotationSpeed;
};
