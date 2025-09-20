#pragma once

#include <atomic>
#include <chrono>
#include <string>

// Declaramos un contador atómico estático para asegurar
// que cada UID generado sea único en la sesión del programa.
static std::atomic<long long> uid_counter{ 0 };

inline static std::string generateUID() {
    // Obtiene el tiempo actual en nanosegundos para tener una base
    // de tiempo precisa y única.
    long long timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    // Incrementa el contador atómico. El prefijo ++ asegura que
    // el valor se incremente antes de ser usado.
    long long counter = ++uid_counter;

    // Combina el tiempo y el contador en una cadena.
    std::stringstream ss;
    ss << timestamp << "-" << counter;
    return ss.str();
}
