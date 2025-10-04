#pragma once

#include <Defines/VertexDefinition.h>
#include <IDefine/IVertex.h>
#include "DefineLocator/DefineLocator.h"
#include <memory>

// DefineFullTypeName: El nombre completo del tipo, incluyendo su namespace (ej. VertexDefinition::VertexSimple)
// DefineSimpleName: Un nombre simple para usar en la concatenación de la clase registradora (ej. VertexSimple)
// DefineNameString: El string que se usa para registrar (ej. "VertexSimple" o "VertexDefinition::VertexSimple")

#define REGISTER_DEFINE_TYPE(DefineFullTypeName, DefineSimpleName, DefineNameString) \
    namespace { /* Usamos un namespace anónimo para evitar colisiones de nombres de registradores */ \
        class Define ## DefineSimpleName ## Registrar { \
        public: \
            Define ## DefineSimpleName ## Registrar() { \
                DefineLocator::RegisterDefineCreator( \
                    DefineNameString, \
                    /* Lambda de creación: Devuelve std::shared_ptr<DefineFullTypeName> que se convierte implícitamente a std::shared_ptr<IDefine> */ \
                    []() -> std::shared_ptr<IVertex> { \
                        return std::make_shared<DefineFullTypeName>(); \
                    } \
                ); \
            } \
        }; \
        /* Instancia estática para activar el registro en el inicio del programa */ \
        static Define ## DefineSimpleName ## Registrar s_register_ ## DefineSimpleName; \
    }