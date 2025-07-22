#include "InitController.h"
#include <windows.h>

// Implementación del método para registrar un componente.
// Almacena el nombre del componente y su lista de dependencias directas.
void InitController::RegisterComponent(const std::string& componentTypeName, const std::string& componentName, const std::vector<std::string>& dependencies) {
    if (m_componentDependencies.count(componentName)) {
        // En un entorno de producción, esto podría ser un error fatal o un registro de advertencia detallado.
        // Aquí se imprime una advertencia simple.
		OutputDebugStringA("Advertencia: Componente ya registrado. Sus dependencias serán sobrescritas.\n");
    }	
    m_componentDependencies[componentName] = { dependencies, componentTypeName };
    OutputDebugStringA(("Componente registrado " + componentName + "\n").c_str());
}

// Implementación del método principal para obtener el orden de inicialización.
// Inicia el proceso de ordenamiento topológico.
std::vector<std::pair<std::string, std::string>> InitController::GetInitializationOrder() {
    // Limpia el orden calculado y los conjuntos de seguimiento antes de cada cálculo.
    m_calculatedOrder.clear();
    std::set<std::string> visited;   // Nodos que ya se han añadido al orden final.
    std::set<std::string> visiting;  // Nodos actualmente en la pila de recursión (para detección de ciclos).

    // Itera sobre todos los componentes registrados para asegurar que cada uno sea visitado.
    // Esto es crucial para grafos disconexos (varios subgrafos de dependencias).
    for (const auto& pair : m_componentDependencies) {
        // Si el componente no ha sido visitado, inicia un DFS desde él.
        if (visited.find(pair.first) == visited.end()) {
            // Si el DFS detecta un ciclo, lanza una excepción fatal.
			const std::string& componentTypeName = pair.second.second;
            if (!DFS(componentTypeName, pair.first, visited, visiting)) {
                throw std::runtime_error("ERROR FATAL: Se detectó un ciclo de dependencia durante el cálculo del orden de inicialización.");
            }
        }
    }

    // El vector 'm_calculatedOrder' ahora contiene los componentes en orden topológico.
    // Este orden es un post-orden DFS, lo que significa que un nodo se añade después de que todas sus dependencias.
    // Si quisieras un orden inverso (pre-orden), tendrías que aplicar std::reverse aquí.
    return m_calculatedOrder;
}

// Implementación de la función DFS para el ordenamiento topológico.
bool InitController::DFS(const std::string& componentTypeName, const std::string& componentName,
    std::set<std::string>& visited,
    std::set<std::string>& visiting) {
    // Marca el componente actual como 'en visita' (parte de la pila de recursión).
    visiting.insert(componentName);

    // Busca las dependencias directas del componente actual.
    auto it = m_componentDependencies.find(componentName);
    if (it != m_componentDependencies.end()) {
        for (const std::string& dep : it->second.first) {
            std::string type = it->second.second;
            // Manejo de la palabra clave "None": Si la dependencia es "None", la ignoramos.
            // Esto es útil si tu generador de código incluye explícitamente "None" en el vector.
            if (dep == "None") {
                continue; // Ignora "None" y continúa con la siguiente dependencia.
            }

            // Manejo de la palabra clave "All": Si la dependencia es "All", también la ignoramos aquí.
            // "All" implica que el componente depende de todos los demás; el orden topológico natural
            // ya colocará componentes con muchas dependencias (como "All" implicaría) al final.
            if (dep == "All") {
                continue;
            }

            // --- Detección y manejo de dependencias no registradas ---
            // Si una dependencia no se encuentra entre los componentes registrados, es una advertencia.
            // Puedes cambiar esto a un error fatal lanzando una excepción si lo prefieres.
            if (m_componentDependencies.find(dep) == m_componentDependencies.end()) {
                std::cerr << "Advertencia: El componente '" << componentName << "' depende de '" << dep << "', pero '" << dep << "' no se ha registrado como un componente. Se ignorará esta dependencia." << std::endl;
                continue;
            }

            // --- Detección de ciclos de dependencia ---
            // Si una dependencia ya está en el conjunto 'visiting', significa que hemos encontrado un ciclo.
            if (visiting.find(dep) != visiting.end()) {
                std::cerr << "ERROR: Ciclo de dependencia detectado: '" << componentName << "' -> ... -> '" << dep << "'." << std::endl;
                return false; // Se detectó un ciclo.
            }

            // Si la dependencia no ha sido visitada (ni completamente procesada ni en la pila actual),
            // llama recursivamente a DFS para procesarla primero.
            auto itCurrent = m_componentDependencies.find(dep);
            if (itCurrent != m_componentDependencies.end()) {
				type = itCurrent->second.second; // Actualiza el tipo de componente para la dependencia.
			}

            if (visited.find(dep) == visited.end()) {
                if (!DFS(type, dep, visited, visiting)) {
                    return false; // Propaga el error si una dependencia encontró un ciclo.
                }
            }
        }
    }
    else {
        // Este caso ocurre si se intenta iniciar un DFS para un componente que no fue registrado.
        // Indica un problema en cómo se llama a DFS o en la inicialización de 'm_componentDependencies'.
        std::cerr << "ERROR Interno: Intento de procesar componente no registrado en DFS: '" << componentName << "'." << std::endl;
        return false;
    }

    // Una vez que todas las dependencias del componente actual han sido visitadas,
    // se marca el componente como completamente visitado y se añade justo después de la última dependencia procesada.
    visiting.erase(componentName); // Quita el componente de 'en visita'.
    visited.insert(componentName);   // Marca el componente como 'visitado' (completado).

    // --- Inserción justo después de la última dependencia directa ---
    const auto& dependencies = it->second.first;
    size_t insertPos = 0;
    bool found = false;
    // Busca la última posición de cualquier dependencia directa en m_calculatedOrder
    for (const std::string& dep : dependencies) {
        if (dep == "None" || dep == "All") continue;
        auto rit = std::find_if(m_calculatedOrder.rbegin(), m_calculatedOrder.rend(), [&](const std::pair<std::string, std::string>& p) {
            return p.first == dep;
        });
        if (rit != m_calculatedOrder.rend()) {
            size_t pos = std::distance(rit, m_calculatedOrder.rend()) - 1;
            if (!found || pos > insertPos) {
                insertPos = pos + 1; // Insertar después de la última dependencia
                found = true;
            }
        }
    }
    if (found) {
        m_calculatedOrder.insert(m_calculatedOrder.begin() + insertPos, { componentName, componentTypeName });
    } else {
        m_calculatedOrder.insert(m_calculatedOrder.begin(), { componentName, componentTypeName });
    }

    return true; // No se detectaron ciclos en esta rama.
}