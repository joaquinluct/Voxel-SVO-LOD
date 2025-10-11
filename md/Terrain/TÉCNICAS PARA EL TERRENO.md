## 1. Estrategias de Terreno para MUNDOS INFINITOS (Streaming Constante)

Este escenario requiere una alta eficiencia en el streaming de datos y una baja latencia, priorizando sistemas que gestionen el LOD de forma continua y modular (por Chunk) para evitar tirones.

| Técnica de Render                                  | Nombre de Clase de Chunk                     | Draw Calls (por Frame)         | Carga de Geometría por Frame                                                                                   |
|----------------------------------------------------|----------------------------------------------|-------------------------------:|----------------------------------------------------------------------------------------------------------------|
| LOD Fijo / Chunks Agregados (Ruta Inicial)         | AggregatedChunk o MeshChunk                  | Bajo (1-3) (uno por grupo/Tier) | Alto, pero Presupuestado: Reconstrucción de ≤1 grupo grande de chunks por frame.                               |
| Teselación (GPU LOD)                               | TessellationChunk o InstancedChunk           | Muy Bajo (1)                   | Mínima: Solo se suben los Instance Buffers dinámicos (Matriz World + Factor LOD) de los chunks visibles.       |
| A-QTM (Clipmaps) con Skirts                        | QuadTreeNodeChunk o AQTMTile                 | Bajo (4-8)                     | Bajo: Solo se suben/reorganizan los buffers de los anillos de LOD que se mueven o necesitan stitching.          |
| GPU-Driven (Mesh Shaders)                          | VisibilityChunk o MeshletChunk               | Extremadamente Bajo (1-2)      | Bajo: La CPU solo sube comandos indirectos; la GPU gestiona la geometría.                                       |

## 2. Estrategias de Terreno para MUNDOS FINITOS (Precarga Estática)

Este escenario permite la precarga y simplifica la gestión del streaming. La prioridad es la eficiencia del Draw Call y la calidad visual de la malla estática.

| Técnica de Render                                  | Nombre de Clase de Chunk (si aplica)         | Draw Calls (por Juego)        | Carga de Geometría                                                                                              |
|----------------------------------------------------|----------------------------------------------|------------------------------:|------------------------------------------------------------------------------------------------------------------|
| Malla Estática Única (LOD Fijo)                    | N/A (un solo buffer global)                  | 1                             | Alta: Ocurre solo una vez, en la pantalla de carga inicial.                                                      |
| Teselación (con Displacement)                      | TessellationChunk                             | 1 a 4                         | Baja: Solo se sube la malla base de baja resolución del mundo completo.                                         |
| R-QTM Fijo (Simplificación Fija)                   | QuadTreeNodeChunk                              | Bajo (4-8)                    | Media: Se precargan todos los buffers de las distintas regiones de LOD.                                         |
| Mega-Texturing (Solo Altura)                       | N/A                                           | 1                             | Baja: La geometría es mínima; el coste está en el streaming de texturas de altura (heightmaps).                 |


## Estrategia Combinada: LOD Dinámico (CPU) + Teselación (GPU)

Esta es la aproximación más utilizada en la industria para lograr un equilibrio óptimo entre la gestión de datos escalable y el detalle gráfico de alto rendimiento.

| Componente del Pipeline     | Tarea Principal                                                                 | Ubicación (Lógica)                        | Resultado                                                                                                  |
|----------------------------|----------------------------------------------------------------------------------|-------------------------------------------|-----------------------------------------------------------------------------------------------------------|
| LOD Dinámico (CPU)         | Gestión gruesa y streaming: determina visibilidad, distancia y asigna LOD/teselación. | `TerrainDistributionEngine`               | Crea un Instancing Buffer dinámico (pequeño) con los parámetros de cada chunk visible.                    |
| Teselación (GPU)           | Generación de detalle fino: recibe patch base y factor LOD; subdivide y aplica displacement. | `TessellationGeometryEngine` (Shaders)    | Dibuja la malla final altamente detallada.                                                                 |
| Dibujado Final             | Draw calls eficientes: usa los datos de instancia de la CPU para dibujar el mundo en una sola llamada. | Hilo de Render                             | 1 único `DrawIndexedInstanced` para todo el terreno visible.                                              |

## Nomenclatura Modular Recomendada para el Framework

Para mantener la flexibilidad de tu framework (tipo Unity), los nombres de las clases deben reflejar su función específica dentro del pipeline combinado.

| Clase / Módulo              | Tipo de Módulo (YAML)         | Responsabilidad Clave                                                                 |
|----------------------------|-------------------------------|---------------------------------------------------------------------------------------|
| `TessellationChunk`         | Clase Chunk                   | Almacenar solo la data de instancia (Matriz World, Factor LOD, Bounding Box). No almacena V/I Buffers completos. |
| `AQTMDistanceLODEngine`     | `TerrainDistributionEngine`   | Lógica de CPU de gestión de chunks, cálculo de LOD por distancia, culling, y generación/actualización del Instancing Buffer. |
| `TessellationGeometryEngine`| `TerrainGeometryEngine`       | Lógica de GPU: contiene la malla patch base, los shaders de Teselación (Hull y Domain), y la llamada a `DrawIndexedInstanced`. |
| `InstancingRingBuffer`      | Abstracción de Buffer         | Manejar el streaming eficiente y presupuestado de los datos dinámicos de `TessellationChunk` a la VRAM (sin picos de latencia). |


## Configuraciones YAML recomendadas — Mapas de Chunk y Propósito

| Tipo de Configuración YAML                                        | Clase de Chunk Instanciada                                                                 | Propósito                                                                                         |
|-------------------------------------------------------------------|-------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------------------|
| `geometry_engine: GeomtryChunkEngine` (Malla Agregada)            | `ChunkMesh` — Almacena `m_mesh` (V/I Buffers) localmente.                                 | Para sistemas simples/básicos (sin teselación).                                                  |
| `geometry_engine: TessellationEngine`                             | `ChunkTessellation` — Almacena solo la Matriz World, Factor LOD y referencia al Patch base estático. | Para sistemas DX11 de alto rendimiento.                                                           |



