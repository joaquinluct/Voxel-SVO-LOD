#pragma once
#define NOMINMAX

#include <DirectXMath.h>
#include <windows.h>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <IService.h>
#include <Config/Game/System/WaterConfig.h>
#include <CameraManager.h>
#include <ICamera.h>

class WaterAsset;
class MeshAsset;

class Water : public IService
{
public:
	Water();
	~Water() override;

	HRESULT Init() override;
	HRESULT PostInit();
	HRESULT InitServices();
	HRESULT InitConfig();	
	void Render() override;
	void Update(float deltaTime) override;
	void Shutdown() override {};
	const std::string& GetServiceName() const override {
		static const std::string name = "Water";
		return name;
	}
	static const std::string& GetStaticServiceName()
	{
		static const std::string name = "Water";
		return name;
	}

	float GetWaterHeight() const;

	std::vector<DirectX::XMMATRIX>& GetWaterMatrices() { return m_matrices; }

	std::shared_ptr<MeshAsset> GetWaterMesh() const { return m_waterMesh; }
private:	
	float m_oceanHeight = 0.0f; // Altura del agua del oceano
	float m_tileSize = 0.0f; // Tamaño de cada tile del agua

	std::vector<DirectX::XMMATRIX> m_matrices; // Matrices de transformación para los tiles de agua

	std::shared_ptr<CameraManager> m_cameraManager = nullptr;
	std::shared_ptr<WaterConfig> m_config = nullptr;

	std::shared_ptr<MeshAsset> m_waterMesh = nullptr;
};
