# Pipeline de Renderizado en DirectX 11

## Índice
1. [Diagrama de Flujo del Pipeline](#diagrama-de-flujo-del-pipeline)
2. [Descripción de las Etapas del Pipeline](#descripcion-de-las-etapas-del-pipeline)
3. [Operaciones clave de DirectX 11 por fase del pipeline](#operaciones-clave-de-directx-11-por-fase-del-pipeline)

## Diagrama de Flujo del Pipeline
+------------------+
|1. Input Assembler|
|      (IA)        |
+------------------+
+------------------+
|2. Vertex Shader  |
|      (VS)        |
+------------------+
+------------------+
|3. Hull Shader    |
|      (HS)        |   
+------------------+
+------------------+
|4.Tesselator Stage|
|      (TS)        | (Fijo, para Teselación)
+------------------+
+------------------+
|5. Domain Shader  |
|      (DS)        | (Opcional, para Teselación)
+------------------+
+------------------+
|6. Geometry Shader|
|      (GS)        | (Opcional)
+------------------+
+------------------+
|7. Stream Output  |
|      (SO)        | (Opcional)
+------------------+
+------------------+
|8. Rasterizer     |
|      (RS)        |
+------------------+
+------------------+
|10. Pixel Shader  |
|      (PS)        |
+------------------+
+------------------+
|11. Output Merger |
|      (OM)        |
+------------------+
## Descripción de las Etapas del Pipeline

### 1. Input Assembler (IA) - Ensamblador de Entrada

- **Función**: Lee los datos de tus modelos 3D (vértices, índices) de los buffers en la memoria de la GPU y los ensambla en primitivas (puntos, líneas, triángulos).
- **Entrada**: Vertex Buffers, Index Buffers, Formato de Layout de Entrada (Input Layout).
- **Salida**: Primitivas (ej. triángulos) listas para ser procesadas por el Vertex Shader.

### 2. Vertex Shader (VS) - Shader de Vértices

- **Función**: Procesa cada vértice de las primitivas de entrada. Su tarea principal es transformar las posiciones de los vértices del espacio de objeto al espacio de clip (coordenadas de pantalla 2D + profundidad). También puede calcular otras propiedades por vértice como normales, coordenadas de textura, colores, etc.
- **Programable**: Sí.
- **Entrada**: Datos de cada vértice.
- **Salida**: Vértices transformados con sus propiedades.

### 3. Hull Shader (HS), Tesselator Stage (TS) y Domain Shader (DS) - Etapas de Teselación (Opcionales)

Estas tres etapas trabajan juntas para añadir más geometría a la escena "sobre la marcha" (teselación), permitiendo que objetos con bajo número de polígonos se vean más detallados sin la necesidad de almacenar modelos con altísimo detalle.

- **Hull Shader (HS)**: Decide cómo se subdividirá una superficie (ej. cuántos vértices nuevos se crearán). Es programable.
- **Tessellator Stage (TS)**: Es una etapa fija de hardware que genera los nuevos vértices basándose en las instrucciones del Hull Shader.
- **Domain Shader (DS)**: Procesa los vértices generados por el Tessellator, dándoles sus posiciones finales y otras propiedades. Es programable.
- **Uso**: Se usan para superficies curvas, modelos con LOD adaptable, etc.

### 4. Geometry Shader (GS) - Shader de Geometría (Opcional)

- **Función**: Puede tomar una primitiva entera (punto, línea o triángulo) como entrada y generar una o más primitivas nuevas de cualquier tipo. Por ejemplo, puede generar un quad para cada punto, o expandir triángulos en primitivas más complejas.
- **Programable**: Sí.
- **Uso**: Explosiones de partículas, "finning" (añadir aletas a los modelos), renderizado de pelo, etc. A menudo se evita por su coste de rendimiento en algunas arquitecturas.

### 5. Stream Output (SO) - Salida de Flujo (Opcional)

- **Función**: Permite que los datos de las primitivas (vértices, etc.) que salen del Geometry Shader (o del Vertex Shader si no hay GS) sean enviados de vuelta a la memoria de la GPU en un buffer, en lugar de continuar por el pipeline de renderizado.
- **Programable**: No (es una etapa de hardware configurable).
- **Uso**: Feedback loops (cálculos en un frame para el siguiente), culling en la GPU, simulaciones de partículas.

### 6. Rasterizer (RS) - Rasterizador

- **Función**: Es una etapa fija del hardware que toma las primitivas transformadas (triángulos, etc.) y las convierte en fragmentos o píxeles. Determina qué píxeles en la pantalla cubre cada primitiva. También realiza el "culling" (descarte de primitivas que están fuera de la vista o que miran hacia atrás) y el "clipping" (recorte de primitivas que cruzan los límites de la pantalla).
- **Fijo (Configurable)**: Sí (configuras sus parámetros con ID3D11RasterizerState).
- **Salida**: Una lista de píxeles (fragmentos) y sus propiedades interpoladas (ej. coordenadas de textura, normales) para cada primitiva.

### 7. Pixel Shader (PS) - Shader de Píxeles

- **Función**: Procesa cada píxel generado por el Rasterizador. Es aquí donde se realiza la mayoría de los cálculos de iluminación, muestreo de texturas, cálculo de sombras, efectos de post-procesado, etc., para determinar el color final de cada píxel.
- **Programable**: Sí.
- **Entrada**: Propiedades interpoladas de los vértices (ej. coordenadas de textura, normales en el espacio mundo, posición en el espacio de la luz).
- **Salida**: El color final (y opcionalmente otros valores como normales o datos de material para renderizado diferido) para cada píxel.

### 8. Output Merger (OM) - Mezclador de Salida

- **Función**: Es la etapa final donde los píxeles procesados por el Pixel Shader se combinan con el contenido existente en los Render Targets y el Depth/Stencil Buffer. Aquí se realizan la prueba de profundidad (depth test), la prueba de stencil (stencil test), y la mezcla de color (blending).
- **Fijo (Configurable)**: Sí (configuras sus parámetros con ID3D11DepthStencilState y ID3D11BlendState).
- **Entrada**: Píxeles de salida del Pixel Shader, contenidos actuales del Render Target y Depth/Stencil Buffer.
- **Salida**: Los píxeles finales escritos en el Render Target (que eventualmente irán a la pantalla) y la información de profundidad/stencil actualizada.

---

## Operaciones clave de DirectX 11 por fase del pipeline

### 1. Input Assembler (IA) - Ensamblador de Entrada

Esta etapa se encarga de leer y ensamblar los datos de los vértices y los índices de tus mallas.

- `ID3D11DeviceContext::IASetInputLayout()`: Vincula un objeto de layout de entrada que describe cómo los datos de los vértices están organizados en el Vertex Buffer.
- `ID3D11DeviceContext::IASetVertexBuffers()`: Vincula uno o más Vertex Buffers al pipeline.
- `ID3D11DeviceContext::IASetIndexBuffer()`: Vincula un Index Buffer al pipeline (si estás usando indexado).
- `ID3D11DeviceContext::IASetPrimitiveTopology()`: Define el tipo de primitiva a dibujar (ej. lista de triángulos, lista de líneas, puntos).

### 2. Vertex Shader (VS) - Shader de Vértices

El Vertex Shader procesa cada vértice individualmente.

- `ID3D11DeviceContext::VSSetShader()`: Vincula tu Vertex Shader compilado al pipeline.
- `ID3D11DeviceContext::VSSetConstantBuffers()`: Vincula Constant Buffers que contienen datos que son constantes para un dibujo o grupo de dibujos (ej. matrices World, View, Projection).
- `ID3D11DeviceContext::VSSetShaderResources()`: (Menos común en VS para renderizado estándar, más para Compute Shaders o pasajes específicos). Vincula Shader Resource Views para leer datos de texturas o buffers.
- `ID3D11DeviceContext::VSSetSamplers()`: (Menos común). Vincula Sampler States para el muestreo de texturas.

### 3. Hull Shader (HS), Tesselator Stage (TS), Domain Shader (DS) - Teselación (Opcionales)

Estas etapas se usan para generar geometría adicional a partir de parches de control.

- `ID3D11DeviceContext::HSSetShader()`: Vincula tu Hull Shader.
- `ID3D11DeviceContext::DSSetShader()`: Vincula tu Domain Shader.
- `ID3D11DeviceContext::HSSetConstantBuffers()`, `ID3D11DeviceContext::DSSetConstantBuffers()`: Vinculan Constant Buffers para estos shaders.
- `ID3D11DeviceContext::HSSetShaderResources()`, `ID3D11DeviceContext::DSSetShaderResources()`: Vinculan recursos para estos shaders.
- `ID3D11DeviceContext::HSSetSamplers()`, `ID3D11DeviceContext::DSSetSamplers()`: Vinculan samplers para estos shaders.

### 4. Geometry Shader (GS) - Shader de Geometría (Opcional)

El Geometry Shader puede tomar una primitiva entera y generar cero o más primitivas nuevas.

- `ID3D11DeviceContext::GSSetShader()`: Vincula tu Geometry Shader.
- `ID3D11DeviceContext::GSSetConstantBuffers()`: Vincula Constant Buffers para el Geometry Shader.
- `ID3D11DeviceContext::GSSetShaderResources()`: Vincula recursos para el Geometry Shader.
- `ID3D11DeviceContext::GSSetSamplers()`: Vincula samplers para el Geometry Shader.

### 5. Stream Output (SO) - Salida de Flujo (Opcional)

Permite enviar los datos de las primitivas de vuelta a un buffer en la memoria de la GPU.

- `ID3D11DeviceContext::SOSetTargets()`: Configura los buffers a los que se escribirán los datos de salida.
- (`ID3D11DeviceContext::DrawAuto()`: Se usa en combinación con Stream Output para dibujar lo que se ha escrito en los buffers sin volver a pasar por el Input Assembler.)

### 6. Rasterizer (RS) - Rasterizador

Esta etapa fija convierte las primitivas en píxeles y aplica culling, clipping y la configuración del viewport.

- `ID3D11DeviceContext::RSSetState()`: Vincula un objeto ID3D11RasterizerState que define el modo de relleno, culling, profundidad de bias, etc.
- `ID3D11DeviceContext::RSSetViewports()`: Define la región rectangular de la pantalla donde se dibujará la salida del rasterizador.
- `ID3D11DeviceContext::RSSetScissorRects()`: Define rectángulos de tijera para limitar el área de dibujo.

### 7. Pixel Shader (PS) - Shader de Píxeles

El Pixel Shader calcula el color final de cada píxel.

- `ID3D11DeviceContext::PSSetShader()`: Vincula tu Pixel Shader compilado.
- `ID3D11DeviceContext::PSSetConstantBuffers()`: Vincula Constant Buffers para el Pixel Shader (ej. datos de luz, de cámara, de material).
- `ID3D11DeviceContext::PSSetShaderResources()`: Crucial. Vincula las Shader Resource Views para las texturas de color, normales, rugosidad, metalicidad, oclusión ambiental, y el mapa de sombras.
- `ID3D11DeviceContext::PSSetSamplers()`: Crucial. Vincula los Sampler States para el muestreo de texturas (incluido el SamplerComparisonState para el mapa de sombras).

### 8. Output Merger (OM) - Mezclador de Salida

Esta es la etapa final donde los píxeles se combinan con el Render Target y se realizan pruebas de profundidad/stencil y blending.

- `ID3D11DeviceContext::OMSetRenderTargets()`: Vincula uno o más Render Target Views (RTVs) y, opcionalmente, un Depth Stencil View (DSV) a la etapa de Output Merger. Esta es la función que te permite dibujar en tu Swap Chain, tu mapa de sombras, o cualquier Render Target intermedio.
- `ID3D11DeviceContext::OMSetDepthStencilState()`: Vincula un objeto ID3D11DepthStencilState que define cómo se realizan la prueba de profundidad y las operaciones de stencil.
- `ID3D11DeviceContext::OMSetBlendState()`: Vincula un objeto ID3D11BlendState que define cómo se combinan los colores salientes del Pixel Shader con los colores existentes en el Render Target.

### Operaciones Comunes de Limpieza y Dibujo

- `ID3D11DeviceContext::ClearRenderTargetView()`: Limpia un Render Target (ej. el color de fondo).
- `ID3D11DeviceContext::ClearDepthStencilView()`: Limpia el Depth Stencil Buffer (ej. establece la profundidad a 1.0 y stencil a 0).
- `ID3D11DeviceContext::Draw()`: Realiza una llamada de dibujo sin usar un Index Buffer.
- `ID3D11DeviceContext::DrawIndexed()`: Realiza una llamada de dibujo utilizando un Index Buffer (lo más común y eficiente).