#pragma once

#include "ServiceLocator/ServiceLocator.h"
#include "IWindowDependentInitializable.h"
#include "IInitializable.h"
#include "IRenderable.h"
#include "IManager.h"
#include "IService.h"


#define DEFINE_INIT_SERVICE_LAMBDA(ServiceClassName) \
    [](std::shared_ptr<IService> servicePtr) -> HRESULT { \
        if (auto simpleInitializable = std::dynamic_pointer_cast<IInitializable>(servicePtr)) { \
            return simpleInitializable->Init(); \
        } \
        OutputDebugStringA(("ERROR: " ServiceClassName " does not implement a valid Init interface.\n")); \
        return E_FAIL; \
    }

#define DEFINE_RENDER_SERVICE_LAMBDA(ServiceClassName) \
    [](std::shared_ptr<IService> servicePtr) -> HRESULT { \
        if (auto renderable = std::dynamic_pointer_cast<IRenderable>(servicePtr)) { \
            renderable->Render(); \
            return S_OK; /* O el HRESULT que devuelva Render() */ \
        } \
    OutputDebugStringA(("ERROR: " ServiceClassName " does not implement IRenderable.\n")); \
    return E_FAIL; \
    }

#define DEFINE_UPDATE_SERVICE_LAMBDA(ServiceClassName) \
    [](std::shared_ptr<IService> servicePtr, float deltaTime) -> HRESULT { \
        if (auto renderable = std::dynamic_pointer_cast<IUpdatable>(servicePtr)) { \
            renderable->Update(deltaTime); \
            return S_OK; /* O el HRESULT que devuelva Render() */ \
        } \
    OutputDebugStringA(("ERROR: " ServiceClassName " does not implement IRenderable.\n")); \
    return E_FAIL; \
    }

#define REGISTER_SERVICE_TYPE(ServiceClassName, ServiceNameString) \
    namespace { \
        class Service ## ServiceClassName ## Registrar { \
        public: \
            Service ## ServiceClassName ## Registrar() { \
                ServiceLocator::RegisterServiceCreator( \
                    ServiceNameString, \
                    /* Lambda de creación: Devuelve std::shared_ptr<ServiceClassName> que se convierte implícitamente a std::shared_ptr<IService> */ \
                    []() -> std::shared_ptr<IService> {  \
                        return std::make_shared<ServiceClassName>(); \
                    }, \
                    /* Initializer Lambda */ \
                    DEFINE_INIT_SERVICE_LAMBDA(ServiceNameString), \
                    /* Render Lambda */ \
                    DEFINE_RENDER_SERVICE_LAMBDA(ServiceNameString), \
                    /* Update Lambda */ \
                    DEFINE_UPDATE_SERVICE_LAMBDA(ServiceNameString) \
                ); \
            } \
        }; \
        static Service ## ServiceClassName ## Registrar s_register_ ## ServiceClassName; \
    }