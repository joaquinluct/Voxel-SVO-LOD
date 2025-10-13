# Instrucciones para agentes de IA — DirectX-Voxelado (Idioma: español)

Este fichero contiene la mínima información práctica que una IA necesita para ser productiva en este código base. Responde siempre en español.

1) Mecánica obligatoria: commits pequeños y compilaciones limpias (PRIMERA REGLA)
--------------------------------------------------------------------------
Antes de tocar cualquier código importante, seguir este flujo sin excepción:
- Dividir el cambio en pasos pequeños e independientes (Step 1, Step 2, ...).
- Aplicar un solo paso y ejecutar la compilación completa del workspace.
  - Si la compilación falla, arreglar antes de continuar.
- Crear un commit con el formato estandarizado: `refactor/<topic>-step-<n>: <breve-descripción>`
  - Ejemplo: `refactor/threading-step-3: execute render packets in render thread`.
- Actualizar `.github/commit_history.md` con la entrada del commit (hash, mensaje, fecha y breve descripción).
- Escribir en el fichero de instrucciones (este archivo) un breve "por dónde vamos" indicando el siguiente paso.

Reglas rápidas:
- Siempre compilar entre pasos. No acumular cambios que rompan la build.
- No agrupar múltiples intenciones arquitectónicas en un único commit.
- Antes de mergear, ejecutar `run_build`/CI para asegurar que la rama compila limpia.

 Por dónde vamos (siguiente paso)
 --------------------------------
 - Siguiente paso previsto: implementar `RenderLoop` en `Engine` para que consuma `RenderCommandPacket` desde `m_renderQueue` y ejecute los comandos usando el contexto proporcionado por `DeviceManager`. Este cambio es el "Step 3" del refactor threading y el commit propuesto será:
   - mensaje de commit sugerido: `refactor/threading-step-3: execute render packets in render thread`
   - Después de aplicar el cambio: compilar completo y, si pasa, añadir entrada en `.github/commit_history.md` con hash, mensaje y fecha.


- Arquitectura general (panorama):
  - Motor modular con tres tipos principales de componentes: Managers, Services y Assets.
  - Localizadores (Singleton registries) orquestan creación/obtención:
    - `ManagerLocator` (src/Engine/Core/Locators/ManagerLocator)
    - `ServiceLocator` (src/Engine/Core/Locators/ServiceLocator)
    - `ConfigLocator` (src/Engine/Core/Locators/ConfigLocator)
    - `AssetLocator` (src/Engine/Core/Locators/AssetLocator)
  - Flujo de inicio: `ConfigLocator::CreateConfigs()` → registrar componentes en `InitController` (InitController calcula orden por dependencias) → `ManagerLocator::InitializeManagers` / `ServiceLocator::InitializeServices` / `AssetLocator::InitializeAssets`.
  - Punto central de arranque: `Main.cpp` → `MainWindow` → `MainController` (ver `MainController.cpp`) que ejecuta la inicialización y el bucle Update/Render.

- Convenciones de registro y patrones de código:
  - Macro para registrar servicios: `REGISTER_SERVICE_TYPE(ServiceClass, "ServiceName")` (ver `src/Engine/Core/Locators/Registers/REGISTER_SERVICE_MACRO.h`).
  - Macro para registrar managers: `REGISTER_MANAGER_TYPE(ManagerClass, "ManagerName")` (ver `REGISTER_MANAGER_MACRO.h`).
  - Los registros añaden lambdas de creación/inicialización/render/update; las firmas esperadas aparecen en los archivos de locator (ej. `ServiceLocator::RegisterServiceCreator`).
  - Para obtener una instancia desde el código: `ServiceLocator::GetService<T>()` y `ManagerLocator::GetManager<T>()` o funciones helper como `ManagerLocator::GetDeviceManager()`.
  - Configs: las clases de configuración exponen `GetStaticConfigName()` y se obtienen vía `ConfigLocator::GetConfig<T>()`.

- Flujo de datos y dependencias:
  - Las dependencias se declaran en los Configs y son resueltas por `InitController` (detecta ciclos y emite errores con OutputDebugString / cerr).
  - Orden de update/render está controlado por vectores en los configs (`services_update_order`, `managers_update_order`, etc.) y usado por los locators.

- Compilación, ejecución y debugging (puntos prácticos):
  - Proyecto Visual Studio: `DirectX-Voxelado.sln` / `DirectX-Voxelado.vcxproj` (toolset v143). Compilar desde Visual Studio 2022: seleccionar plataforma x64/Debug o Release.
  - Versión de compilación requerida (declarada en instrucciones): `build_toolset_version: 23` (usar toolset/SDK acorde en el entorno de build).
  - Integración vcpkg: el proyecto usa `$(VcpkgRoot)` en include dirs y hay `vcpkg.json` en la raíz. Asegurar `VcpkgRoot` y que las libs (DirectXTK, yaml-cpp) estén disponibles.
  - Shaders ya están compilados (.cso) en la raíz; los fuentes HLSL están bajo `Assets/Shader/` o `Assets/` según sea necesario.
  - Configuración de compilación de shaders (script del repo): `Libs/DirectX/DirectXTex/Shaders/CompileShaders.cmd`.
    - El script usa `fxc.exe` (Legacy FXC) y compila con los perfiles de shader `cs_5_0` y `cs_4_0` (variantes), por lo que la pipeline actual espera estos modelos. Si se migra a `dxc` / HLSL 6.x, actualizar esta sección y el script correspondiente.
  - Scripts útiles incluidos en el proyecto: `copy_debug_to_root.bat` y `Assets/Textures/DefaultSkybox/build_dds.bat` (se referencian desde el .vcxproj). Úsalos desde PowerShell si necesitas copiar artefactos o generar DDS.

