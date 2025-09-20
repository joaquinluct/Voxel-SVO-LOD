#pragma once

#include <chrono>

// Función que convierte un time_point a un float que representa los segundos
float ConvertTimeToFloat(const std::chrono::system_clock::time_point& timePoint) {
    // 1. Obtiene la duración desde el epoch (el punto de referencia de tiempo).
    auto duration_since_epoch = timePoint.time_since_epoch();

    // 2. Convierte esa duración a segundos usando un float como tipo subyacente.
    // Esto es lo que permite que el resultado tenga decimales.
    auto float_seconds = std::chrono::duration_cast<std::chrono::duration<float>>(duration_since_epoch);

    // 3. Retorna el valor numérico de la duración.
    return float_seconds.count();
}

