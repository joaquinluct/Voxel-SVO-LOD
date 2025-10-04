// IManager.h
#pragma once
#include <string>

class IManager {
public:
    virtual ~IManager() = default;
    virtual const std::string& GetManagerName() const = 0;
};
