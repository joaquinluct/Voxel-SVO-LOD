#include "UpdateSystem.h"
#include <Managers/UpdateManager.h>

UpdateSystem::UpdateSystem(UpdateManager* legacyManager) : m_legacy(legacyManager) {}

void UpdateSystem::Update(float deltaTime) {
    if (m_legacy) m_legacy->Update(deltaTime);
}
