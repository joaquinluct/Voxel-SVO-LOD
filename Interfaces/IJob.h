#pragma once
#include <chrono>
#include <thread>

struct JobContext;

class IJob
{
public:
    virtual ~IJob() = default;
    virtual bool Execute(JobContext* context) {
        // Pequeña pausa para evitar el uso excesivo de la CPU y
        // para dar tiempo a qeu la tarea se encole antes de que termine
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        return true;
    };
};
