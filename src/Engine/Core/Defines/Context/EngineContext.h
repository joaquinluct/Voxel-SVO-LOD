#pragma once
#include <Defines/Context/DirectXContext.h>
#include <windows.h>

struct EngineContext
{
    EngineContext() : deltaTime(0.0f), isInitialized(false), isRunning(false), width(0.0f), height(0.0f), hInstance(nullptr), hWnd(nullptr), nCmdShow(0) {}

    float deltaTime;
    bool isInitialized;
    bool isRunning;
    float width;
    float height;

    HINSTANCE hInstance;
    HWND* hWnd;
    int nCmdShow;

    DirectXContext directX;
};

