#pragma once

#include <string>
#include <memory>

class IClonable {
public:
    virtual ~IClonable() = default;
    virtual std::shared_ptr<IClonable> Clone() const = 0;
};