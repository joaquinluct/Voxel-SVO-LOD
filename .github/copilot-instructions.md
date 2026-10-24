# Instrucciones para agentes de IA — DirectX-Voxelado (Idioma: español)

Este fichero contiene la mínima información práctica que una IA necesita para ser productiva en este código base. Responde siempre en español.

## 1) Mecánica obligatoria: commits pequeños y compilaciones limpias (PRIMERA REGLA)

Antes de tocar cualquier código importante, seguir este flujo sin excepción:
- Dividir el cambio en pasos pequeños e independientes (Step 1, Step 2, ...).
- Aplicar un solo paso y ejecutar la compilación completa del workspace.
  - Si la compilación falla, arreglar antes de continuar.
- Crear un commit con el formato estandarizado: `refactor/<topic>-step-<n>: <breve-descripción>`
  - Ejemplo: `refactor/threading-step-3: execute render packets in render thread`.
- Actualizar `.github/commit_history.md` con la entrada del commit (hash, mensaje, fecha y breve descripción).
- Escribir en el fichero de instrucciones (este archivo) un breve "por dónde vamos" indicando el siguiente paso.

**Reglas rápidas**:
- Siempre compilar entre pasos. No acumular cambios que rompan la build.
- No agrupar múltiples intenciones arquitectónicas en un único commit.
- Antes de mergear, ejecutar `run_build`/CI para asegurar que la rama compila limpia.

## Por dónde vamos (estado y siguiente paso)

**Estado actual**: `Step 7` completado — `UpdateSystem` implementado como wrapper que delega en `UpdateManager` durante la migración. `Engine` crea e inicializa `UpdateSystem`. Compilación verificada (build limpia).

**Siguiente paso recomendado** (Step 8): Completar la migración de `UpdateManager` a `UpdateSystem`:
- Extraer lógica de update del manager al system
- Procesar futures dentro del system
- Exponer API de jobs limpia
- Commit sugerido: `refactor/threading-step-8: migrate UpdateManager logic into UpdateSystem`

**Notas prácticas**:
- Mantener commits pequeños y compilar entre pasos.
- Actualizar `.github/commit_history.md` tras cada commit relevante.
- Aplicar cambio pequeño → compilar workspace completo → crear commit con mensaje estandarizado → actualizar historial.

## Arquitectura general (panorama)

- Motor modular con tres tipos principales de componentes: Managers, Services y Assets.
- Localizadores (Singleton registries) orquestan creación/obtención:
  - `ManagerLocator` (src/Engine/Core/Locators/ManagerLocator)
  - `ServiceLocator` (src/Engine/Core/Locators/ServiceLocator)
  - `ConfigLocator` (src/Engine/Core/Locators/ConfigLocator)
  - `AssetLocator` (src/Engine/Core/Locators/AssetLocator)
- Flujo de inicio: `ConfigLocator::CreateConfigs()` → registrar componentes en `InitController` (InitController calcula orden por dependencias) → `ManagerLocator::InitializeManagers` / `ServiceLocator::InitializeServices` / `AssetLocator::InitializeAssets`.
- Punto central de arranque: `Main.cpp` → `MainWindow` → `MainController` (ver `MainController.cpp`) que ejecuta la inicialización y el bucle Update/Render.

## Convenciones de registro y patrones de código

- Macro para registrar servicios: `REGISTER_SERVICE_TYPE(ServiceClass, "ServiceName")` (ver `src/Engine/Core/Locators/Registers/REGISTER_SERVICE_MACRO.h`).
- Macro para registrar managers: `REGISTER_MANAGER_TYPE(ManagerClass, "ManagerName")` (ver `REGISTER_MANAGER_MACRO.h`).
- Los registros añaden lambdas de creación/inicialización/render/update; las firmas esperadas aparecen en los archivos de locator (ej. `ServiceLocator::RegisterServiceCreator`).
- Para obtener una instancia desde el código: `ServiceLocator::GetService<T>()` y `ManagerLocator::GetManager<T>()` o funciones helper como `ManagerLocator::GetDeviceManager()`.
- Configs: las clases de configuración exponen `GetStaticConfigName()` y se obtienen vía `ConfigLocator::GetConfig<T>()`.

## Flujo de datos y dependencias

- Las dependencias se declaran en los Configs y son resueltas por `InitController` (detecta ciclos y emite errores con OutputDebugString / cerr).
- Orden de update/render está controlado por vectores en los configs (`services_update_order`, `managers_update_order`, etc.) y usado por los locators.

## Compilación, ejecución y debugging (puntos prácticos)