- Qué buscar cuando hagas cambios:
  - Si añades un nuevo Service/Manager/Config: crear la clase, exponer `GetStaticConfigName()` (para configs) y usar la macro `REGISTER_*_TYPE` para registrarla.
  - Para managers que requieren ventana, usan la lambda de inicialización que recibe `EngineContext*` o (en instancias) HWND,width,height (ver macros y `ManagerLocator::InitializeManagers`).
  - Evitar romper dependencias declaradas en los Configs; si cambias nombres, actualiza `*IndexConfig` y el YAML (si aplica).

- Archivos y directorios clave (ejemplos a consultar):
  - `MainController.cpp` — proceso de Init y orden de inicialización por dependencias.
  - `src/Engine/Core/Locators/*` — implementaciones de locators (Registro/Init/Get/Shutdown).
  - `src/Engine/Core/Locators/Registers/REGISTER_SERVICE_MACRO.h` y `REGISTER_MANAGER_MACRO.h` — macros de registro y firmas esperadas.
  - `DirectX-Voxelado.vcxproj` — configuración de build (Includes, librerías, plataformas, prebuilds).
  - `Assets/` y `bin/` — dónde están los recursos y artefactos finales.

- Reglas de estilo locales (descubribles):
  - Uso intensivo de smart pointers (`std::shared_ptr`) y dinámicas con `dynamic_pointer_cast` en locators.
  - Mensajes de debug con `OutputDebugStringA` o `std::cerr` en puntos críticos (InitController, MainController) — usar esto para traza al modificar inicialización.

Regla de orden de includes (convención)
-------------------------------------
Se recomienda agrupar y ordenar los includes para mejorar legibilidad y evitar diffs ruidosos.

- Orden esperado en cada fichero C/C++/H:
  1. Includes locales del proyecto entre comillas (`"..."`) — orden alfabético.
  2. Includes de sistema entre angulares (`<...>`) — orden alfabético.
  3. Separar los dos bloques con una línea en blanco.

- Excepción: archivos auto-generados (por ejemplo los de `YamlToStruct.exe` en `src/Engine/Core/Config`) no deben modificarse manualmente; no aplicar reordenación automática sobre ellos.

- Automatización: se recomienda usar `clang-format` para aplicar la convención de forma automática. Crear un `.clang-format` con `SortIncludes: true` y `IncludeBlocks: Regroup` aplicará reglas coherentes.

Ejemplo:

// Locales
"Engine/Rendering/RenderCommand.h"
"Managers/RenderManager/RenderManager.h"

// Sistema
<algorithm>
<vector>

Si alguna sección quedó incompleta o quieres que añada ejemplos concretos (por ejemplo: plantilla mínima para registrar un nuevo Service/Manager o fragmentos de YAML de config), dime cuál y lo añado ahora.

-- Objetivos del proyecto (útiles para agentes de IA):
  - Objetivo principal: construir un Framework 3D de alto nivel (estilo Unity) orientado a rendimiento.
  - Requisitos relevantes: máximo rendimiento, modularidad, toda la configuración definida en `.yaml` (fuente de verdad), diseño pensando en facilitar futura multiplataforma aunque el código actual dependa de DirectX11.

-- YAML como fuente de verdad (workflow descubierto):
  - Carpeta origen de la configuración YAML: `Resources/Config`.
  - Carpeta destino (generada): `src/Engine/Core/Config`.
  - Herramienta que transforma YAML → .h/.cpp: `YamlToStruct.exe` (incluida en el repo). Flujo típico:
    1. Borrar o limpiar `src/Engine/Core/Config` si quieres regenerar desde cero.
    2. Ejecutar `YamlToStruct.exe` apuntando a `Resources/Config` para regenerar las clases/implementaciones.
  - Nota sobre supuestos: el ejecutable es propietario y espera formatos YAML concretos; no lo ejecutamos aquí, pero documenta esto como paso canónico para regeneración de configs.

-- CRÍTICO - AUTO-GENERACIÓN DE CONFIGURACIONES (YamlToStruct):
==================================================================================
⚠️ **REGLA FUNDAMENTAL**: El directorio `C:\Users\joaqu\source\repos\DirectX-Voxelado\src\Engine\Core\Config` está **AUTO-GENERADO** por la herramienta `YamlToStruct.exe` y **NUNCA debe editarse directamente**.

### **Flujo obligatorio para cambios de configuración**:
1. **Editar archivos YAML** en `Resources/Config/` (fuente de verdad)
2. **Ejecutar YamlToStruct.exe** para regenerar `src/Engine/Core/Config/`
3. **Compilar** el proyecto para usar las nuevas configuraciones

### **Ejemplo de archivos auto-generados encontrados**:
- `src/Engine/Core/Config/Game/GeometryEngines/GeomtryChunkEngineConfig.h/.cpp`
- Estos contienen clases C++ generadas automáticamente desde YAML
- Incluyen macros de registro como `REGISTER_CONFIG_TYPE(GeomtryChunkEngineConfig,"GeomtryChunkEngineConfig")`

### **Nombres con typos encontrados que requieren corrección**:
Los siguientes elementos tienen el typo `GeomtryChunkEngine` (falta "e" en "Geometry"):

**Archivos de código (ya corregidos en clases):**
- `src/Game/Systems/Terrain/Engines/Geometry/Chunk/GeomtryChunkEngine.h` 
- `src/Game/Systems/Terrain/Engines/Geometry/Chunk/GeomtryChunkEngine.cpp`
- La clase `GeometryChunkEngine` ya fue corregida en el código

**Archivos auto-generados (requieren corrección en YAML):**
- `src/Engine/Core/Config/Game/GeometryEngines/GeomtryChunkEngineConfig.h`
- `src/Engine/Core/Config/Game/GeometryEngines/GeomtryChunkEngineConfig.cpp`
- Referencias en `md/Terrain/TÉCNICAS PARA EL TERRENO.md`

### **Acción requerida para corregir el typo**:
1. **Buscar en `Resources/Config/`** el archivo YAML que define `GeomtryChunkEngineConfig`
2. **Renombrar** `GeomtryChunkEngine` → `GeometryChunkEngine` en el YAML
3. **Ejecutar** `YamlToStruct.exe` para regenerar los configs C++
4. **Recompilar** para que la corrección sea efectiva

