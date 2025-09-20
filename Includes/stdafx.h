#pragma once

// Este archivo incluye los archivos de cabecera del sistema y
// los archivos de cabecera de la biblioteca estándar para el proyecto.
// Acelera el proceso de compilación.

// Definiciones de la arquitectura (necesarias para evitar tu error)
#if defined(_WIN64)
#define TARGET_ARCH_X64
#elif defined(_M_IX86)
#define TARGET_ARCH_X86
#endif

// Incluye bibliotecas comunes de Windows y C++
//#include <iostream>
//#include <map>
//#include <memory>
//#include <mutex>
//#include <string>
//#include <vector>

// Añade cualquier otro archivo de cabecera que necesites aquí
// #include <d3d11.h>
// #include <dxgi.h>

