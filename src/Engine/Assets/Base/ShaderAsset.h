#pragma once
#include <Assets/Base/AssetBase.h>
#include <Assets/IAssetShaderConfig.h>
#include <ConfigBase.h>

class ShaderAsset : public AssetBase
{
public:
	ShaderAsset();
	std::unique_ptr<ShaderAsset> CloneShader() const {
		// Usa std::make_unique para crear una nueva instancia de VertexShader
		// y el constructor de copia para copiar el contenido.
		return std::make_unique<ShaderAsset>(*this);
	}
	virtual std::shared_ptr<AssetBase> Clone() const override {
		// Usa std::make_unique para crear una nueva instancia de VertexShader
		// y el constructor de copia para copiar el contenido.
		return std::make_unique<ShaderAsset>(*this);
	}
	virtual std::shared_ptr<MeshAsset> CloneAsMesh() const override {
		return nullptr;
	}
	virtual std::unique_ptr<AssetBase> CloneUnique() const override {
		// Usa std::make_unique para crear una nueva instancia de VertexShader
		// y el constructor de copia para copiar el contenido.
		return std::make_unique<ShaderAsset>(*this);
	}
	~ShaderAsset() override;
	void Load() override {};
	void Unload() override {};
	HRESULT Init() override;
	void Render() override {};
	void Update(float deltaTime) override {};
	void Shutdown() override;
	const std::string& GetAssetName() override {
		return m_name;
	}
	static const std::string& GetStaticAssetName()
	{
		static const std::string name = "ShaderAsset";
		return name;
	}

	void SetConfig(std::shared_ptr<ConfigBase> config) override;
	std::shared_ptr<IAssetShaderConfig> GetConfig() const;
	void SetName(const std::string& name);

private:
	std::shared_ptr<IAssetShaderConfig> m_shaderConfig = nullptr; // Configuración del shader
	std::string m_name = "ShaderAsset";
};
