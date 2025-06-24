#include "DeviceManager.h" // Asegúrate de que la ruta sea correcta
#include <iostream>        // Para imprimir en consola

int main1() {
    // Imprime el nombre directamente
    std::cout << "Nombre estático del DeviceManager: " << DeviceManager::GetStaticManagerName() << std::endl;
    return 0;
}