#pragma once

#include <Assets/IAsset.h>
#include <Assets/Base/MeshAsset.h>

class ShaderAsset : public IAsset
{
public:
	ShaderAsset();
	~ShaderAsset() override;
	void Load() override {};
	void Unload() override {};
	HRESULT Init() override;
	void Render() override {};
	void Update(float deltaTime) override {};
	void Shutdown() override;
	const std::string& GetAssetName() override {
		static const std::string name = "ShaderAsset";
		return name;
	}
	static const std::string& GetStaticAssetName()
	{
		static const std::string name = "ShaderAsset";
		return name;
	}

private:
	std::string m_name = "ShaderAsset";
	//std::shared_ptr<MeshAsset> m_mesh;
};
