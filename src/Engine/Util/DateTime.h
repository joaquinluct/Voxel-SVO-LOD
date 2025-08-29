#pragma once

#include <string>
#include <ctime>
#include <chrono>
#include <sstream>
#include <iomanip> // <-- Nuevo include para std::get_time

namespace DateTime
{
    // --- Funciones para convertir de una cadena de texto ---

    inline std::time_t fromString(const std::string& datetime_str)
    {
        std::tm timeinfo = {};
        std::stringstream ss(datetime_str);

        // Define el formato esperado. %d, %m, %Y, %H, %M, %S son placeholders.
        // %H es la hora en formato 24h
        // %Y es el año completo
        ss >> std::get_time(&timeinfo, "%d/%m/%Y %H:%M:%S");

        if (ss.fail()) {
            // Intenta el formato con guiones si el primero falla
            ss.clear();
            ss.seekg(0);
            ss >> std::get_time(&timeinfo, "%d-%m-%Y %H:%M:%S");
        }

        if (ss.fail()) {
            return -1; // Fallo en el parseo con ambos formatos
        }

        // std::get_time ya ajusta los valores de tm_year y tm_mon
        // mktime normalizará los valores, como el horario de verano, etc.
        return std::mktime(&timeinfo);
    }

    inline std::chrono::system_clock::time_point fromStringTimePoint(const std::string& datetime_str)
    {
        std::time_t t = fromString(datetime_str);
        if (t == -1) {
            return std::chrono::system_clock::time_point();
        }
        return std::chrono::system_clock::from_time_t(t);
    }

    // --- Funciones para convertir de parámetros numéricos ---

    inline std::time_t fromNumbers(int day, int month, int year, int hour, int minute, int second)
    {
        std::tm timeinfo = {};
        timeinfo.tm_year = year - 1900;
        timeinfo.tm_mon = month - 1;
        timeinfo.tm_mday = day;
        timeinfo.tm_hour = hour;
        timeinfo.tm_min = minute;
        timeinfo.tm_sec = second;

        return std::mktime(&timeinfo);
    }

    inline std::chrono::system_clock::time_point fromNumbersTimePoint(int day, int month, int year, int hour, int minute, int second)
    {
        std::time_t t = fromNumbers(day, month, year, hour, minute, second);
        if (t == -1) {
            return std::chrono::system_clock::time_point();
        }
        return std::chrono::system_clock::from_time_t(t);
    }

    // --- Funciones para convertir a una cadena de texto ---

    inline std::string FormatTimePoint(const std::chrono::system_clock::time_point& tp) {
        // 1. Convierte time_point a time_t
        std::time_t t = std::chrono::system_clock::to_time_t(tp);

        // 2. Convierte time_t a std::tm de forma segura
        std::tm timeinfo;
        if (localtime_s(&timeinfo, &t) != 0) {
            return "Error al formatear la fecha.";
        }

        // 3. Usa stringstream y put_time para formatear
        std::stringstream ss;
        ss << std::put_time(&timeinfo, "%d/%m/%Y %H:%M:%S");

        return ss.str();
    }

    inline float GetTimeInSeconds() {
        // Almacena el tiempo de inicio la primera vez que se llama a la función
        static const auto start_time = std::chrono::high_resolution_clock::now();

        // Obtiene el tiempo actual
        auto current_time = std::chrono::high_resolution_clock::now();

        // Calcula la duración desde el inicio
        std::chrono::duration<float> duration = current_time - start_time;

        // Devuelve la duración en segundos
        return duration.count();
    }

} // namespace DateTime