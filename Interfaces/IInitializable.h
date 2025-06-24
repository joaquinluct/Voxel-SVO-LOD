// IInitializable.h (para managers que solo necesitan config YAML)
#pragma once
#include "yaml-cpp/yaml.h"
#include <winerror.h>

class IInitializable {
public:
    virtual ~IInitializable() = default;
    virtual HRESULT Init() = 0;
};