### **Implicaciones del typo**:
- Inconsistencia entre nombres de archivos físicos y clases C++
- Potenciales errores de resolución por nombre en factories/locators
- Confusión en documentación y referencias cruzadas

-- Recomendaciones prácticas para agentes:
  - Cuando modifiques un Config o introduzcas un nuevo tipo de config: actualiza primero el YAML en `Resources/Config`, luego ejecuta `YamlToStruct.exe` para regenerar `src/Engine/Core/Config` y compilar.
  - Para cambios en nombres de componentes (Service/Manager/Asset): revisar `*IndexConfig` y los vectors (`services_init`, `managers_init_order`, etc.) — `InitController` usa esas entradas para calcular el orden.
  - Mantener las macros `REGISTER_SERVICE_TYPE` / `REGISTER_MANAGER_TYPE` para el registro estático; preferir este patrón sobre registrar manualmente en runtime.

-- Ejemplo rápido (cómo regenerar configs — suponer PowerShell):
```powershell
# Limpiar carpeta de configs y regenerar (ejemplo conceptual)
Remove-Item -Recurse -Force .\src\Engine\Core\Config\*
.\YamlToStruct.exe --src .\Resources\Config --out .\src\Engine\Core\Config
```
Incluye variantes concretas si quieres que arme un script `regen_configs.ps1` en el repo.

 
 Mecánica de trabajo recomendada para refactorings (especialmente threading)
 ---------------------------------------------------------------------
 Para cambios grandes (p. ej. migración a modelo AAA de threading) seguir esta mecánica para minimizar riesgo y facilitar rollback:
 
 - Dividir el refactor en pasos cortos y numerados (Step 1, Step 2, ...). Cada paso debe ser pequeño y compilar limpio.
 - Después de aplicar cada paso:
   - Ejecutar la compilación del workspace (msbuild / Visual Studio). Si falla, revertir el cambio o arreglar inmediatamente.
   - Crear un commit con mensaje claro y estandarizado: `refactor/threading-step-<n>: <breve-descripción>`.
   - Empujar el commit si procede (remote branch). Esto permite volver a un punto seguro si algo sale mal.
 - Mantener un archivo de notas `md/Threading/ESTO ES LO QUE QUIERO.md` con la lista de pasos y el estado (pendiente/completado) para guiar el trabajo.
 - Cuando se introduzcan cambios invasivos (eliminación de clases, cambio de firmas públicas), añadir una etiqueta `BREAKING` en el mensaje del commit y documentar cómo volver al estado anterior.
 - Automatizar la verificación: antes de cada merge, ejecutar `run_build`/CI para asegurar que todos los pasos generan builds limpios.
 
 Regla práctica para la IA:
 - Nunca agrupar más de una intención arquitectónica en un único commit. Cada commit debe implementar y verificar una única idea.
 - Si un cambio requiere tocar múltiples subsistemas, crear una rama `refactor/threading` y trabajar por commits numerados.
 
 Aplicación inmediata: al migrar `UpdateManager` y `SceneManager` a sistemas sin hilo, implementar y compilar por pasos (por ejemplo: 1) convertir RunLoop a no-op, 2) llamar Update desde Engine::MainLoop, 3) eliminar ThreadedService y actualizar ManagerBase, 4) limpiar y documentar). Cada paso deberá tener su propio commit.

-- ARQUITECTURA DE TERRENO MODULAR (OBJETIVO PRINCIPAL DEL FRAMEWORK) --
=========================================================================

El objetivo principal es crear un framework 3D de alto nivel donde el usuario puede seleccionar, con mucho detalle y desde YAML, cómo generar el terreno. Las combinaciones que implementamos son:

### **Estrategias de Renderizado Implementadas**

**1) Tessellation GPU + Chunks + Ring Zones** 
   - **Engine**: `TessellationGeometryEngine`
   - **Distribución**: `TerrainChunkEngine` con ring buffering
   - **Características**: 3-4 drawcalls por frame, alta eficiencia GPU
   - **YAML config**: `geometry_engine: "TessellationGeometryEngine"`
   - **Descripción**: GPU genera la geometría final, chunks manejan la distribución espacial

**2) LOD Fijo + Agregados**
   - **Engine**: `AggregatedGeometryEngine` 
   - **Distribución**: `TerrainChunkEngine` con meshes pre-calculados
   - **Características**: Muchos drawcalls por frame, LOD fijo por distancia
   - **YAML config**: `geometry_engine: "AggregatedGeometryEngine"`
   - **Descripción**: CPU genera mallas densas, múltiples drawcalls por chunk

**3) LOD Dinámico + Tessellation GPU** (FUTURO)
   - **Engine**: `DynamicTessellationGeometryEngine` (por implementar)
   - **Distribución**: `TerrainChunkEngine` con LOD adaptativo
   - **Características**: Tessellation que se adapta en tiempo real según distancia/screen space
   - **YAML config**: `geometry_engine: "DynamicTessellationGeometryEngine"`

### **Configuraciones Futuras Soportadas por el Framework**

El diseño modular permite seleccionar desde YAML:

**Tipo de Mundo**:
- `mode: "2D" | "3D" | "Flat"`
- `terrain.enabled: true/false`
- `world_type: "finite" | "infinite"`

**Tipo de Terreno**:
- `terrain_type: "procedural" | "heightmap" | "mesh_static" | "voxel" | "dual_contouring"`
- `generation_source: "runtime" | "hdd_cache" | "streaming"`

**Generación Procedural**:
- `procedural_engine: "FastNoiseLite" | "DoubleLayerNoise" | "BiomeMapping"`
- Parámetros: `frequency`, `amplitude`, `octaves`, `lacunarity`, `gain`, `seed`

**Estructura Espacial**:
- `spatial_structure: "Grid" | "QuadTree" | "Octree" | "Clipmap"`
- `chunk_size`, `render_distance`, `lod_strategy`

