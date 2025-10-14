#pragma once

#include <functional>
#include <map>
#include <mutex>
#include <vector>
#include <cstddef>

class CommandBuffer;

// RenderCommandSystem centraliza la recolección de comandos desde
// distintos subsistemas (UI, Scene, Particles, etc.) y los agrega
// a un `CommandBuffer` por frame.
class RenderCommandSystem {
public:
    using ContributorId = std::size_t;
    using Contributor = std::function<void(CommandBuffer&)>;

    RenderCommandSystem();
    ~RenderCommandSystem();

    // Acceso al singleton de sistema (fácil de usar desde Engine)
    static RenderCommandSystem& Get();

    // Registrar/Desregistrar contribuidores de comandos. El contribuidor
    // es una función que recibe un CommandBuffer& y escribe comandos.
    ContributorId RegisterContributor(Contributor c);
    void UnregisterContributor(ContributorId id);

    // Llenar el buffer invocando a todos los contribuidores registrados.
    void Aggregate(CommandBuffer& buffer);

private:
    std::mutex m_mutex;
    std::map<ContributorId, Contributor> m_contributors;
    ContributorId m_nextId{1};
};
