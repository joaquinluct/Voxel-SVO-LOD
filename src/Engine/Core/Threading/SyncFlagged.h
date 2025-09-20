#pragma once

#include <unordered_map>
#include <string_view>
#include <mutex>
#include <condition_variable>
#include <chrono>

class SyncFlagged {
/*
    ┌────────────────────────────────────────────────────────────────────────┐
    │  SyncFlagged.h                                                         │
    │  Parte del módulo Core::Threading del framework                        │
    └────────────────────────────────────────────────────────────────────────┘

    Esta clase base proporciona un sistema de sincronización encapsulado
    para componentes que requieren gestión de estados booleanos (flags)
    compartidos entre hilos.

    Características:
    - Acceso seguro a flags mediante mutex.
    - Espera condicional con timeout usando condition_variable.
    - Interfaz sencilla para marcar, consultar y esperar estados.

    Uso típico:
    - Managers y servicios que deben señalizar su estado de inicialización,
      disponibilidad, apagado, etc.
    - Componentes que se comunican entre hilos sin necesidad de lógica
      de sincronización personalizada.

    Ejemplo de integración:
    class SceneManager : public SyncFlagged {
    public:
        void Init() {
            // Inicialización del sistema
            SetFlag("initialized", true);
        }

        bool IsInitialized() const {
            return GetFlag("initialized");
        }
    };

    En otro hilo:
    if (!sceneManager->WaitForFlag("initialized", true, std::chrono::seconds(5))) {
        std::cerr << "SceneManager no está listo. Continuando..." << std::endl;
    }

    Este sistema permite desacoplar la lógica de sincronización del flujo
    principal de la aplicación, manteniendo el código limpio, seguro y escalable.
*/

protected:
    mutable std::mutex m_flagMutex;
    std::condition_variable m_flagCondition;

public:
    std::unordered_map<std::string_view, bool> m_flags;
    void SetFlag(const std::string_view& name, bool value) {
        {
            std::lock_guard<std::mutex> lock(m_flagMutex);
            m_flags[name] = value;
        }
        m_flagCondition.notify_all();
    }

    bool GetFlag(const std::string_view& name) const {
        std::lock_guard<std::mutex> lock(m_flagMutex);
        auto it = m_flags.find(name);
        return it != m_flags.end() ? it->second : false;
    }

    bool WaitForFlag(const std::string_view& name, bool expected, std::chrono::milliseconds timeout) {
        std::unique_lock<std::mutex> lock(m_flagMutex);
        return m_flagCondition.wait_for(lock, timeout, [&] {
            auto it = m_flags.find(name);
            return it != m_flags.end() && it->second == expected;
            });
    }
};