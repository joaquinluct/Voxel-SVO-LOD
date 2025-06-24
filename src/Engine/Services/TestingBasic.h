#pragma once
#include "IService.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <DeviceManager.h>

class TestingBasic : public IService {
public:
    TestingBasic();
    ~TestingBasic() override;

    HRESULT Init() override;
    void Render() override;
    void Shutdown() override;
    void Update(float) override {}

    const std::string& GetServiceName() const override {
        static const std::string name = "TestingBasic";
        return name;
    }

    static const std::string& GetStaticManagerName() {
        static const std::string name = "TestingBasic";
        return name;
    }

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

    std::shared_ptr<DeviceManager> m_deviceManager;
};