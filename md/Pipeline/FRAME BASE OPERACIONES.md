# Pipeline de Renderizado Direct3D 11

Este documento describe la secuencia completa de operaciones de renderizado en Direct3D 11, mostrando los recursos y métodos D3D11 asociados.

## ?? 1. Operaciones de Inicialización por Frame

> Estas operaciones se ejecutan una vez por fotograma

### Limpieza del Buffer

<div class="operation-block">
PipelineOperationType::Device_ClearRenderTargetView

<hr class="separator" />
[Recurso] ID3D11RenderTargetView [/Recurso]
<hr class="separator" />

[Descripción] Limpia el búfer de destino de renderizado con un color sólido para empezar a dibujar. Es como borrar un lienzo antes de pintar. [/Descripción]
</div>

<div class="operation-block">
PipelineOperationType::Mesh_Render_SetInputLayout

<hr class="separator" />
[Recurso] ID3D11InputLayout [/Recurso]
<hr class="separator" />

[Descripción] Define la estructura de los datos de vértice (posiciones, normales, coordenadas de textura) que el sombreador de vértices espera recibir. [/Descripción]
</div>

## ?? 2. Operaciones para Cada Malla

> Estas operaciones se ejecutan una vez por cada DrawIndexed

### Configuración de la Malla

<div class="operation-block">
PipelineOperationType::Mesh_Render_SetPrimitiveToplogy

<hr class="separator" />
[Recurso] D3D11_PRIMITIVE_TOPOLOGY [/Recurso]
<hr class="separator" />

[Descripción] Le dice a la GPU cómo interpretar los vértices, por ejemplo, como triángulos (TriangleList), líneas o puntos. [/Descripción]
</div>

<div class="operation-block">
PipelineOperationType::Mesh_Render_SetInputLayout

<hr class="separator" />
[Recurso] ID3D11InputLayout [/Recurso]
<hr class="separator" />

[Descripción] Vuelve a enlazar el diseño de entrada para la malla actual, confirmando la estructura de los datos de vértice. [/Descripción]
</div>

<div class="operation-block">
PipelineOperationType::Mesh_Render_SetVertexBuffer

<hr class="separator" />
[Recurso] ID3D11Buffer (VertexBuffer) [/Recurso]
<hr class="separator" />

[Descripción] Enlaza el búfer de vértices a la fase de ensamblador de entrada, proporcionando los datos geométricos del modelo. [/Descripción]
</div>

<div class="operation-block">
PipelineOperationType::Mesh_Render_SetIndexBuffer

<hr class="separator" />
[Recurso] ID3D11Buffer (IndexBuffer) [/Recurso]
<hr class="separator" />

[Descripción] Enlaza el búfer de índices, que es una lista de números que la GPU utiliza para reutilizar vértices y formar primitivas de manera eficiente. [/Descripción]
</div>

### Operaciones de Configuración de Shaders

<div class="operation-block">
PipelineOperationType::Mesh_Render_SetVertexShader

<hr class="separator" />
[Recurso] ID3D11VertexShader [/Recurso]
<hr class="separator" />

[Descripción] Enlaza el programa del sombreador de vértices, que es responsable de transformar la geometría 3D en la pantalla 2D. [/Descripción]
</div>

<div class="operation-block">
PipelineOperationType::Mesh_Render_SetPixelShader

<hr class="separator" />
[Recurso] ID3D11PixelShader [/Recurso]
<hr class="separator" />

[Descripción] Enlaza el sombreador de píxeles, que calcula el color final de cada píxel en la pantalla. [/Descripción]
</div>

<div class="operation-block">
PipelineOperationType::Mesh_Render_SetConstantBuffers

<hr class="separator" />
[Recurso] ID3D11Buffer (ConstantBuffer) [/Recurso]
<hr class="separator" />

[Descripción] Enlaza los búferes de constantes, que contienen datos globales (como matrices de transformación o propiedades de material) que los sombreadores necesitan. [/Descripción]
</div>

