# Pase de Sombras vs. Pase Principal

|Característica	|Pase Principal (Color)	|Pase de Sombras|
|------|-------------|-------------|
|Recursos	|Reutiliza los recursos de inicialización (mallas, shaders).	|Reutiliza los mismos recursos.|
|Vista de Destino	|`ID3D11RenderTargetView` (el back buffer).	|Un `ID3D11DepthStencilView` (la superficie del mapa de sombras).|
|Shaders	|`Vertex shader` y `pixel shader` completos (para calcular el color).	|Solo el `vertex shader` (para calcular la profundidad). El pixel shader puede ser nulo o un shader muy simple.|
|Operaciones	|`Clear`, `SetRenderTarget`, `DrawIndexed`.	|`ClearDepthStencilView`, `SetDepthStencilView`, `DrawIndexed`.|
