#pragma once
#include <windows.h>
#include <string>
#include <IService.h>
#include <Assets/Base/MeshAsset.h>
#include <ManagerLocator/ManagerLocator.h>
#include <AssetLocator/AssetLocator.h>

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
	static const std::string& GetStaticManagerName()
	{
		static const std::string name = "Skybox";
		return name;
	}

private:
	MeshAsset mesh;
};
