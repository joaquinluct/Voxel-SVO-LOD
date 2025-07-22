#pragma once

#include "ConfigLocator/ConfigLocator.h"
#include "IConfig.h"

#define REGISTER_CONFIG_TYPE(ConfigClassName, ConfigNameString) \
    namespace { \
        class Config ## ConfigClassName ## Registrar { \
        public: \
            Config ## ConfigClassName ## Registrar() { \
                ConfigLocator::RegisterConfigCreator( \
                    ConfigNameString, \
                    /* Lambda de creación: Devuelve std::shared_ptr<ConfigClassName> que se convierte implícitamente a std::shared_ptr<IConfig> */ \
                    []() -> std::shared_ptr<IConfig> {  \
                        return std::make_shared<ConfigClassName>(); \
                    } \
                ); \
            } \
        }; \
        static Config ## ConfigClassName ## Registrar s_register_ ## ConfigClassName; \
    }