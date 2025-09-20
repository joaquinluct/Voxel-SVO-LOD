// IService.h
#pragma once
#include "ILifeCycle.h"
#include <string>

class IService : public ILifeCycle {
private:
    bool m_isRunning = false; // Default to not running
    bool m_shadowPass = false; // Default to not using shadow pass
public:
    virtual ~IService() = default;
    virtual const std::string& GetServiceName() const = 0;
    void Start() {
        m_isRunning = true;
    };
    void Stop() {
        m_isRunning = false;
    };
    bool IsRunning() const {
        return m_isRunning;
    };
    bool NeedsShadowPass() const {
        return m_shadowPass;
    };
    void SetNeedsShadowPass(bool shadowPass) {
        m_shadowPass = shadowPass;
    };
    //virtual void SetConfig(std::shared_ptr<ConfigService> config) = 0;
};