**Buffering y Optimización**:
- `buffer_layout: "Simple" | "Ring" | "Triple" | "InstancingRingBuffer"`
- `buffer_zones`, `culling_strategy`

### **Patrón de Implementación para Engines**

Todos los engines de terreno siguen este patrón modular:

1. **Herencia de interfaces base**: `TerrainGeometryEngineBase`, `TerrainDistributionEngineBase`
2. **Registro con macros**: `REGISTER_TERRAIN_ENGINE_TYPE(EngineClass, "EngineYamlName")`
3. **Resolución por YAML**: Los strings en YAML se resuelven a implementaciones concretas
4. **Factory pattern**: `TerrainDistributionFactory`, `TerrainGeometryFactory` para crear instances
5. **Configuración central**: Todo configurable desde `TerrainConfig` generado de YAML

### **Flujo de Desarrollo de Nuevos Engines**

Para añadir una nueva estrategia de terreno:

1. **Definir en YAML**: Añadir entrada en `Resources/Config/Game/Terrain/*.yaml`
2. **Regenerar configs**: Ejecutar `YamlToStruct.exe` 
3. **Implementar engine**: Crear clase que herede de la interfaz base correspondiente
4. **Registrar engine**: Usar macro `REGISTER_TERRAIN_ENGINE_TYPE`
5. **Integrar en factory**: Añadir resolución por nombre en la factory correspondiente

### **Ejemplo de Configuración YAML Completa**

```yaml
terrain:
  enabled: true
  mode: "3D"
  world_type: "infinite"
  terrain_type: "procedural"
  
  # Estrategia de renderizado (selecciona una de las implementadas)
  geometry_engine: "TessellationGeometryEngine"  # o "AggregatedGeometryEngine"
  terrain_engine: "TerrainChunkEngine"
  
  # Generación procedural
  procedural_engine: "FastNoiseLite"
  generation_frequency: 0.01
  generation_amplitude: 100.0
  generation_octaves: 4
  seed: 12345
  
  # Estructura espacial
  spatial_structure: "Grid"
  chunk_size: 32
  render_distance: 8
  
  # LOD y optimización
  lod_strategy: "Distance"
  lod_thresholds: [64, 128, 256]
  buffer_layout: "Ring"
  buffer_zones: 3
```

Este enfoque permite que el usuario final tenga control granular sobre todos los aspectos del terreno sin tocar código, solo editando YAML y reiniciando la aplicación.

Generación de Terreno — interfaces y opciones (orientación para UI/YAML)
-----------------------------------------------------------------
El código debe orientarse a exponer interfaces y configuraciones que permitan al usuario final decidir, desde un UI o YAML, las opciones clave del terreno sin cambiar código:

- Decisiones de alto nivel que debe permitir la UI/YAML:
  - Modo: `2D` | `3D` | `Flat` (mundo sin altura).
  - ¿Usa terreno?: `terrain.enabled: true/false`.
  - Tipo de terreno: `procedural` | `static` | `voxel`.
  - Procedural engine: `FastNoiseLite` | `DoubleLayerNoise` | `BiomeMapping` | custom.
  - Estructura espacial de chunks: `Grid` | `QuadTree` | `Octree` | `Clipmap`.
  - Estrategia LOD: `Fixed` | `Distance` | `ScreenSpace` | `Manual`.
  - Buffering: `Simple` | `Ring` | `Triple`.

- Interfaces y clases recomendadas (nombres sugeridos):
  - `ITerrainConfig` / `TerrainConfig` (ya generado desde YAML): define keys anteriores.
  - `IProceduralEngine` (métodos Init(seed, def), GetHeight(x,z)).
  - `TerrainDistributionEngine` (interfaz): decide qué chunks existen y cuando cargar/unload.
  - `TerrainGeometryEngine` (interfaz): crea/contiene patch base y shaders (tessellation, meshlets, etc.).
  - `IChunk` (interfaz): SetProceduralEngine, Init(), Shutdown(), SetCurrentLOD(), GetFlags(), GetPosition().
  - `IChunkFactory` / `ChunkMeshFactory`: produce implementaciones concretas de `IChunk` según config.
  - `IChunkBufferManager` (ej. `InstancingRingBuffer`) para abstracción de uploads.

- Wiring y patrón de runtime:
  - Mantener los nombres de engine/chunk en YAML (ej. `terrain.procedural_engine: FastNoiseLite`) y resolverlos con locators (`TerrainEngineLocator`, `AssetLocator`) o factories (`TerrainDistributionFactory`).
  - Registrar implementaciones con las macros existentes (`REGISTER_SERVICE_TYPE` / `REGISTER_MANAGER_TYPE`) para que `ServiceLocator`/`ManagerLocator` puedan inicializarlos desde los configs.
  - Para cambios en tiempo de ejecución: soportar recarga de config (leer YAML, regenerar config structs con `YamlToStruct.exe` si cambió la forma) o implementar un runtime-reload que reinitialice servicios afectados.

- Ejemplo rápido de snippet YAML (alto nivel):
  terrain:
    enabled: true
    mode: "3D"
    type: "procedural"
    procedural_engine: "FastNoiseLite"
    geometry_engine: "TessellationGeometryEngine"
    spatial_structure: "Grid"
    chunk:
      size: 32
      initialRenderDistanceChunks: 4
    lod:
      strategy: "Distance"
      thresholds: [64, 128, 256]
    buffering: "InstancingRingBuffer"

- Notas prácticas:
  - UI debe editar estas claves YAML; el flujo recomendado es: editar YAML → (opcional) regenerar configs con `YamlToStruct.exe` → reiniciar/recargar subsistemas afectados.
  - Mantener la separación entre sampling (IProceduralEngine) y construcción de V/I (IChunk/IChunkFactory). El sampling puede ser concurrente; los uploads GPU deben centralizarse en el render thread o en un uploader sincronizado.
  - En el diseño, favorecer la resolución por nombre+locator (string keys) porque ya existe el patrón (`ConfigLocator`, `TerrainEngineLocator`) y facilita registrarlo desde macros.

