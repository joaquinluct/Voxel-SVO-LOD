#pragma once

class IUpdateSystem {
public:
    virtual ~IUpdateSystem() = default;
    virtual void Update(float deltaTime) = 0;
};
