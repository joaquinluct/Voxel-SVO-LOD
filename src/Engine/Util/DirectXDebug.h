#pragma once

// Define the Windows version level to enable functions from Windows 10 or later.
// 0x0A00 corresponds to Windows 10 and Windows 11.
#define _WIN32_WINNT 0x0A00

// Includes the necessary header files for DXGI and D3D11 debugging functions.
#include <combaseapi.h>
#include <cstdio> // For printf
#include <dxgi1_5.h>
#include <dxgidebug.h>

// Asegúrate de que tu proyecto también esté enlazado con 'dxguid.lib'.
// Esto se hace en las propiedades del proyecto -> Enlazador -> Entrada -> Dependencias adicionales.

void ReportLiveObjects()
{
    HRESULT hr = S_OK;
    IDXGIDebug1* dxgiDebug = nullptr;

    // DXGIGetDebugInterface1 es la función moderna que toma 3 parámetros.
    // El primer parámetro, 'Flags', se utiliza para especificar el comportamiento. Aquí es 0.
    // El segundo parámetro es el ID de la interfaz, que es IID_PPV_ARGS(&dxgiDebug).
    // El último parámetro es el puntero a la interfaz IDXGIDebug1.
    // La interfaz debe ser de tipo IDXGIDebug1 (con '1') para que coincida.
    hr = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug));

    if (SUCCEEDED(hr))
    {
        printf("Reportando objetos de D3D11 en vivo...\n");

        // Genera un informe detallado de todos los objetos "vivos" (Refcount > 0).
        hr = dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);

        if (FAILED(hr))
        {
            printf("Fallo al generar el reporte de objetos en vivo.\n");
        }

        // ¡Importante! Liberar la referencia a la interfaz de depuración.
        dxgiDebug->Release();
    }
    else
    {
        printf("Fallo al obtener la interfaz de depuración DXGI.\n");
        printf("Asegúrate de que el dispositivo D3D11 fue creado con la bandera D3D11_CREATE_DEVICE_DEBUG.\n");
    }
}