Con esto tendrás la guía necesaria para que el código se diseñe desde el principio para exponer menús/checkboxes que permitan elegir 2D/3D, voxel/heightmap, motores de ruido, estructura espacial y buffering, todo controlable desde YAML. Si quieres, ahora creo plantillas de archivos (headers + cpp) para `TessellationChunk`, `AggregatedMeshChunk` e `InstancingRingBuffer` y un ejemplo YAML completo en `Resources/Config`.

Detalle: cómo se construye `TerrainConfig` (explicación de campos)
---------------------------------------------------------------
La clase generada `TerrainConfig` (ver `src/Engine/Core/Config/Game/System/TerrainConfig.h`) es la representación C++ de los YAML que se encuentran en `Resources/Config/Game/Terrain/`.
Abajo explico campo a campo (nombre, tipo y cómo influye en runtime):

- `name` (string)
  - Nombre identificador del config.
  - Uso: logging y decisiones nominativas; generalmente no obligatorio.

- `version` (string)
  - Versión del config/engine.
  - Uso: control de compatibilidad al cargar/switch de engines.

- `geometry_type` (int)
  - Código numérico que puede mapear a estrategias internas (ej. 0=mesh,1=tessellation,2=meshlets).
  - Uso: elegir la implementación concreta de `TerrainGeometryEngine`.

- `terrain_distribution` (int)
  - Código que identifica el engine de distribución (Grid, QuadTree, Octree, Clipmap).
  - Uso: resuelto por `TerrainDistributionFactory` / `TerrainEngineLocator`.

- `terrain_generator` (int)
  - Código que identifica el motor procedural (FastNoiseLite, DoubleLayer, BiomeMapping).
  - Uso: `ProceduralService` lo usará para crear el engine adecuado.

- `geometry_engine` (string)
  - Nombre del engine de geometría (p. ej. "TessellationGeometryEngine").
  - Uso: `TerrainGeometryFactory` o `TerrainEngineLocator` para obtener la implementación.

- `terrain_engine` (string)
  - Nombre del engine de distribución/procedural (p. ej. "TerrainChunkEngine").
  - Uso: `TerrainDistributionFactory::Init(config)` carga/initializa el engine.

- `procedural_engine` (int)
  - Redundante con `terrain_generator`; indica enum del engine procedural.

- `geometry_lod_strategy` (string)
  - Valores esperados: `Fixed`, `Distance`, `ScreenSpace`, `Manual`.
  - Uso: pasa a `LODManager::GetLOD()` para decidir LOD por chunk.

- `geometry_lod_thresholds` (vector<int>)
  - Umbrales en unidades de distancia para la estrategia `Distance`.
  - Uso: recorrer de mayor a menor para determinar LOD index.

- `geometry_lod_blending` (bool)
  - Si true, aplicar blending (smoothstep / morph) entre LODs para evitar popping.

- `generation_type` (string)
  - Ej.: `heightmap`, `voxel`, `procedural`.
  - Uso: define si se usa `ProceduralService` o assets estáticos.

- `generation_algorithm` (int)
  - Código interno para variantes (ej. 0=Perlin,1=DoubleLayer,2=BiomeMapping).

- `generation_frequency` (float)
- `generation_amplitude` (float)
- `generation_octaves` (int)
- `generation_lacunarity` (float)
- `generation_gain` (float)
  - Parámetros clásicos de ruido: frecuencia/amplitud/octavas/lacunarity/gain.
  - Uso: inicializan el engine procedural (`FastNoiseLiteEngine::Init(...)` y variantes).

- `geometry_resolution` (int)
  - Resolución base del parche/mesh (ej. número de vertices por lado) o índice de LOD máximo.
  - Uso: determina tamaño del V/I local o densidad del patch base para tessellation.

- `geometry_normals` (bool), `geometry_smooth` (bool)
  - Control de si se calculan normales y si se aplican suavizados en la malla.

- `spatial_structure` (string)
  - Valores: `Grid`, `QuadTree`, `Octree`, `Clipmap`.
  - Uso: define la estructura que administra chunks; `TerrainDistributionFactory` lo usa para instanciar el engine correcto.

- `quad_tree_max_depth` (int), `quad_tree_min_size` (int), `quad_tree_lod_strategy` (string)
  - Parámetros específicos para `QuadTree` si `spatial_structure` == `QuadTree`.

- `buffer_layout` (string), `buffer_zones` (int)
  - Define la estrategia de buffering para instancias/meshes: `InstancingRingBuffer`, `Triple`, `Simple`.
  - `buffer_zones` puede definir el número de sub-regiones del ring buffer o número de buffers.

Cómo se usa en runtime (flujo resumido)
- `ConfigLocator::CreateConfigs()` lee los YAML y crea instancias de `TerrainConfig` generadas.
- En `Terrain::Init()` se obtiene `TerrainConfig` con `ConfigLocator::GetConfig<TerrainConfig>()`.
- `Terrain::InitServices()` llama a `TerrainDistributionFactory::Init(m_config.get())` y `m_terrainFactory->Init(...)` resuelve `terrain_engine`/`geometry_engine` por nombre usando locators.
- `ProceduralService::Init({...})` usa los parámetros `generation_*` para inicializar el engine procedural.
- `ChunkService::Init(...)` obtiene `chunk_size`/`initialRenderDistanceChunks` desde `TerrainEngines` config y los aplica.

Consejos prácticos para editar el YAML de `TerrainConfig`:
- Cambios pequeños de parámetros (frecuencia, octaves) pueden probarse en caliente si implementas reload; para cambios estructurales (añadir campos) regenera `src/Engine/Core/Config` con `YamlToStruct.exe`.
- Para añadir un nuevo engine: añade el bloque en `Resources/Config/Game/Terrain/terrain_procedural.yaml` y una entrada en `terrain_engines.yaml`/`geometry_engines.yaml`, luego registra la clase concreta en `ProceduralService::Init` o crea un `TerrainEngineLocator` dinámico.

