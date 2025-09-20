#pragma once

#include <chrono>
#include <string>
#include <iostream>

class ScopedTimer {
public:
    ScopedTimer(const std::string& label)
        : m_label(label), m_start(std::chrono::high_resolution_clock::now()) {
    }

    ~ScopedTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        auto durationMs = std::chrono::duration_cast<std::chrono::microseconds>(end - m_start).count() / 1000.0;
        std::cout << "[Timer] " << m_label << ": " << durationMs << " ms\n";
    }

private:
    std::string m_label;
    std::chrono::high_resolution_clock::time_point m_start;
};
