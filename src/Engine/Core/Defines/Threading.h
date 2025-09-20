#pragma once

#include <atomic>
#include <chrono>
#include <future>
#include <memory>
#include <string>

namespace Threading {
    //template <typename T>
    struct Job
    {
        std::string name;
        bool allowDuplicates = false;
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
        std::future<bool> future;
        bool isSuccessful = false;
        //T data;
        std::shared_ptr<std::atomic<bool>> cancel_token;
    };
}
