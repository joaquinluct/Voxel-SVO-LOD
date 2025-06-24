#pragma once

#include <Assets/IAssetSkybox.h>
#include <Assets/Base/MeshAsset.h>

class DefaultSkybox : public IAssetSkybox
{
	public:
	DefaultSkybox();
	~DefaultSkybox() override;
	void Load() override {};
	void Unload() override {};
	HRESULT Init() override;
	void Render() override {};
	void Update(float deltaTime) override {};
	void Shutdown() override;
	const std::string& GetAssetName() override {
		static const std::string name = "DefaultSkyBox";
		return name;
	}
	static const std::string& GetStaticAssetName()
	{
		static const std::string name = "DefaultSkyBox";
		return name;
	}	
	void GetMesh() override;

private:
	std::string m_name = "DefaultSkybox";
	std::shared_ptr<MeshAsset> m_mesh;
};