### Operaciones de Estados y Recursos

<div class="operation-block">
PipelineOperationType::Mesh_Render_SetShaderResources

<hr class="separator" />
[Recurso] ID3D11ShaderResourceView [/Recurso]
<hr class="separator" />

[Descripción] Enlaza los recursos de sombreador (como texturas) a la fase de sombreador, haciéndolos disponibles para ser muestreados. [/Descripción]
</div>

<div class="operation-block">
PipelineOperationType::Mesh_Render_SetSamplerState

<hr class="separator" />
[Recurso] ID3D11SamplerState [/Recurso]
<hr class="separator" />

[Descripción] Enlaza los estados del muestreador, que definen cómo se leen los datos de textura (filtrado, modo de envoltura, etc.). [/Descripción]
</div>

<div class="operation-block">
PipelineOperationType::Mesh_Render_SetDepthStencilState

<hr class="separator" />
[Recurso] ID3D11DepthStencilState [/Recurso]
<hr class="separator" />

[Descripción] Enlaza el estado de prueba de profundidad y galería de símbolos, que se utiliza para determinar qué píxeles son visibles y cuáles están ocultos por otros objetos. [/Descripción]
</div>

<div class="operation-block">
PipelineOperationType::Mesh_Render_SetRasterizerState

<hr class="separator" />
[Recurso] ID3D11RasterizerState [/Recurso]
<hr class="separator" />

[Descripción] Enlaza el estado del rasterizador, que controla el comportamiento de la etapa de rasterización (por ejemplo, el modo de relleno o el descarte de caras frontales/traseras). [/Descripción]
</div>

## ?? 3. Operación de Dibujo

> La operación final por cada malla

<div class="operation-block">
PipelineOperationType::Device_drawIndexed

<hr class="separator" />
[Recurso] Búferes enlazados por las operaciones anteriores [/Recurso]
<hr class="separator" />

[Descripción] La operación final que le dice a la GPU que ejecute el pipeline de renderizado y dibuje la malla utilizando los vértices e índices proporcionados. [/Descripción]
</div>

## ?? 4. Operación Final del Frame

> Estas operaciones se ejecutan una vez por fotograma

<div class="operation-block">
PipelineOperationType::Device_PresentSwapChain

<hr class="separator" />
[Recurso] IDXGISwapChain [/Recurso]
<hr class="separator" />

[Descripción] Intercambia el búfer trasero (donde se ha dibujado el fotograma) con el búfer delantero (que está en pantalla), mostrando el resultado final al usuario. [/Descripción]
</div>

<!-- Estilo actualizado para los bloques -->
<style>
.operation-block {
    margin: 20px 0;
    padding: 15px;
    border: 1px solid #666;
    border-radius: 8px;
    background-color: #2b2b2b;
    color: #e0e0e0;
    font-family: 'Consolas', monospace;
}

.separator {
    border: 0;
    height: 1px;
    background-image: linear-gradient(to right, rgba(255,255,255,0), rgba(255,255,255,0.75), rgba(255,255,255,0));
    margin: 15px 0;
}

[Recurso] {
    display: inline-block;
    padding: 5px 10px;
    background-color: #3b3b3b;
    border: 1px solid #90caf9;
    border-radius: 6px;
    color: #90caf9;
    font-family: 'Consolas', monospace;
    font-weight: 500;
}

[Operación] {
    display: inline-block;
    padding: 5px 10px;
    background-color: #3b3b3b;
    border: 1px solid #ce93d8;
    border-radius: 6px;
    color: #ce93d8;
    font-family: 'Consolas', monospace;
}

[Descripción] {
    display: block;
    margin-top: 10px;
    color: #b0b0b0;
    font-style: italic;
    line-height: 1.4;
}

.operation-block > *:first-child {
    color: #4CAF50;
    font-weight: bold;
    margin-bottom: 10px;
}
</style>
