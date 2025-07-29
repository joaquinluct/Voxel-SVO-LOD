#pragma once

#include "Pipeline/RenderPassLocator.h"
#include "IRenderPass.h"

#define REGISTER_RENDER_PASS_TYPE(RenderPassClassName, RenderPassNameString) \
    namespace { \
        class RenderPass ## RenderPassClassName ## Registrar { \
        public: \
            RenderPass ## RenderPassClassName ## Registrar() { \
                RenderPassLocator::RegisterRenderPassCreator( \
                    RenderPassNameString, \
                    /* Lambda de creación: Devuelve std::shared_ptr<RenderPassClassName> que se convierte implícitamente a std::shared_ptr<IRenderPass> */ \
                    []() -> std::shared_ptr<IRenderPass> {  \
                        return std::make_shared<RenderPassClassName>(); \
                    } \
                ); \
            } \
        }; \
        static RenderPass ## RenderPassClassName ## Registrar s_register_ ## RenderPassClassName; \
    }
