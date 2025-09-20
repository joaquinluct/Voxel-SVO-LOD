#pragma once

class IPipelineState
{
public:
    virtual ~IPipelineState() = default;
    virtual void Activate() = 0;
    virtual void Deactivate() = 0;
    virtual bool IsActive() const = 0;
};