Si quieres, añado ahora un extracto automático del YAML real (`Resources/Config/Game/Terrain/*.yaml`) y lo pego como ejemplo literal dentro del fichero de instrucciones (útil para la UI). ¿Lo hago? 

Flujo de trabajo obligatorio para la IA al modificar la generación del terreno
-------------------------------------------------------------------------
Cuando la IA introduce cambios relacionados con la generación de terreno, debe seguir este flujo estricto (no opcional). Esto asegura que todas las propiedades configurables sigan siendo editables desde YAML y que el código generado/compilado refleje la configuración:

1. Confirmar el objetivo funcional
  - El usuario pide una funcionalidad nueva o un ajuste en la generación del terreno (por ejemplo: nuevo parámetro de ruido, nuevo engine procedural, opción de tessellation).

2. Decidir si requiere datos en YAML
  - Regla: cualquier propiedad que afecte comportamiento, parámetros o selección de engine debe estar en YAML (no hardcodear en código fuente).
  - Si la nueva funcionalidad necesita un parámetro persistente/configurable, añadir la clave correspondiente en el YAML adecuado (`Resources/Config/Game/Terrain/*.yaml` o `Resources/Config/...`).

3. Añadir la entrada al YAML
  - Localización típica: `Resources/Config/Game/Terrain/terrain_procedural.yaml`, `terrain_engines.yaml` o `geometry_engines.yaml` según corresponda.
  - Usar nomenclatura consistente (por ejemplo `procedural_engine: "FastNoiseLite"` o agregar nuevos campos bajo `Config: Game: Terrain: Procedural:`).

4. Ejecutar `YamlToStruct.exe`
  - Después de editar los YAML, ejecutar la herramienta `YamlToStruct.exe` para regenerar los `.h`/`.cpp` en `src/Engine/Core/Config/`.
  - Ejemplo (PowerShell):
```powershell
Remove-Item -Recurse -Force .\src\Engine\Core\Config\*
.\YamlToStruct.exe --src .\Resources\Config --out .\src\Engine\Core\Config
```

5. Compilar el proyecto principal
  - Abrir la solución `DirectX-Voxelado.sln` y compilar (o usar MSBuild) para que las nuevas structs y getters estén disponibles.

6. Modificar/añadir clases en `src/Game/Systems/Terrain`
  - Añadir nuevas clases (por ejemplo `TessellationChunk`, `AQTMDistanceLODEngine`, `InstancingRingBuffer`) en `src/Game/Systems/Terrain` o subcarpetas (`Chunk`, `Engines`, `Factory`), respetando la organización del repo.
  - NO introducir nuevas propiedades configurables dentro de clases sin haberlas definido primero en YAML y regenerado `TerrainConfig` (o el config correspondiente).

7. Leer valores desde el config en runtime
  - En `Init()` o `InitServices()` usar `ConfigLocator::GetConfig<TerrainConfig>()` o el config específico para obtener las nuevas propiedades y alimentar la lógica interna.
  - Ejemplo: `auto config = ConfigLocator::GetConfig<TerrainConfig>(); m_chunkSize = config->chunk_size;`

8. Testing y PR
  - Ejecutar la aplicación localmente para validar que el nuevo parámetro tiene efecto.
  - Documentar en el PR el cambio en YAML y el motivo.

Notas importantes para la IA
 - Nunca hardcodees parámetros que vayan a ser útiles para tuning; siempre proponer un campo YAML y seguir el flujo anterior.
 - Si la modificación es sólo un experimento rápido (POC), indícalo explícitamente en el PR y crea el campo YAML temporalmente para evitar que el cambio se pierda.
 - Mantén la backward compatibility: si añades campos nuevos, marca valores por defecto sensatos y documenta en el YAML/README.

Regla obligatoria (shaders y assets)
----------------------------------
Todas las definiciones y registros de shaders y assets deben existir primero en YAML bajo `Resources/Config/Assets/` y en los índices (`assets_base.yaml` / `assets_shader.yaml`). No compilar ni crear shaders directamente desde engines o módulos de render en código fuente.

Proceso requerido para shaders nuevos:
  - Añadir la entrada YAML en `Resources/Config/Assets/assets_shader.yaml` (ej. `TerrainTessellation` con `shader_path`, `vertex_def`, `matrix_slots`, `sampler_slots`).
  - Ejecutar `YamlToStruct.exe` si se han cambiado las estructuras de configuración para regenerar los tipos C++.
  - Asegurar que `assets_base.yaml` / `MainIndex` incluye el asset para carga automática, o añadirlo al índice.
  - En runtime, obtener el shader con `AssetLocator::GetShaderAsset("Name")` o forzar su carga con `ShaderManager::LoadShaderByName(L"Name")`.
  - Evitar llamadas a `D3DCompileFromFile` o compilación directa en engines; `ShaderManager` es el responsable central de compilar y almacenar blobs/objetos de shader.

Si se necesita un POC rápido, documentalo en el PR y añade el bloque YAML provisional; la versión final debe migrarse a la pipeline YAML/ShaderManager antes de fusionar.

Este flujo debe quedar documentado y ser la guía que cualquier agente de IA siga antes de editar código relacionado con la generación de terreno.

--- ESTADO ACTUAL DEL PROYECTO (ACTUALIZADO AUTOMÁTICAMENTE) ---
================================================================

## Trabajo completado recientemente:
- ✅ **TerrainAsset limpiado**: Simplificado para manejar solo shaders, materiales y pases de renderizado (siguiendo filosofía C++ de "tener solo lo que se usa")
- ✅ **Errores de compilación arreglados**: 12 errores de linkado resueltos
- ✅ **Instancias estáticas creadas**: `s_tessEngine` y `s_aggEngine` definidos
- ✅ **Implementaciones básicas de IChunk**: `AggregatedMeshChunk` y `TessellationChunk` con métodos temporales
- ✅ **BoundingBox reutilizada**: Usando definición existente de `Util::RayTracing::RayTracing.h`
- ✅ **Jobs de terreno identificados**: `UpdateTerrainJob` y `UpdateMeshTerrainJob` en `SceneManager`

