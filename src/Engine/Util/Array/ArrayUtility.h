#pragma once

#include <string>
#include <vector>
#include <optional>

namespace ArrayUtility
{
    static inline std::string find(const std::vector<std::string>& vec, const std::string& value) {
        auto it = std::find(vec.begin(), vec.end(), value);
        if (it != vec.end()) {
            return *it; // Devuelve una copia de la cadena encontrada
        }
        return "";
    }
}