- Proyecto Visual Studio: `DirectX-Voxelado.sln` / `DirectX-Voxelado.vcxproj` (toolset v143). Compilar desde Visual Studio 2022: seleccionar plataforma x64/Debug o Release.
- Versión de compilación requerida (declarada en instrucciones): `build_toolset_version: 23` (usar toolset/SDK acorde en el entorno de build).
- Integración vcpkg: el proyecto usa `$(VcpkgRoot)` en include dirs y hay `vcpkg.json` en la raíz. Asegurar `VcpkgRoot` y que las libs (DirectXTK, yaml-cpp) estén disponibles.
- Shaders ya están compilados (.cso) en la raíz; los fuentes HLSL están bajo `Assets/Shader/` o `Assets/` según sea necesario.
- Configuración de compilación de shaders (script del repo): `Libs/DirectX/DirectXTex/Shaders/CompileShaders.cmd`.
  - El script usa `fxc.exe` (Legacy FXC) y compila con los perfiles de shader `cs_5_0` y `cs_4_0` (variantes), por lo que la pipeline actual espera estos modelos. Si se migra a `dxc` / HLSL 6.x, actualizar esta sección y el script correspondiente.
- Scripts útiles incluidos en el proyecto: `copy_debug_to_root.bat` y `Assets/Textures/DefaultSkybox/build_dds.bat` (se referencian desde el .vcxproj). Úsalos desde PowerShell si necesitas copiar artefactos o generar DDS.

## Qué buscar cuando hagas cambios

- Si añades un nuevo Service/Manager/Config: crear la clase, exponer `GetStaticConfigName()` (para configs) y usar la macro `REGISTER_*_TYPE` para registrarla.
- Para managers que requieren ventana, usan la lambda de inicialización que recibe `EngineContext*` o (en instancias) HWND,width,height (ver macros y `ManagerLocator::InitializeManagers`).
- Evitar romper dependencias declaradas en los Configs; si cambias nombres, actualiza `*IndexConfig` y el YAML (si aplica).

## Archivos y directorios clave (ejemplos a consultar)

- `MainController.cpp` — proceso de Init y orden de inicialización por dependencias.
- `src/Engine/Core/Locators/*` — implementaciones de locators (Registro/Init/Get/Shutdown).
- `src/Engine/Core/Locators/Registers/REGISTER_SERVICE_MACRO.h` y `REGISTER_MANAGER_MACRO.h` — macros de registro y firmas esperadas.
- `DirectX-Voxelado.vcxproj` — configuración de build (Includes, librerías, plataformas, prebuilds).
- `Assets/` y `bin/` — dónde están los recursos y artefactos finales.

## Reglas de estilo locales (descubribles)

- Uso intensivo de smart pointers (`std::shared_ptr`) y dinámicas con `dynamic_pointer_cast` en locators.
- Mensajes de debug con `OutputDebugStringA` o `std::cerr` en puntos críticos (InitController, MainController) — usar esto para traza al modificar inicialización.

## Regla de orden de includes (convención)

Se recomienda agrupar y ordenar los includes para mejorar legibilidad y evitar diffs ruidosos.

**Orden esperado en cada fichero C/C++/H**:
1. Includes locales del proyecto entre comillas (`"..."`) — orden alfabético.
2. Includes de sistema entre angulares (`<...>`) — orden alfabético.
3. Separar los dos bloques con una línea en blanco.

**Excepción**: archivos auto-generados (por ejemplo los de `YamlToStruct.exe` en `src/Engine/Core/Config`) no deben modificarse manualmente; no aplicar reordenación automática sobre ellos.

**Automatización**: se recomienda usar `clang-format` para aplicar la convención de forma automática. Crear un `.clang-format` con `SortIncludes: true` y `IncludeBlocks: Regroup` aplicará reglas coherentes.

**Ejemplo**:
```cpp
// Locales
#include "Engine/Rendering/RenderCommand.h"
#include "Managers/RenderManager/RenderManager.h"

// Sistema
#include <algorithm>
#include <vector>
```

## Objetivos del proyecto (útiles para agentes de IA)

- Objetivo principal: construir un Framework 3D de alto nivel (estilo Unity) orientado a rendimiento.
- Requisitos relevantes: máximo rendimiento, modularidad, toda la configuración definida en `.yaml` (fuente de verdad), diseño pensando en facilitar futura multiplataforma aunque el código actual dependa de DirectX11.

## YAML como fuente de verdad (workflow descubierto)

- Carpeta origen de la configuración YAML: `Resources/Config`.
- Carpeta destino (generada): `src/Engine/Core/Config`.
- Herramienta que transforma YAML → .h/.cpp: `YamlToStruct.exe` (incluida en el repo). 

**Flujo típico**:
1. Borrar o limpiar `src/Engine/Core/Config` si quieres regenerar desde cero.
2. Ejecutar `YamlToStruct.exe` apuntando a `Resources/Config` para regenerar las clases/implementaciones.