## Trabajo completado:
✅ **Conexiones entre Jobs y Engines completadas** - 3 pasos completados:

### **Paso 1**: COMPLETADO ✅
- TerrainDistributionFactory::UpdateTerrain/Generate ya implementados
- TerrainChunkEngine::UpdateTerrain/Generate ya implementados

### **Paso 2**: COMPLETADO ✅
- ✅ **CREADO**: `TerrainGeometryEngineBase.h` (interfaz base para geometry engines)
- ✅ **ACTUALIZADO**: `TessellationGeometryEngine` hereda de la base e implementa `GenerateMesh()`
- ✅ **ACTUALIZADO**: `AggregatedGeometryEngine` hereda de la base e implementa `GenerateMesh()`
- ✅ **CONECTADO**: `TerrainChunkEngine` selecciona dinámicamente geometry engine basado en config YAML

### **Paso 3**: COMPLETADO ✅
- ✅ Implementada lógica específica en cada engine (tessellation vs aggregated mesh)
- ✅ Chunks se crean correctamente via factories
- ✅ **COMPILACIÓN EXITOSA**: 0 errores, solo warnings menores

## Archivos clave modificados recientemente:
- `src/Engine/Assets/Base/TerrainAsset.h/.cpp` - Limpiado y simplificado
- `src/Game/Systems/Terrain/Engines/Geometry/TessellationGeometryEngine.cpp` - Instancia estática añadida
- `src/Game/Systems/Terrain/Engines/Geometry/AggregatedGeometryEngine.cpp` - Instancia estática añadida
- `src/Game/Systems/Terrain/Engines/Chunk/AggregatedMeshChunk.h/.cpp` - Implementaciones básicas
- `src/Game/Systems/Terrain/Engines/Chunk/TessellationChunk.h/.cpp` - Implementaciones básicas
- `Interfaces/IChunk.h` - GetCenter() ahora tiene implementación por defecto

## Flujo de datos completo:
```
SceneManager::Update() 
→ UpdateTerrainJob/UpdateMeshTerrainJob 
→ Terrain::Update()/Generate() 
→ TerrainDistributionFactory::UpdateTerrain()/Generate() 
→ TerrainChunkEngine::UpdateTerrain()/Generate() 
→ [CONECTADO] → GeometryEngines (TessellationGeometryEngine | AggregatedGeometryEngine)
```

## Próximo paso (cerrando ciclo completo):
**IMPLEMENTAR CICLO COMPLETO UPDATE → RENDER**:
1. Completar la lógica de generación de meshes en geometry engines
2. Asegurar que chunks cargan/descargan correctamente con ring buffering
3. Implementar el renderizado específico por engine (tessellation vs meshes agregados)
4. Testear cambio dinámico de engine via YAML config

## Comando especial para IA:
**Cuando el usuario diga: "Me ha salido lo de que se acaba la conversación"**
→ Actualizar automáticamente esta sección con el estado exacto donde se quedó el trabajo.

-- ARQUITECTURA THREADING AAA - NUEVO OBJETIVO PRIORITARIO --
============================================================

### **CRÍTICO: Refactoring Threading Model a Estándar AAA**

**PROBLEMA IDENTIFICADO**: El modelo de threading actual no cumple estándares de frameworks AAA. Mezcla threads dedicados innecesarios con ThreadPool sin justificación clara.

**ARQUITECTURA ACTUAL (PROBLEMÁTICA)**:
```
MainWindow (hilo principal)
├── Cálculo de deltaTime  
├── Manejo de mensajes Windows
└── Engine::InitThreads()
    ├── SceneManager::Start() [Thread dedicado ❌]
    ├── UpdateManager::Start() [Thread dedicado ❌] 
    └── RenderManager::Start() [Thread dedicado ❌]
    
+ ThreadPool para jobs asíncronos
= 4 + N threads (EXCESIVO)
```

**ARQUITECTURA OBJETIVO (ESTÁNDAR AAA)**:
```
┌─────────────────┐    ┌──────────────────┐    ┌─────────────────┐
│   Main Thread   │────▶│   Render Thread  │    │   ThreadPool    │
│                 │    │                  │    │                 │
│ • Input         │    │ • CommandLists   │    │ • Terrain Jobs  │
│ • Timing        │    │ • GPU Submit     │    │ • Asset Loading │
│ • Game Logic    │    │ • Present        │    │ • Physics       │
│ • Scene Update  │    │                  │    │ • Audio         │
└─────────────────┘    └──────────────────┘    └─────────────────┘

= 2 threads principales + ThreadPool workers (EFICIENTE)
```

### **CAMBIOS OBLIGATORIOS PARA THREADING AAA**

**1. CONSOLIDAR MAIN THREAD**:
- ❌ **ELIMINAR**: UpdateManager thread dedicado
- ❌ **ELIMINAR**: SceneManager thread dedicado  
- ✅ **MOVER**: Toda la lógica de update al main thread
- ✅ **MANTENER**: Solo RenderThread separado

**2. RENDERTHREAD PURO**:
- ✅ **SOLO**: Ejecutar command lists y present
- ✅ **RECIBIR**: Comandos del main thread via producer-consumer
- ✅ **SINCRONIZAR**: Con main thread via condition variables

**3. THREADPOOL ESPECIALIZADO**:
- ✅ **USAR PARA**: Terrain chunk generation, Asset streaming, Physics, Audio
- ❌ **NO USAR PARA**: Update logic básico (debe ir en main thread)

### **PATRONES DE DISEÑO ADICIONALES REQUERIDOS**

