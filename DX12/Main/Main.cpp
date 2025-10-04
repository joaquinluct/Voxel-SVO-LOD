#include "Main/MainWindow.h"
#include <windows.h>
#include <iostream>

// Versión Console compatible (temporal)
int main() {
    // Obtener instancia de la aplicación
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    
    // Crear instancia de MainWindow DX12
    Engine::MainWindow mainWindow(hInstance, nullptr, GetCommandLineW(), SW_SHOW);
    
    // Ejecutar aplicación
    auto result = mainWindow.Create();
    
    if (result.has_value()) {
        std::wcout << L"Aplicación terminada con código: " << result.value() << std::endl;
        return result.value();
    } else {
        std::wcerr << L"Error al ejecutar aplicación" << std::endl;
        return -1;
    }
}

// Mantener wWinMain para compatibilidad futura
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR lpCmdLine,
                      _In_ int nCmdShow) {
    
    Engine::MainWindow mainWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    auto result = mainWindow.Create();
    
    return result.has_value() ? result.value() : -1;
}
