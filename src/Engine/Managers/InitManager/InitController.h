#pragma once

#include <string>
#include <vector>
#include <utility>
#include <map>
#include <set>
#include <stdexcept> // Para std::runtime_error
#include <iostream>  // Para std::cerr y std::cout

class InitController {
public:
    // Constructor por defecto
    InitController() = default;
    // Destructor por defecto
    ~InitController() = default;

    // Método para registrar un componente junto con sus dependencias de inicialización.
    // El 'componentName' es un identificador único (ej. "KeyboardManager").
    // 'dependencies' es una lista de nombres de otros componentes de los que este depende.
    void RegisterComponent(const std::string& componentTypeName, const std::string& componentName, const std::vector<std::string>& dependencies);

    // Método público que calcula y devuelve el orden de inicialización de todos los componentes registrados.
    // Lanza un 'std::runtime_error' si se detecta un ciclo de dependencia, indicando un error fatal.
    // El orden retornado garantiza que todas las dependencias de un componente se inicialicen antes que él.
    std::vector<std::pair<std::string, std::string>>  GetInitializationOrder();

private:
    // Mapa que almacena las dependencias directas de cada componente.
    // La clave es el nombre del componente, el valor es un vector de nombres de los componentes de los que depende.
    std::map<std::string, std::pair<std::vector<std::string>, std::string>> m_componentDependencies;

    // Vector que almacenará el orden de inicialización calculado (resultado del ordenamiento topológico).
    std::vector<std::pair<std::string, std::string>> m_calculatedOrder;

    // Función auxiliar recursiva que implementa el algoritmo de búsqueda en profundidad (DFS)
    // para realizar el ordenamiento topológico y detectar ciclos.
    // 'componentName': El nombre del componente actual que se está visitando.
    // 'visited': Un conjunto de componentes que ya han sido completamente procesados y añadidos a 'm_calculatedOrder'.
    // 'visiting': Un conjunto de componentes que están actualmente en la pila de recursión del DFS (para detección de ciclos).
    // Retorna 'true' si el subgrafo desde 'componentName' se procesó sin ciclos, 'false' si se detectó un ciclo.
    bool DFS(const std::string& componentTypeName,
        const std::string& componentName,
        std::set<std::string>& visited,
        std::set<std::string>& visiting);
};