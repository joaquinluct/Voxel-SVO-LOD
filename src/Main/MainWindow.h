#pragma once
//#include <stdafx.h>
#include <Defines/EngineDefinition.h>
#include <Engine.h> // Se incluye la clase de motor de juego, ahora llamada Engine.
#include <memory>
#include <windows.h>

//--------------------------------------------------------------------------------------
// Clase principal que gestiona la ventana y el bucle de la aplicación.
//--------------------------------------------------------------------------------------
class MainWindow {
public:
    //--------------------------------------------------------------------------------------
    // Constructor
    //--------------------------------------------------------------------------------------
    MainWindow(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);

    //--------------------------------------------------------------------------------------
    // Método para crear la ventana y ejecutar el bucle principal.
    //--------------------------------------------------------------------------------------
    int Create(int width, int height);

    //--------------------------------------------------------------------------------------
    // Métodos para el manejo de mensajes de la ventana, llamados desde WndProc.
    //--------------------------------------------------------------------------------------
    void HandleInput(UINT message, WPARAM wParam, LPARAM lParam);
    void OnResize(int width, int height);

private:
    //--------------------------------------------------------------------------------------
    // Puntero para la instancia del motor de juego.
    //--------------------------------------------------------------------------------------
    std::unique_ptr<Engine> m_gameEngine;

    // Contexto de la aplicación
    EngineContext m_context;

    //--------------------------------------------------------------------------------------
    // Variables de la ventana
    //--------------------------------------------------------------------------------------
    HINSTANCE m_hInstance = nullptr;
    HWND m_hWnd = nullptr;
    int m_nCmdShow = 0;

    //--------------------------------------------------------------------------------------
    // Métodos privados para la gestión de la ventana.
    //--------------------------------------------------------------------------------------
    bool InitializeWindow(int width, int height);
    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

    //--------------------------------------------------------------------------------------
    // Puntero a la instancia actual de MainWindow para su uso en la función WndProc estática.
    //--------------------------------------------------------------------------------------
    static MainWindow* s_pMainWindow;
    static LRESULT CALLBACK StaticWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};
