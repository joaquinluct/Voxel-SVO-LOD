#pragma once

#include <variant>
#include <RenderManager/Passes/MainColorPass.h>
#include <RenderManager/Passes/ShadowPass.h>
#include <RenderManager/RenderPass.h>

using PassVariant = std::variant<MainColorPass, ShadowPass>;