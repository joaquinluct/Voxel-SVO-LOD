#pragma once

#include <Assets/Base/MeshAsset.h>
#include <CameraManager.h>
#include <Config/Game/System/SkyboxConfig.h>
#include <DirectXMath.h>
#include <Game/Systems/Lighting.h>
#include <IService.h>
#include <memory>
#include <RenderManager/RenderManager.h>
#include <string>
#include <windows.h>

class Skybox : public IService
{
public:
    Skybox();
    ~Skybox() override;

    HRESULT Init() override;
    HRESULT InitConfig();
    HRESULT InitManagers();
    HRESULT InitMesh();
    void Render() override;
    void Update(float deltaTime) override;
    void Shutdown() override;
    void UpdateSunPosition(float deltaTime);
    const std::string& GetServiceName() const override {
        static const std::string name = "Skybox";
        return name;
    }
    static const std::string& GetStaticServiceName()
    {
        static const std::string name = "Skybox";
        return name;
    }

    DirectX::XMFLOAT4 GetSunColor() const;
    DirectX::XMFLOAT4 GetSkyColor() const;

    MeshAssetBase* GetMesh() const
    {
        return mesh.get();
    }

private:
    SkyboxConfig m_config = {};

    DirectX::XMFLOAT4 m_sunColor;
    DirectX::XMFLOAT4 m_skyColor;

    std::shared_ptr<RenderManager> m_renderManager;
    std::shared_ptr<CameraManager> m_cameraManager;
    std::shared_ptr<Lighting> lighting;

    std::shared_ptr<MeshAsset> mesh;

    float m_eventTimer;
    float m_eventInterval;
    bool sign;
    float m_rotationSpeed;

    DirectX::XMVECTOR skyboxSunDirection = DirectX::XMVectorSet(0.5f, -1.0f, 1.27f, 0.0f);

    DirectX::XMMATRIX GetRotationMatrix() const;
};
