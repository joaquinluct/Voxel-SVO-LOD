#pragma once

#include <ManagerLocator/ManagerLocator.h>
#include "IWindowDependentInitializable.h"
#include "IInitializable.h"
#include "IRenderable.h"
#include "IManager.h"
#include "IService.h"

#define DEFINE_INIT_MANANGER_LAMBDA(ManagerClassName) \
    [](std::shared_ptr<IManager> servicePtr, HWND hwnd, int width, int height) -> HRESULT { \
        if (auto windowDependent = std::dynamic_pointer_cast<IWindowDependentInitializable>(servicePtr)) { \
            return windowDependent->Init(hwnd, width, height); \
        } \
        if (auto simpleInitializable = std::dynamic_pointer_cast<IInitializable>(servicePtr)) { \
            return simpleInitializable->Init(); \
        } \
        OutputDebugStringA(("ERROR: " ManagerClassName " does not implement a valid Init interface.\n")); \
        return E_FAIL; \
    }

#define DEFINE_RENDER_MANAGER_LAMBDA(ManagerClassName) \
    [](std::shared_ptr<IManager> servicePtr) -> HRESULT { \
        if (auto renderable = std::dynamic_pointer_cast<IRenderable>(servicePtr)) { \
            renderable->Render(); \
            return S_OK; /* O el HRESULT que devuelva Render() */ \
        } \
    OutputDebugStringA(("ERROR: " ManagerClassName " does not implement IRenderable.\n")); \
    return E_FAIL; \
    }

#define DEFINE_UPDATE_MANAGER_LAMBDA(ManagerClassName) \
    [](std::shared_ptr<IManager> servicePtr, float deltaTime) -> HRESULT { \
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
                    /* Lambda de creación: Devuelve std::shared_ptr<ManagerClassName> que se convierte implícitamente a std::shared_ptr<IManager> */ \
                    []() -> std::shared_ptr<IManager> {  \
                        return std::make_shared<ManagerClassName>(); \
                    }, \
                    /* Initializer Lambda */ \
                    DEFINE_INIT_MANANGER_LAMBDA(ManagerNameString), \
                    /* Render Lambda */ \
                    DEFINE_RENDER_MANAGER_LAMBDA(ManagerNameString), \
                    /* Update Lambda */ \
                    DEFINE_UPDATE_MANAGER_LAMBDA(ManagerNameString) \
                ); \
            } \
        }; \
        static Manager ## ManagerClassName ## Registrar s_register_ ## ManagerClassName; \
    }