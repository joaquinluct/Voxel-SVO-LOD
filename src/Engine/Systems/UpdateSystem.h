#pragma once

#include <memory>
#include "IUpdateSystem.h"

class UpdateManager;

class UpdateSystem : public IUpdateSystem {
public:
    UpdateSystem(UpdateManager* legacyManager);
    void Update(float deltaTime) override;

private:
    UpdateManager* m_legacy = nullptr; // delegate to legacy manager during migration
};
