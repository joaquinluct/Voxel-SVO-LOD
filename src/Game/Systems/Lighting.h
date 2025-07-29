#pragma once
#include <windows.h>
#include <string>
#include <memory>
#include <IService.h>
#include <Defines/MatrixDefinitionBase.h>
#include <Config/Game/System/LightingConfig.h>

class Lighting : public IService
{
public:
	Lighting();
	~Lighting() override;

	HRESULT Init() override;
	void Render() override;
	void Update(float deltaTime) override;
	void Shutdown() override;
	const std::string& GetServiceName() const override {
		static const std::string name = "Lighting";
		return name;
	}
	static const std::string& GetStaticServiceName()
	{
		static const std::string name = "Lighting";
		return name;
	}

	void SetLightDirection(const DirectX::XMFLOAT3& direction);	
	void SetLightColor(const DirectX::XMFLOAT4& color);
	DirectX::XMFLOAT3 GetLightDirection() const;
	DirectX::XMFLOAT4 GetLightColor() const;

private:
	std::shared_ptr<MatrixDefinitionBase::MatrixParams> matrixParams;
	std::shared_ptr<LightingConfig> config;	
};
