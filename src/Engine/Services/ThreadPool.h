#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <IService.h>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <vector>

// La clase ThreadPool gestiona un pool de hilos para ejecutar tareas de forma asíncrona.
class ThreadPool : public IService {
public:
    ThreadPool();
    ~ThreadPool() override;

    HRESULT Init() override { return S_OK; };
    HRESULT Init(size_t numThreads);
    void Shutdown() override {};
    void Render() override {};
    void Update(float deltaTime) override {};

    const std::string& GetServiceName() const override {
        static const std::string name = "ThreadPool"; return name;
    }
    static const std::string& GetStaticServiceName() { static const std::string name = "ThreadPool"; return name; }

    // Encola una tarea con argumentos y devuelve un std::future para obtener el resultado.
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>>
    {
        using return_type = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;

        auto boundTask = std::make_shared<std::packaged_task<return_type()>>(
            [func = std::forward<F>(f), argsTuple = std::make_tuple(std::forward<Args>(args)...)]() mutable {
                return std::apply(func, std::move(argsTuple));
            }
        );

        std::future<return_type> res = boundTask->get_future();
        {
            std::unique_lock<std::mutex> lock(m_queueMutex);
            if (m_stop)
                throw std::runtime_error("enqueue on stopped ThreadPool");

            m_tasks.emplace([boundTask]() { (*boundTask)(); });
        }
        m_condition.notify_one();
        return res;
    }

private:
    // Vector para almacenar los hilos de trabajo.
    std::vector<std::thread> m_workers;

    // Cola de tareas a ejecutar.
    std::queue<std::function<void()>> m_tasks;

    // Mutex para la sincronización del acceso a la cola.
    std::mutex m_queueMutex;

    // Variable de condición para la notificación de tareas.
    std::condition_variable m_condition;

    // Bandera de parada para el cierre del pool.
    bool m_stop;
};

// Ejemplo de uso
//int main() {
//    // Crea un pool de hilos con 4 hilos de trabajo.
//    ThreadPool pool(4);
//
//    // Encola 8 tareas en el pool.
//    std::vector<std::future<int>> results;
//    for (int i = 0; i < 8; ++i) {
//        results.emplace_back(pool.enqueue([i] {
//            std::cout << "Tarea " << i << " ejecutada en el hilo " << std::this_thread::get_id() << std::endl;
//            std::this_thread::sleep_for(std::chrono::milliseconds(200));
//            return i * i;
//            }));
//    }
//
//    // Obtiene los resultados de las tareas.
//    std::cout << "Esperando resultados..." << std::endl;
//    for (auto& res : results) {
//        std::cout << "Resultado: " << res.get() << std::endl;
//    }
//
//    return 0;
//}
