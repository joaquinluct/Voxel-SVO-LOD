#pragma once

#include <string>
#include <vector>
#include <IService.h>
#include <Assets/Base/MeshAsset.h>

class TestingFloor: public IService
{
public:    
    TestingFloor();
    ~TestingFloor() override;

    HRESULT Init() override;
    void Render() override;
    void Shutdown() override {};
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
    std::shared_ptr<MeshAsset> mesh;
};
