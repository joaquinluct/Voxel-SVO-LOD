// ServiceCreator.h
#pragma once
#include <memory>
#include <functional>
#include <string>

// Define un alias para el tipo de función que creará un shared_ptr a un servicio.
// Por ahora, asumimos que todos los servicios pueden crearse sin argumentos,
// o que sus argumentos se manejarán por separado (ej. pasándolos a un método Init() después).
using ServiceCreator = std::function<std::shared_ptr<void>()>;
// Usamos shared_ptr<void> para la genericidad, y luego haremos static_pointer_cast.