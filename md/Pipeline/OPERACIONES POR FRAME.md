# Operaciones por Frame en DirectX 11 🎮

Aquí tienes el resumen de las operaciones de renderizado en un frame típico, organizado en una tabla con los apartados que solicitaste.

## Flujo de Renderizado de un Frame Típico 🔄

| Fase | Operaciones | Descripción |
|------|-------------|-------------|
| **🎬 Inicio del Frame** | `ID3D11DeviceContext::OMSetRenderTargets` | Vincula los render targets (como el back buffer y el depth-stencil buffer) al pipeline. |
| | `ID3D11DeviceContext::ClearRenderTargetView` | Limpia el render target (la pantalla) con un color de fondo. |
| | `ID3D11DeviceContext::ClearDepthStencilView` | Limpia el depth-stencil buffer para las nuevas pruebas de profundidad. |
| | ✅`ID3D11DeviceContext::RSSetViewports` | Establece el área de la pantalla donde se va a dibujar. |
| | `ID3D11DeviceContext::VSSetConstantBuffers` | Vincula los constant buffers con datos que no cambian en todo el frame (ej. matrices de la cámara). |
| | `ID3D11DeviceContext::PSSetConstantBuffers` | Lo mismo que el anterior, pero para el pixel shader. |
| **🔄 Bucle por Cada Objeto (Mesh)** | `ID3D11DeviceContext::IASetInputLayout` | Vincula el input layout que describe el formato de los vértices. |
| | `ID3D11DeviceContext::VSSetShader` | Vincula el vertex shader del objeto. |
| | `ID3D11DeviceContext::PSSetShader` | Vincula el pixel shader del objeto. |
| | `ID3D11DeviceContext::IASetVertexBuffers` | Vincula el vertex buffer que contiene la geometría del objeto. |
| | `ID3D11DeviceContext::IASetIndexBuffer` | Vincula el index buffer para dibujar con índices. |
| | `ID3D11DeviceContext::VSSetConstantBuffers` | Vincula el constant buffer del objeto (ej. la matriz World). |
| | `ID3D11DeviceContext::PSSetShaderResources` | Vincula las texturas del objeto. |
| | `ID3D11DeviceContext::PSSetSamplers` | Vincula los sampler states para las texturas. |
| | `ID3D11DeviceContext::DrawIndexed` | Ejecuta el comando de dibujo para el objeto. |
| **🏁 Final del Frame** | `IDXGISwapChain::Present` | Muestra el back buffer en la pantalla. Esta operación es la única que debe hacerse solo una vez. |

## ⚠️ Operaciones Críticas que NO Deben Repetirse por Frame

| Operación | Razón |
|-----------|--------|
| `ID3D11DeviceContext::ClearRenderTargetView` | Borraría todo el trabajo de renderizado previo en el frame actual. |
| `ID3D11DeviceContext::ClearDepthStencilView` | Reiniciaría las pruebas de profundidad, causando problemas de oclusión. |
| `ID3D11DeviceContext::RSSetViewports` | Alteraría la región de renderizado para toda la geometría subsecuente. |
| `IDXGISwapChain::Present` | Podría causar tearing visual o comportamiento indefinido de la pantalla. |
| `ID3D11DeviceContext::OMSetRenderTargets` | Cambiaría el destino de renderizado, perdiendo el trabajo previo. |
