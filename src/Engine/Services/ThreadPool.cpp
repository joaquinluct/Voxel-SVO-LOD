#include "ThreadPool.h"
#include <REGISTER_SERVICE_MACRO.h>

REGISTER_SERVICE_TYPE(ThreadPool, "ThreadPool")

ThreadPool::ThreadPool(): 
    m_stop(false) 
{}

HRESULT ThreadPool::Init(size_t numThreads) {
    for (size_t i = 0; i < numThreads; ++i) {
        m_workers.emplace_back([this] {
            // Cada hilo ejecuta este bucle de trabajo.
            while (true) {
                std::function<void()> task;
                {
                    // Bloqueo del mutex para acceder a la cola de tareas.
                    std::unique_lock<std::mutex> lock(m_queueMutex);

                    // Espera hasta que haya una tarea o se indique la parada.
                    m_condition.wait(lock, [this] {
                        return m_stop || !m_tasks.empty();
                        });

                    // Si se debe detener y no hay más tareas, el hilo sale del bucle.
                    if (m_stop && m_tasks.empty()) {
                        return;
                    }

                    // Toma la tarea de la parte frontal de la cola.
                    task = std::move(m_tasks.front());
                    m_tasks.pop();
                }
                // Desbloqueo del mutex y ejecución de la tarea.
                // Esto permite que otros hilos tomen tareas mientras este está trabajando.
                task();
            }
        });
    }
	return S_OK;
}

// Destructor que se asegura de que todos los hilos se detengan y se unan (join).
ThreadPool::~ThreadPool() {
    {
        // Bloquea el mutex para establecer la bandera de parada.
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_stop = true;
    }
    // Notifica a todos los hilos de la condición de parada.
    m_condition.notify_all();

    // Espera a que cada hilo termine su ejecución.
    for (std::thread& worker : m_workers) {
        worker.join();
    }
}