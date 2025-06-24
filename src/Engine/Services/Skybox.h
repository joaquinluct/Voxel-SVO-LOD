#pragma once
#define NOMINMAX 
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <string>
#include "IService.h"
#include <Config/Game/SkyboxConfig.h>
#include <Assets/IAsset.h>
#include <Assets/Base/SkyboxAsset.h>
#include <AssetLocator/AssetLocator.h>
#include <ManagerLocator/ManagerLocator.h>
#include <DeviceManager.h>

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
		static const std::string name = "Skybox"; // Esta cadena se crea una sola vez y vive durante toda la ejecución del programa.
		return name;
	}
	static const std::string& GetStaticManagerName()
	{
		static const std::string name = "Skybox"; // Esta cadena se crea una sola vez y vive durante toda la ejecución del programa.
		return name;
	}

private:
	std::shared_ptr<MeshAsset> m_mesh;
	std::shared_ptr<DeviceManager> m_deviceManager;
	SkyboxConfig::Values* m_config{};
	IAsset* m_skyboxAsset{};
};