**Nota sobre supuestos**: el ejecutable es propietario y espera formatos YAML concretos; no lo ejecutamos aquí, pero documenta esto como paso canónico para regeneración de configs.

## CRÍTICO - AUTO-GENERACIÓN DE CONFIGURACIONES (YamlToStruct)

⚠️ **REGLA FUNDAMENTAL**: El directorio `src\Engine\Core\Config` está **AUTO-GENERADO** por la herramienta `YamlToStruct.exe` y **NUNCA debe editarse directamente**.

### Flujo obligatorio para cambios de configuración:
1. **Editar archivos YAML** en `Resources/Config/` (fuente de verdad)
2. **Ejecutar YamlToStruct.exe** para regenerar `src/Engine/Core/Config/`
3. **Compilar** el proyecto para usar las nuevas configuraciones

### Ejemplo de archivos auto-generados encontrados:
- `src/Engine/Core/Config/Game/GeometryEngines/GeomtryChunkEngineConfig.h/.cpp`
- Estos contienen clases C++ generadas automáticamente desde YAML
- Incluyen macros de registro como `REGISTER_CONFIG_TYPE(GeomtryChunkEngineConfig,"GeomtryChunkEngineConfig")`

### Ejemplo rápido (cómo regenerar configs — PowerShell):
```powershell
# Limpiar carpeta de configs y regenerar
Remove-Item -Recurse -Force .\src\Engine\Core\Config\*
.\YamlToStruct.exe --src .\Resources\Config --out .\src\Engine\Core\Config
```

## Recomendaciones prácticas para agentes

- Cuando modifiques un Config o introduzcas un nuevo tipo de config: actualiza primero el YAML en `Resources/Config`, luego ejecuta `YamlToStruct.exe` para regenerar `src/Engine/Core/Config` y compilar.
- Para cambios en nombres de componentes (Service/Manager/Asset): revisar `*IndexConfig` y los vectors (`services_init`, `managers_init_order`, etc.) — `InitController` usa esas entradas para calcular el orden.
- Mantener las macros `REGISTER_SERVICE_TYPE` / `REGISTER_MANAGER_TYPE` para el registro estático; preferir este patrón sobre registrar manualmente en runtime.

## Mejora opcional para UIRenderer: renderizado por carácter (per-character quads)

- **Enfoque**: en lugar de usar meshes preconstruidas, el `UIRenderer` puede generar quads por carácter en tiempo de render (cada carácter -> 6 vértices) y subirlos a un VB dinámico.
- **Ventajas**: batching eficiente, control de kerning/espaciado, SDF text support, posibilidad de atlas único para toda la UI.

## Mecánica de trabajo recomendada para refactorings (especialmente threading)

Para cambios grandes (p. ej. migración a modelo AAA de threading) seguir esta mecánica para minimizar riesgo y facilitar rollback:

- Dividir el refactor en pasos cortos y numerados (Step 1, Step 2, ...). Cada paso debe ser pequeño y compilar limpio.
- Después de aplicar cada paso:
  - Ejecutar la compilación del workspace (msbuild / Visual Studio). Si falla, revertir el cambio o arreglar inmediatamente.
  - Crear un commit con mensaje claro y estandarizado: `refactor/threading-step-<n>: <breve-descripción>`.
  - Empujar el commit si procede (remote branch). Esto permite volver a un punto seguro si algo sale mal.
- Mantener un archivo de notas `md/Threading/ESTO ES LO QUE QUIERO.md` con la lista de pasos y el estado (pendiente/completado) para guiar el trabajo.
- Cuando se introduzcan cambios invasivos (eliminación de clases, cambio de firmas públicas), añadir una etiqueta `BREAKING` en el mensaje del commit y documentar cómo volver al estado anterior.
- Automatizar la verificación: antes de cada merge, ejecutar `run_build`/CI para asegurar que todos los pasos generan builds limpios.

**Regla práctica para la IA**:
- Nunca agrupar más de una intención arquitectónica en un único commit. Cada commit debe implementar y verificar una única idea.
- Si un cambio requiere tocar múltiples subsistemas, crear una rama `refactor/threading` y trabajar por commits numerados.

## ARQUITECTURA DE TERRENO MODULAR (OBJETIVO PRINCIPAL DEL FRAMEWORK)

El objetivo principal es crear un framework 3D de alto nivel donde el usuario puede seleccionar, con mucho detalle y desde YAML, cómo generar el terreno.

### Estrategias de Renderizado Implementadas

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

### Configuraciones Futuras Soportadas por el Framework

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

### Ejemplo de Configuración YAML Completa

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

