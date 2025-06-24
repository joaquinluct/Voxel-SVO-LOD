// IService.h
#pragma once
#include <string>
#include "ILifeCycle.h"

class IService: public ILifeCycle {
public:
    virtual ~IService() = default;
    virtual const std::string& GetServiceName() const = 0;
};