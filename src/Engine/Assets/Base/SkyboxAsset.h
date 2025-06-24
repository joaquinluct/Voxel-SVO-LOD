#pragma once

#include <Assets/IAssetSkybox.h>
#include <Assets/Base/MeshAsset.h>
#include <Assets/IAssetSkyboxConfigBase.h>
#include <Config/Asset/DefaultSkyboxConfig.h>

class SkyboxAsset : public IAssetSkybox
{
	public:
	SkyboxAsset();
	~SkyboxAsset() override;
	void Load() override {};
	void Unload() override {};
	HRESULT Init() override;
	void Render() override {};
	void Update(float deltaTime) override {};
	void Shutdown() override;
	const std::string& GetAssetName() override {
		static const std::string name = "SkyboxAsset";
		return name;
	}
	static const std::string& GetStaticAssetName()
	{
		static const std::string name = "SkyboxAsset";
		return name;
	}	
	void GetMesh() override;
	void SetType(const std::string& type);
	IAssetSkyboxConfig* GetConfig() {
		return dynamic_cast<IAssetSkyboxConfig*>(m_config);
	};

private:
	IAssetSkyboxConfigBase* m_config;
	std::shared_ptr<MeshAsset> m_mesh;
};