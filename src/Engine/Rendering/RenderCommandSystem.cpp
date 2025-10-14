#include "RenderCommandSystem.h"
#include <Rendering/RenderCommand.h>
#include <Managers/SceneManager.h>

RenderCommandSystem::RenderCommandSystem() {}
RenderCommandSystem::~RenderCommandSystem() {}

RenderCommandSystem& RenderCommandSystem::Get() {
    static RenderCommandSystem s_instance;
    return s_instance;
}

RenderCommandSystem::ContributorId RenderCommandSystem::RegisterContributor(Contributor c) {
    std::lock_guard<std::mutex> lock(m_mutex);
    ContributorId id = m_nextId++;
    m_contributors.emplace(id, std::move(c));
    return id;
}

void RenderCommandSystem::UnregisterContributor(ContributorId id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_contributors.erase(id);
}

void RenderCommandSystem::Aggregate(CommandBuffer& buffer) {
    // Copy contributors under lock then invoke without holding lock
    std::vector<Contributor> copy;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        copy.reserve(m_contributors.size());
        for (auto &kv : m_contributors) copy.push_back(kv.second);
    }
    for (auto &c : copy) {
        if (c) c(buffer);
    }
}
