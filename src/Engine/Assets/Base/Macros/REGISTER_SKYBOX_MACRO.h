#pragma once

#include "AssetLocator/AssetLocator.h"
#include "IWindowDependentInitializable.h"
#include "IInitializable.h"
#include "IRenderable.h"
#include "IManager.h"
#include "Assets/IAssetSkybox.h"

#define DEFINE_INIT_ASSET_LAMBDA(AssetSkyboxClassName) \
    [](std::shared_ptr<IAssetSkybox> servicePtr) -> HRESULT { \
        if (auto simpleInitializable = std::dynamic_pointer_cast<IInitializable>(servicePtr)) { \
            return simpleInitializable->Init(); \
        } \
        OutputDebugStringA(("ERROR: " AssetSkyboxClassName " does not implement a valid Init interface.\n")); \
        return E_FAIL; \
    }

#define DEFINE_RENDER_ASSET_LAMBDA(AssetSkyboxClassName) \
    [](std::shared_ptr<IAssetSkybox> servicePtr) -> HRESULT { \
        if (auto renderable = std::dynamic_pointer_cast<IRenderable>(servicePtr)) { \
            renderable->Render(); \
            return S_OK; /* O el HRESULT que devuelva Render() */ \
        } \
    OutputDebugStringA(("ERROR: " AssetSkyboxClassName " does not implement IRenderable.\n")); \
    return E_FAIL; \
    }

#define REGISTER_ASSET_TYPE(AssetSkyboxClassName, AssetSkyboxNameString) \
    namespace { \
        class AssetSkybox ## AssetSkyboxClassName ## Registrar { \
        public: \
            AssetSkybox ## AssetSkyboxClassName ## Registrar() { \
                AssetLocator::RegisterAssetCreator( \
                    AssetSkyboxNameString, \
                    /* Lambda de creación: Devuelve std::shared_ptr<AssetSkyboxClassName> que se convierte implícitamente a std::shared_ptr<IAssetSkybox> */ \
                    []() -> std::shared_ptr<IAssetSkybox> {  \
                        return std::make_shared<AssetSkyboxClassName>(); \
                    }, \
                    /* Initializer Lambda */ \
                    DEFINE_INIT_ASSET_LAMBDA(AssetSkyboxNameString), \
                    /* Render Lambda */ \
                    DEFINE_RENDER_ASSET_LAMBDA(AssetSkyboxNameString) \
                ); \
            } \
        }; \
        static AssetSkybox ## AssetSkyboxClassName ## Registrar s_register_ ## AssetSkyboxClassName; \
    }