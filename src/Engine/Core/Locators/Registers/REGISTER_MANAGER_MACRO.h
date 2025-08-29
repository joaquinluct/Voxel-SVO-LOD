#pragma once

#include <ManagerLocator/ManagerLocator.h>
#include "IWindowDependentInitializable.h"
#include "IInitializable.h"
#include "IRenderable.h"
#include "ManagerBase.h"
#include "IService.h"

//#define DEFINE_INIT_WITH_PARAMS_MANANGER_LAMBDA(ManagerClassName) \
//    [](std::shared_ptr<ManagerBase> servicePtr, HWND* hwnd, int width, int height) -> HRESULT { \
//        auto manager = std::dynamic_pointer_cast<ManagerBase>(servicePtr); \
//        if (manager->IsWindowDependent()) { \
//            return manager->Init(hwnd, width, height); \
//        } \
//        return manager->Init(); \
//    }

#define DEFINE_INIT_MANANGER_LAMBDA(ManagerClassName) \
    [](std::shared_ptr<ManagerBase> servicePtr) -> HRESULT { \
        return servicePtr->Init(); \
    }

#define DEFINE_INIT_WITH_PARAMS_MANANGER_LAMBDA(ManagerClassName) \
    [](std::shared_ptr<ManagerBase> servicePtr, HWND* hwnd, int width, int height) -> HRESULT { \
        return servicePtr->Init(hwnd, width, height); \
    }

#define DEFINE_RENDER_MANAGER_LAMBDA(ManagerClassName) \
    [](std::shared_ptr<ManagerBase> servicePtr) -> HRESULT { \
        if (auto renderable = std::dynamic_pointer_cast<IRenderable>(servicePtr)) { \
            renderable->Render(); \
            return S_OK; /* O el HRESULT que devuelva Render() */ \
        } \
    OutputDebugStringA(("ERROR: " ManagerClassName " does not implement IRenderable.\n")); \
    return E_FAIL; \
    }

#define DEFINE_UPDATE_MANAGER_LAMBDA(ManagerClassName) \
    [](std::shared_ptr<ManagerBase> servicePtr, float deltaTime) -> HRESULT { \
        if (auto updatable = std::dynamic_pointer_cast<IUpdatable>(servicePtr)) { \
            updatable->Update(deltaTime); \
            return S_OK; /* O el HRESULT que devuelva Render() */ \
        } \
    OutputDebugStringA(("ERROR: " ManagerClassName " does not implement IRenderable.\n")); \
    return E_FAIL; \
    }

#define REGISTER_MANAGER_TYPE(ManagerClassName, ManagerNameString) \
    namespace { \
        class Manager ## ManagerClassName ## Registrar { \
        public: \
            Manager ## ManagerClassName ## Registrar() { \
                ManagerLocator::RegisterManagerCreator( \
                    ManagerNameString, \
                    /* Lambda de creación: Devuelve std::shared_ptr<ManagerClassName> que se convierte implícitamente a std::shared_ptr<ManagerBase> */ \
                    []() -> std::shared_ptr<ManagerBase> {  \
                        return std::make_shared<ManagerClassName>(); \
                    }, \
                    /* Initializer Lambda */ \
                    DEFINE_INIT_MANANGER_LAMBDA(ManagerNameString), \
                    /* Initializer con parámetros Lambda */ \
                    DEFINE_INIT_WITH_PARAMS_MANANGER_LAMBDA(ManagerNameString), \
                    /* Render Lambda */ \
                    DEFINE_RENDER_MANAGER_LAMBDA(ManagerNameString), \
                    /* Update Lambda */ \
                    DEFINE_UPDATE_MANAGER_LAMBDA(ManagerNameString) \
                ); \
            } \
        }; \
        static Manager ## ManagerClassName ## Registrar s_register_ ## ManagerClassName; \
    }