## Flujo de trabajo obligatorio para la IA al modificar la generación del terreno

Cuando la IA introduce cambios relacionados con la generación de terreno, debe seguir este flujo estricto (no opcional). Esto asegura que todas las propiedades configurables sigan siendo editables desde YAML y que el código generado/compilado refleje la configuración.

### Regla obligatoria (shaders y assets)

Todas las definiciones y registros de shaders y assets deben existir primero en YAML bajo `Resources/Config/Assets/` y en los índices (`assets_base.yaml` / `assets_shader.yaml`). No compilar ni crear shaders directamente desde engines o módulos de render en código fuente.

## ESTADO ACTUAL DEL PROYECTO (ACTUALIZADO AUTOMÁTICAMENTE)

### Trabajo completado recientemente:
- ✅ **TerrainAsset limpiado**: Simplificado para manejar solo shaders, materiales y pases de renderizado
- ✅ **Errores de compilación arreglados**: 12 errores de linkado resueltos
- ✅ **Instancias estáticas creadas**: `s_tessEngine` y `s_aggEngine` definidos
- ✅ **Implementaciones básicas de IChunk**: `AggregatedMeshChunk` y `TessellationChunk` con métodos temporales
- ✅ **BoundingBox reutilizada**: Usando definición existente de `Util::RayTracing::RayTracing.h`
- ✅ **Jobs de terreno identificados**: `UpdateTerrainJob` y `UpdateMeshTerrainJob` en `SceneManager`

### Trabajo completado:
✅ **Conexiones entre Jobs y Engines completadas** - 3 pasos completados

**Paso 1**: COMPLETADO ✅
- TerrainDistributionFactory::UpdateTerrain/Generate ya implementados
- TerrainChunkEngine::UpdateTerrain/Generate ya implementados

**Paso 2**: COMPLETADO ✅
- ✅ **CREADO**: `TerrainGeometryEngineBase.h` (interfaz base para geometry engines)
- ✅ **ACTUALIZADO**: `TessellationGeometryEngine` hereda de la base e implementa `GenerateMesh()`
- ✅ **ACTUALIZADO**: `AggregatedGeometryEngine` hereda de la base e implementa `GenerateMesh()`
- ✅ **CONECTADO**: `TerrainChunkEngine` selecciona dinámicamente geometry engine basado en config YAML

**Paso 3**: COMPLETADO ✅
- ✅ Implementada lógica específica en cada engine (tessellation vs aggregated mesh)
- ✅ Chunks se crean correctamente via factories
- ✅ **COMPILACIÓN EXITOSA**: 0 errores, solo warnings menores

### Flujo de datos completo:
```
SceneManager::Update() 
→ UpdateTerrainJob/UpdateMeshTerrainJob 
→ Terrain::Update()/Generate() 
→ TerrainDistributionFactory::UpdateTerrain()/Generate() 
→ TerrainChunkEngine::UpdateTerrain()/Generate() 
→ [CONECTADO] → GeometryEngines (TessellationGeometryEngine | AggregatedGeometryEngine)
```

### Próximo paso (cerrando ciclo completo):
**IMPLEMENTAR CICLO COMPLETO UPDATE → RENDER**:
1. Completar la lógica de generación de meshes en geometry engines
2. Asegurar que chunks cargan/descargan correctamente con ring buffering
3. Implementar el renderizado específico por engine (tessellation vs meshes agregados)
4. Testear cambio dinámico de engine via YAML config

### Comando especial para IA:
**Cuando el usuario diga: "Me ha salido lo de que se acaba la conversación"**
→ Actualizar automáticamente esta sección con el estado exacto donde se quedó el trabajo.

## ARQUITECTURA THREADING AAA - NUEVO OBJETIVO PRIORITARIO

### CRÍTICO: Refactoring Threading Model a Estándar AAA

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

### CAMBIOS OBLIGATORIOS PARA THREADING AAA

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

### CONFIGURACIÓN YAML PARA THREADING

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

**REGLA FUNDAMENTAL**: Todo nuevo código relacionado con threading debe seguir el modelo AAA (Main + Render + ThreadPool). No crear nuevos threads dedicados sin justificación arquitectónica sólida.

## Historial de commits y puntos de retorno

Mantener un historial centralizado de commits importantes facilita rollback y trazabilidad. Se ha añadido un fichero dedicado en `.github/commit_history.md` donde se registran los commits relevantes (hash, mensaje breve y descripción).

**Regla rápida**:
- El asistente actualizará `.github/commit_history.md` después de cada cambio relevante. Si añades commits manualmente, añade también la entrada correspondiente en ese fichero.

Ver `.github/commit_history.md` para el historial actual y para las instrucciones sobre cómo anotar nuevos commits.