**COMMAND PATTERN** (Crítico para render pipeline):
```cpp
class IRenderCommand {
    virtual void Execute(ID3D11DeviceContext* context) = 0;
    virtual uint32_t GetSortKey() const = 0;  // Para batching
};

class CommandBuffer {
    std::vector<std::unique_ptr<IRenderCommand>> m_commands;
    void ExecuteAll(ID3D11DeviceContext* context);
    void SortForOptimalExecution();  // Minimiza state changes
};
```

**FLYWEIGHT PATTERN** (Para chunk data):
```cpp
class ChunkIntrinsicState {  // Datos compartidos entre chunks
    std::shared_ptr<TerrainShader> shader;
    std::shared_ptr<MaterialData> material;
    std::array<MeshTemplate, MAX_LODS> lodMeshTemplates;
};

class ChunkExtrinsicState {  // Datos únicos por chunk
    TerrainChunk::ChunkID chunkId;
    DirectX::XMFLOAT3 worldPosition;
    const ChunkIntrinsicState* intrinsicState;
};
```

**VISITOR PATTERN** (Para algoritmos de terreno):
```cpp
class ITerrainVisitor {
    virtual void Visit(AggregatedMeshChunk& chunk) = 0;
    virtual void Visit(TessellationChunk& chunk) = 0;
};

class LODUpdateVisitor : public ITerrainVisitor {
    // Algoritmo específico para cada tipo de chunk
};
```

**OBJECT POOL PATTERN** (Para performance):
```cpp
template<typename TChunk>
class ChunkPool {
    std::queue<std::unique_ptr<TChunk>> m_availableChunks;
    std::unique_ptr<TChunk> Acquire(TerrainChunk::ChunkID id);
    void Release(std::unique_ptr<TChunk> chunk);
};
```

### **FLUJO DE REFACTORING OBLIGATORIO**

**Paso 1: Preparar Threading Infrastructure**
```cpp
class Engine {
    std::unique_ptr<std::thread> m_renderThread;      // Solo render thread
    std::queue<RenderCommandPacket> m_renderQueue;    // Producer-Consumer
    std::mutex m_renderQueueMutex;
    std::condition_variable m_renderCondition;
    
    void MainLoop();      // Game logic en main thread
    void RenderLoop();    // Solo renderizado en thread separado
};
```

**Paso 2: Convertir Managers en Systems**
```cpp
// ❌ ELIMINAR: UpdateManager como ThreadedService
// ✅ CREAR: UpdateSystem sin thread propio
class UpdateSystem {  // No hereda de ThreadedService
    void Update(float deltaTime);           // Llamado desde MainLoop
    void SubmitAsyncJob(UpdateJob job);     // Para jobs pesados
};
```

**Paso 3: Implementar Command Buffer System**
```cpp
struct RenderCommandPacket {
    int frameId;
    std::vector<RenderCommand> commands;
    std::chrono::high_resolution_clock::time_point submitTime;
};

void Engine::SubmitRenderCommands() {
    // Recopilar comandos de todos los sistemas
    RenderCommandPacket framePacket;
    framePacket.commands = m_sceneManager->GetRenderCommands();
    m_renderQueue.push(std::move(framePacket));
    m_renderCondition.notify_one();
}
```

### **BENEFICIOS ESPERADOS DEL REFACTORING**

**Performance**:
- ✅ Reducción de threads: De 4+N a 2+N
- ✅ Mejor CPU cache: Menos context switching
- ✅ Command batching: Minimiza GPU state changes

**Arquitectura**:
- ✅ Sincronización clara: Producer-Consumer explícito
- ✅ Compatible AAA: Patrón usado por Unreal, CryEngine
- ✅ Mejor debugging: Game logic en main thread

**Escalabilidad**:
- ✅ ThreadPool maneja trabajo paralelo eficientemente
- ✅ Flyweight reduce memory footprint masivamente
- ✅ Object pools eliminan allocation overhead

### **PRIORIDAD DE IMPLEMENTACIÓN**

**FASE 1 (CRÍTICA)**: Threading Infrastructure
1. Crear nuevo MainLoop consolidado
2. Implementar RenderThread puro con command buffer
3. Migrar UpdateManager/SceneManager a sistemas sin threads

**FASE 2 (ALTA)**: Design Patterns
1. Implementar Command Pattern para render pipeline
2. Añadir Flyweight Pattern para chunk data
3. Crear Object Pool para chunk management

**FASE 3 (MEDIA)**: Advanced Patterns
1. Visitor Pattern para algoritmos de terreno
2. Strategy Pattern con hot-swapping
3. Composite Pattern para scene hierarchy

**REGLA FUNDAMENTAL**: Todo nuevo código relacionado con threading debe seguir el modelo AAA (Main + Render + ThreadPool). No crear nuevos threads dedicados sin justificación arquitectónica sólida.

### **CONFIGURACIÓN YAML PARA THREADING**
```yaml
Threading:
  main_thread:
    target_fps: 60
    frame_limit_enabled: true
    
  render_thread:
    priority: "time_critical"
    affinity_mask: 2  # CPU core 1
    
  thread_pool:
    worker_count: -1  # hardware_concurrency() - 2
    terrain_workers: 4
    asset_workers: 2
    
  frame_sync:
    max_frames_ahead: 2
    vsync_enabled: true
```

Este refactoring es **OBLIGATORIO** para cumplir estándares AAA y debe implementarse antes de añadir nuevas features significativas al framework.

## Historial de commits y puntos de retorno

Mantener un historial centralizado de commits importantes facilita rollback y trazabilidad. Se ha añadido un fichero dedicado en `.github/commit_history.md` donde se registran los commits relevantes (hash, mensaje breve y descripción).

Regla rápida:
- El asistente actualizará `.github/commit_history.md` después de cada cambio relevante. Si añades commits manualmente, añade también la entrada correspondiente en ese fichero.

Ver `.github/commit_history.md` para el historial actual y para las instrucciones sobre cómo anotar nuevos commits.
