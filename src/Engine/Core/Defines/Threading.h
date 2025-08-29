#pragma once

#include <string>
#include <chrono>
#include <future>

namespace Threading {
    template <typename T>
    struct Job
    {
        std::string name;
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
        std::future<bool> future;
        T data;
        std::shared_ptr<std::atomic<bool>> cancel_token;
    };
}