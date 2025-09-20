#pragma once

#include "AssetLocator/AssetLocator.h"
#include "IEngineDependentInitializable.h"
#include "IInitializable.h"
#include "IRenderable.h"
#include <Assets/Base/AssetBase.h>

#define DEFINE_INIT_ASSET_LAMBDA(AssetClassName) \
    [](std::shared_ptr<AssetBase> servicePtr) -> HRESULT { \
        if (auto simpleInitializable = std::dynamic_pointer_cast<IInitializable>(servicePtr)) { \
            return simpleInitializable->Init(); \
        } \
        OutputDebugStringA(("ERROR: " AssetClassName " does not implement a valid Init interface.\n")); \
        return E_FAIL; \
    }

#define DEFINE_RENDER_ASSET_LAMBDA(AssetClassName) \
    [](std::shared_ptr<AssetBase> servicePtr) -> HRESULT { \
        if (auto renderable = std::dynamic_pointer_cast<IRenderable>(servicePtr)) { \
            renderable->Render(); \
            return S_OK; /* O el HRESULT que devuelva Render() */ \
        } \
    OutputDebugStringA(("ERROR: " AssetClassName " does not implement IRenderable.\n")); \
    return E_FAIL; \
    }

#define REGISTER_ASSET_TYPE(AssetClassName, AssetNameString) \
    namespace { \
        class Asset ## AssetClassName ## Registrar { \
        public: \
            Asset ## AssetClassName ## Registrar() { \
                AssetLocator::RegisterAssetCreator( \
                    AssetNameString, \
                    /* Lambda de creación: Devuelve std::shared_ptr<AssetClassName> que se convierte implícitamente a std::shared_ptr<AssetBase> */ \
                    []() -> std::shared_ptr<AssetBase> {  \
                        return std::make_shared<AssetClassName>(); \
                    }, \
                    /* Initializer Lambda */ \
                    DEFINE_INIT_ASSET_LAMBDA(AssetNameString), \
                    /* Render Lambda */ \
                    DEFINE_RENDER_ASSET_LAMBDA(AssetNameString) \
                ); \
            } \
        }; \
        static Asset ## AssetClassName ## Registrar s_register_ ## AssetClassName; \
    }