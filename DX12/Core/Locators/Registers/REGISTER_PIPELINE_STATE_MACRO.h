#pragma once

#include "Pipeline/PipelineStateLocator.h"
#include "IPipelineState.h"

#define REGISTER_PIPELINE_STATE_TYPE(PipelineStateClassName, PipelineStateNameString) \
    namespace { \
        class PipelineState ## PipelineStateClassName ## Registrar { \
        public: \
            PipelineState ## PipelineStateClassName ## Registrar() { \
                PipelineStateLocator::RegisterPipelineStateCreator( \
                    PipelineStateNameString, \
                    /* Lambda de creación: Devuelve std::shared_ptr<PipelineStateClassName> que se convierte implícitamente a std::shared_ptr<IPipelineState> */ \
                    []() -> std::shared_ptr<IPipelineState> {  \
                        return std::make_shared<PipelineStateClassName>(); \
                    } \
                ); \
            } \
        }; \
        static PipelineState ## PipelineStateClassName ## Registrar s_register_ ## PipelineStateClassName; \
    }
#pragma once
