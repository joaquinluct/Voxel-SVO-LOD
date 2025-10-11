# Tipos de Chunk Modulares y Árbol de Decisiones

La matriz de posibilidades es grande, pero podemos condensar las divisiones en tres ejes principales:

- **Gestión Espacial (CPU):** ¿Cómo se organizan los chunks en el mundo?
- **Estrategia de Geometría/LOD (Render):** ¿Cómo se dibujan y simplifican?
- **Tipo de Malla (Datos):** ¿Qué tipo de datos almacena el Chunk?

El tipo de Chunk a instanciar se define principalmente por la Estrategia de Geometría/LOD y la Gestión del Terreno.

## Tabla de Casos Posibles y el Chunk Resultante
A continuación se presentan los tipos de Chunk (implementaciones de `IChunk`) más importantes, basados en las selecciones que un usuario podría hacer en tu menú modular.

| Opción de Usuario (Decisión Clave) | Tipo de Gestión del Terreno (Espacial) | Nombre de Clase de Chunk Sugerida | Propósito y Responsabilidad Principal |
|------------------------------------|----------------------------------------|-----------------------------------|--------------------------------------|
| Teselación (GPU)                   | Cualquiera (Chunk, QuadTree, OctTree)  | `TessellationChunk`               | Contenedor mínimo para datos de instancing (Matriz World, Factor LOD, Texturas de altura). Optimizado para `DrawIndexedInstanced`. |
| LOD Fijo (CPU) / Agregación        | Chunk (Parrilla Regular)               | `AggregatedMeshChunk`             | Contenedor de su propia malla completa (V/I Buffers). Responsable de la reconstrucción asíncrona y presupuestada de la malla al cambiar de LOD. |
| LOD Dinámico (CPU)                 | QuadTree (A-QTM / Clipmaps)            | `QuadTreeNodeChunk`               | Contenedor de la geometría patch. Responsable de la lógica de stitching (cosido de bordes) con los vecinos y de la subdivisión/fusión del quadtree. |
| GPU-Driven Rendering               | Cualquiera (Requiere DX12/Vulkan)      | `MeshletChunk`                    | Contenedor de pequeños buffers de vértices/triángulos (meshlets) y datos de culling. Optimizado para `ExecuteIndirect`. |
| Terreno Destruible (Vóxel)         | OctTree                                | `VoxelOctNodeChunk`               | Contenedor de los datos de densidad/material del vóxel. La geometría se genera con Dual Contouring (o similar) en tiempo real. |

Exportar a Hojas de cálculo

---

## El Árbol de Decisiones: Cómo se Fuerza la Elección
Tu framework debe seguir una lógica determinista para forzar la instanciación de la implementación correcta.

Fórmula resumida:

`ChunkInstanciado = f(Teselación, LOD_Dinámico, Gestión_del_Terreno)`

A continuación una tabla con las implicaciones más comunes:

| Opción de Usuario                                    | Implicación de Render                                          | Tipo de Chunk Forzado                                                                 |
|-----------------------------------------------------|----------------------------------------------------------------|---------------------------------------------------------------------------------------|
| Teselación: Sí                                      | Implica LOD fino en GPU.                                       | `TessellationChunk` (se necesita Matriz World y LOD Factor).                         |
| Teselación: No, LOD Dinámico: Sí                    | Implica LOD grueso en CPU con reconstrucción asíncrona.        | `AggregatedMeshChunk` (se necesita almacenar una malla por cada LOD).               |
| Gestión: QuadTree (y no Teselación)                 | Implica control de stitching y fusión/subdivisión.             | `QuadTreeNodeChunk` (se necesita lógica de vecino y árbol).                         |
| Geometría: Vóxel                                   | Implica generación de malla a partir de densidad.              | `VoxelOctNodeChunk` (cambia completamente la geometría subyacente).                |

Exportar a Hojas de cálculo
