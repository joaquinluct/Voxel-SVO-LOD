// IManager.h
#pragma once
#include <Windows.h>
#include <string>

class IManager {
public:
    virtual ~IManager() = default;
    virtual const std::string& GetManagerName() const = 0;
};