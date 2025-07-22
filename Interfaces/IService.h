// IService.h
#pragma once
#include <string>
#include "ILifeCycle.h"
#include <Config/ConfigService.h>

class IService : public ILifeCycle {
private:
	bool m_isRunning = false; // Default to not running
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
	//virtual void SetConfig(std::shared_ptr<ConfigService> config) = 0;
};