#pragma once

#include <string>
#include <vector>
#include <IService.h>

class UIBase : public IService {
public:
	UIBase();
	~UIBase();
	HRESULT Init() override;
	void Render() override;
	void Update(float deltaTime) override;
	void Shutdown() override;
    const std::string& GetServiceName() const override {
        static const std::string name = "UIBase";
        return name;
    }
    static const std::string& GetStaticServiceName()
    {
        static const std::string name = "UIBase";
        return name;
    }
private:

};
