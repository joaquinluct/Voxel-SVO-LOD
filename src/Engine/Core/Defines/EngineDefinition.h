#pragma once
#include <windows.h>

namespace EngineDefinition
{
    struct Context
    {
        Context() : deltaTime(0.0f), isInitialized(false), isRunning(false), width(0.0f), height(0.0f), hInstance(nullptr), hWnd(nullptr), nCmdShow(0) {}

        float deltaTime;
        bool isInitialized;
        bool isRunning;
        float width;
        float height;

        HINSTANCE hInstance;
        HWND* hWnd;
        int nCmdShow;
    };

} // namespace EngineDefinition

using EngineContext = EngineDefinition::Context;
