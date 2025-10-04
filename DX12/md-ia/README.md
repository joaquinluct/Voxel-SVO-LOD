# Documentación IA - Contexto y Memoria del Proyecto

## PREMISA CERO: DOCUMENTACIÓN OBLIGATORIA

**CADA CAMBIO QUE SE HAGA DEBE DOCUMENTARSE EN este archivo (md-ia/README.md).**

## PREMISAS FUNDAMENTALES

### 1. MIGRAR DX11 A DX12
Objetivo principal del proyecto: migración completa de DirectX 11 a DirectX 12

- **Proyecto origen de la migración:** `C:/Users/joaqu/source/repos/DirectX-Voxelado/DirectX-Voxelado.vcxproj`
- **Proyecto destino de la migración:** `C:/Users/joaqu/source/repos/DirectX-Voxelado/DX12/DirectX-Voxelado-DX12.vcxproj`

### 2. TODOS LOS DATOS SIEMPRE EN CONFIGURACIÓN .YAML
Enlace: [CONFIGURACIÓN-YAML.md](./CONFIGURACION-YAML.md)

### 3. APRENDER Y ENTENDER EL SISTEMA DE CONFIGURACIÓN YAML
**SIEMPRE considerar el sistema YAML al crear o modificar datos/configuraciones**
- Nunca hardcodear valores en el código fuente
- Crear nuevas configuraciones en archivos .yaml apropiados
- Usar tipos nativos soportados (int, bool, float, string, vector, map)
- Definir correctamente struct/class, herencia e implementación de interfaces
- Regenerar las clases Config automáticamente tras cambios en YAML
- Acceder a configuraciones a través de ConfigLocator en lugar de valores directos

### 4. FACTORY PATTERN PARA CONFIGURACIONES
**ConfigLocator funciona como Factory: GetConfig<TipoConfig>("NombreInstancia")**
- Separación entre tipo de configuración y instancia específica
- Múltiples instancias del mismo tipo: `ResourcesConfig("MeshAssetConfig")`, `ResourcesConfig("TextureAssetConfig")`
- Configuración granular total: color de fondo, algoritmos Perlin, carga de recursos, pasa de renderizado, shaders
- TODO absolutamente configurable vía YAML sin recompilar
- Pattern: `YAML ? Generador ? ConfigClasses ? ConfigLocator Factory ? Runtime Access`

### 5. SISTEMA DE LOCATORS (SERVICE LOCATOR PATTERN)
Enlace: [SISTEMA-LOCATORS.md](./SISTEMA-LOCATORS.md)

### 6. APRENDER Y ENTENDER EL SISTEMA DE LOCATORS
**SIEMPRE tener en cuenta el sistema de Locators al pensar en crear o modificar código**
- Usar los patrones establecidos de registro automático
- Respetar las interfaces base (IService, ManagerBase, IConfig)
- Implementar las interfaces funcionales apropiadas (IInitializable, IRenderable, IUpdatable)
- Registrar nuevas clases usando las macros correspondientes
- Acceder a dependencias a través de los Locators en lugar de instanciación directa

### 7. ORGANIZACIÓN PROFESIONAL C++23
Enlace: [ORGANIZACION-CPP23.md](./ORGANIZACION-CPP23.md)

### 8. CUMPLIR ESTRICTAMENTE CON LA ESTRUCTURA ORGANIZACION-CPP23.md
**SIEMPRE seguir la estructura de proyecto definida en ORGANIZACION-CPP23.md**
- Respetar la jerarquía de directorios establecida (`Core/`, `Graphics/`, `Engine/`, etc.")
- Mantener la separación clara entre Core (independiente de API), Graphics (DX12), y Engine (alto nivel)
- Seguir las convenciones de naming y organización de archivos documentadas
- Aplicar las mejores prácticas de arquitectura C++23 profesional establecidas
- No desviarse de la estructura documentada sin justificación explícita y actualización de documentación
- Usar los namespaces, tipos, y patrones definidos (`Engine::`, `Engine::Graphics::DX12::`)

### 9. ORDENACIÓN ESTRICTA DE INCLUDES
**SIEMPRE mantener los includes ordenados según las mejores prácticas de C++ profesional**
- **Orden establecido**: (1) PCH, (2) Header propio, (3) C++ estándar, (4) Platform, (5) Third party, (6) Motor interno
- **Alfabético dentro de cada grupo**: Ordenar alfabéticamente dentro de cada categoría
- **Separación con líneas en blanco**: Separar cada grupo con una línea en blanco
- **Evitar dependencias ocultas**: El orden ayuda a detectar problemas de dependencias
- **Consistencia**: Todos los archivos deben seguir el mismo patrón de ordenación
- **Ejemplo de orden correcto**:
```cpp
// 1. PCH (si se usa)
#include "precompiled/EnginePCH.h"

// 2. Header propio (para .cpp)
#include "MainWindow.h"

// 3. C++ estándar (alfabético)
#include <algorithm>
#include <memory>
#include <string>
#include <vector>

// 4. Platform específico (alfabético)
#ifdef _WIN32
    #include <windows.h>
    #include <d3d12.h>
#endif

// 5. Third party (alfabético)
#include <DirectXMath.h>

// 6. Motor interno (alfabético por categoría)
#include "Core/Foundation/Types.h"
#include "Core/Macros.h"
#include "Engine/Engine.h"
```

### 10. VERIFICACIÓN DE INCLUDES EXISTENTES
**SIEMPRE verificar que los includes añadidos existan y estén correctamente formados**
- **Verificar existencia**: Comprobar que el archivo referenciado existe en el sistema de archivos
- **Validar ruta relativa**: Asegurar que la ruta es correcta respecto al directorio de trabajo del proyecto
- **Comprobar estructura**: Verificar que el include sigue la estructura de directorios establecida
- **Evitar includes circulares**: No crear dependencias circulares entre headers
- **Forward declarations**: Usar forward declarations cuando sea posible en lugar de includes completos
- **Verificar configuración actual**: Respetar las rutas de include configuradas en el proyecto
- **Ejemplo de verificación**:
```cpp
// ? INCORRECTO - archivo puede no existir
#include "Managers/NonExistentManager.h" 

// ? CORRECTO - verificar antes de usar
// 1. Comprobar que existe: DX12/src/Engine/Managers/RenderManager.h
// 2. Usar ruta correcta según configuración del proyecto
#include "Engine/Managers/RenderManager.h"

// ? EVITAR - dependencia circular
// Header A incluye Header B, Header B incluye Header A

// ? PREFERIR - forward declaration cuando sea posible
class RenderManager; // En lugar de #include "Engine/Managers/RenderManager.h"
```

### 11. PROTECCIÓN ABSOLUTA DEL PROYECTO ORIGEN DX11
**JAMÁS modificar, mover, borrar o alterar archivos en el proyecto origen de la migración**
- **SOLO LECTURA**: El proyecto origen (`DirectX-Voxelado.vcxproj`) es DE SOLO LECTURA
- **PROHIBIDO modificar**: Nunca cambiar contenido de archivos del proyecto origen
- **PROHIBIDO mover**: Nunca mover archivos entre carpetas del proyecto origen
- **PROHIBIDO borrar**: Nunca eliminar archivos del proyecto origen
- **PERMITIDO leer**: Sí se puede leer y copiar contenido parcial o completo para referencia
- **PERMITIDO copiar**: Sí se puede copiar archivos completos hacia `/DX12/` y modificar las copias
- **SEPARACIÓN TOTAL**: Proyecto origen y destino deben ser completamente independientes
- **ENFOQUE**: Trabajar ÚNICAMENTE en `/DX12/` y resolver problemas dentro de ese ecosistema
- **Ejemplo de violación**:
```cpp
// ? PROHIBIDO - modificar archivo del proyecto origen
// Editar src/Main/MainWindow.h para cambiar includes

// ? CORRECTO - trabajar en DX12
// Editar DX12/src/Main/MainWindow.h y resolver includes dentro de DX12
```

## PROGRESO ACTUAL DE LA MIGRACIÓN

### ? COMPLETADO

#### ?? Estructura Base DX12 - REORGANIZADA COMPLETAMENTE
- **Estructura completa**: Siguiendo estrictamente ORGANIZACION-CPP23.md
- **Directorios creados**: 
  - `/DX12/Core/` (Foundation, Memory, Containers, Threading, Reflection, Locators, Context)
  - `/DX12/Graphics/DX12/` (Device, Resources, Pipeline, Commands, Descriptors, Sync, Debug)
  - `/DX12/Engine/` (Managers, Components, Systems, World)
  - `/DX12/Math/`, `/DX12/Platform/`, `/DX12/Utils/`, `/DX12/Main/`
  - `/DX12/include/` (Core, Graphics, Engine)
  - `/DX12/interfaces/` (Contratos puros)
  - `/DX12/generated/` (Configs, Defines, Assets)
  - `/DX12/precompiled/` (PCH)
  - `/DX12/Resources/` (Archivos YAML y assets)
- **Nota importante**: No debe existir un directorio `src` replicando la estructura dentro de `/DX12/`. Toda la estructura válida debe colgar directamente de `/DX12/`.
- **Organización C++23**: Tipos modernos, Expected<T,E>, concepts preparados
- **Documentación**: Sistema completo documentado en `/DX12/md-ia/`

#### ??? MainWindow (NUEVO - DX12)
- **Archivo**: `DX12/Main/MainWindow.h` y `MainWindow.cpp`
- **Características**:
  - ? C++23 moderno con `Expected<T,E>` para error handling
  - ? Configuración YAML: `main_window_config.yaml`
  - ? Factory pattern: `MainWindowConfig` + `ConfigLocator`
  - ? Separación de responsabilidades (ventana, timing, motor)
  - ? Namespace `Engine::` apropiado
  - ? Logging estructurado con `OutputDebugStringA`
  - ? Gestión de recursos RAII

#### ?? Core Systems (NUEVO - DX12)
- **Types.h**: Tipos fundamentales C++23 (`Engine::uint32`, `Engine::f32`, etc.) en `/DX12/Core/Foundation/`
- **Macros.h**: Macros de utilidad (`ENGINE_NON_COPYABLE`, `ENGINE_ASSERT`, etc.) en `/DX12/Core/Foundation/`
- **EngineContext.h**: Contexto del motor con información compartida en `/DX12/Core/Context/`
- **Engine.h/cpp**: Clase principal del motor (stub preparado para expansión) en `/DX12/Engine/`

#### ?? Configuración YAML - REORGANIZADA
- **Ubicación**: `/DX12/Resources/Config/` (movido desde src/Yaml)
- **Archivo principal**: `main_window_config.yaml`
- **Índice**: `index_config.yaml` (actualizado con nuevas rutas)
- **Configuraciones**: MainWindow, DebugWindow
- **Propiedades**: fullscreen, vsync, debug settings, timing, input
- **Output**: Apunta a `/DX12/generated/Configs/Application/`

### ?? EN DESARROLLO

#### ?? Motor Engine (DX12)
- **Estado**: Estructura básica creada, falta implementación
- **Pendiente**: Integración con Managers, DX12 device creation
- **Next**: Implementar DeviceManager DX12 en `/DX12/Engine/Managers/`

#### ?? Sistema de Locators DX12
- **Estado**: Copiado desde DX11 en `/DX12/Core/Locators/`
- **Pendiente**: Integrar con nueva arquitectura C++23
- **Next**: Adaptar ConfigLocator para MainWindowConfig

### ?? PRÓXIMOS PASOS PLANIFICADOS

1. **Limpiar solución**: Borrar x64, limpiar solución, cerrar Visual Studio, reabrir y recompilar
2. **Ocultar proyecto origen**: Excluir del proyecto el archivo `DirectX-Voxelado.vcxproj` para evitar conflictos
3. **Generar MainWindowConfig**: Ejecutar YamlToStruct sobre `/DX12/Resources/Config/main_window_config.yaml`
4. **Integrar ConfigLocator**: Cargar configuración en MainWindow::LoadConfiguration()
5. **DeviceManager DX12**: Crear gestión de dispositivo DirectX 12 en `/DX12/Engine/Managers/`

## ESTRUCTURA DE DIRECTORIOS - FINAL

### Proyecto Origen (DirectX 11) - PROTEGIDO - SOLO LECTURA
- `DirectX-Voxelado.vcxproj` - **[PROTEGIDO]**

### Proyecto Destino (DirectX 12) - REORGANIZADO COMPLETAMENTE
```
/DX12/                          # Raíz del proyecto DX12
??? Core/                       # Núcleo independiente de API
?   ??? Foundation/             # Types.h, Macros.h
?   ??? Memory/                 # Gestión de memoria
?   ??? Containers/             # Contenedores optimizados
?   ??? Threading/              # Primitivas de threading
?   ??? Reflection/             # Sistema de reflexión
?   ??? Context/                # EngineContext.h
?   ??? Locators/               # Service Locator pattern
??? Graphics/                   # Sistemas gráficos específicos
?   ??? DX12/                   # DirectX 12 implementation
?   ?   ??? Device/             # Device, Factory, Adapter, SwapChain
?   ?   ??? Resources/          # Buffer, Texture, RenderTarget, DepthStencil
?   ?   ??? Pipeline/           # PSO, Root Signatures, Shaders
?   ?   ??? Commands/           # Command Lists, Queues, Allocators
?   ?   ??? Descriptors/        # Heaps de descriptores
?   ?   ??? Synchronization/    # Fence, Timing, FrameSync
?   ?   ??? Debug/              # Debug Layer, PIX, Validation
??? Engine/                     # Lógica del motor de alto nivel
?   ??? Managers/               # Managers del motor
?   ??? Components/             # Sistema ECS
?   ??? Systems/                # Sistemas del ECS
?   ??? World/                  # Mundo y escenas
??? Math/                       # Matemáticas y utilidades
??? Platform/                   # Código específico de plataforma
??? Utils/                      # Utilidades generales
??? Main/                       # Punto de entrada (MainWindow)
??? include/                    # Headers públicos
?   ??? Core/
?   ??? Graphics/
?   ??? Engine/
??? interfaces/                 # Interfaces puras (contratos)
??? generated/                  # Código generado desde YAML
?   ??? Configs/
?   ??? Defines/
?   ??? Assets/
??? precompiled/                # PCH (Precompiled Headers)
??? Resources/                  # Recursos y configuraciones
?   ??? Config/                 # Archivos YAML
??? md/                         # Documentación del proyecto
??? md-ia/                      # Documentación IA y contexto
```

### Directorio de Configuraciones
- `/DX12/Resources/Config/` - Archivos YAML para DX12 (MOVIDO desde src/Yaml)
- `/DX12/generated/` - Código C++ generado desde YAML

## Información del Proyecto

### Contexto General
- **Proyecto**: DirectX-Voxelado - Motor de renderizado de vóxeles
- **Migración**: De DirectX 11 a DirectX 12
- **Arquitectura nueva**: C++23 con Expected<T,E>, configuración YAML total, estructura profesional

### Estado Actual del Proyecto
- **DX11**: Motor funcional con access violation en `UpdateMeshTerrainJob::Execute` **[PROTEGIDO - NO TOCAR]**
- **DX12**: Estructura completa reorganizada, MainWindow funcional, motor en desarrollo
- **Estrategia**: "Borrón y cuenta nueva" con arquitectura moderna C++23, separación total DX11/DX12

### Componentes Principales Identificados
```
Engine (Principal DX12) - REORGANIZADO
??? MainWindow (? COMPLETADO) ? /DX12/Main/
??? DeviceManager (?? Pendiente) ? /DX12/Engine/Managers/
??? RenderManager (?? Pendiente) ? /DX12/Engine/Managers/
??? UpdateManager (?? Pendiente) ? /DX12/Engine/Managers/
??? SceneManager (?? Pendiente) ? /DX12/Engine/Managers/
??? InputManager (?? Pendiente) ? /DX12/Engine/Managers/
??? ThreadPool (?? Pendiente) ? /DX12/Core/Threading/
```

### Sistema YamlToStruct
- **Estado**: Funcionando 100% desde DX11
- **Integración**: Preparado para generar MainWindowConfig
- **Ubicación YAML**: `/DX12/Resources/Config/` (ACTUALIZADA)
- **Output**: `/DX12/generated/Configs/` (ACTUALIZADA)
- **Workflow**: YAML ? YamlToStruct.exe ? .h/.cpp ? ConfigLocator

## Reglas de Trabajo Establecidas

1. **Documentación continua**: Actualizar estos archivos con cada avance significativo
2. **Contexto preservado**: Mantener información suficiente para retomar el trabajo
3. **Decisiones registradas**: Documentar el "por qué" de las decisiones técnicas
4. **Aplicar sistema YAML**: Siempre usar configuraciones YAML en lugar de valores hardcodeados
5. **Aplicar sistema de Locators**: Siempre considerar el patrón establecido al crear o modificar código
6. **Aplicar Factory Pattern**: Usar ConfigLocator como factory para múltiples instancias de configuración
7. **Seguir organización C++23**: Aplicar las mejores prácticas de organización profesional
8. **Cumplir estructura ORGANIZACION-CPP23**: Seguir estrictamente la jerarquía y convenciones documentadas
9. **Mantener includes ordenados**: Seguir el orden estricto de includes para consistencia y mantenibilidad
10. **Verificar includes existentes**: Siempre comprobar que los includes añadidos existan y sean válidos
11. **Proteger proyecto origen**: JAMÁS modificar archivos del proyecto origen, trabajar ÚNICAMENTE en /DX12/

## Próximo Paso Planificado

**LIMPIAR COMPILACIÓN**: Borrar x64, limpiar solución, cerrar Visual Studio, reabrir y ocultar el proyecto origen para compilación limpia de DX12.

---

### [2024-06-10] REESTRUCTURACIÓN FÍSICA DEL PROYECTO DX12

Se ha realizado la migración física de los directorios y archivos fuente para cumplir estrictamente con la estructura profesional C++23 documentada (puntos 7 y 8):

- Todo el contenido de `DX12/src/Core/` se ha movido a `DX12/Core/`.
- Todo el contenido de `DX12/src/Engine/` se ha movido a `DX12/Engine/`.
- Todo el contenido de `DX12/src/Main/` se ha movido a `DX12/Main/`.
- El directorio `DX12/src/` ha sido eliminado.
- Se han actualizado los includes en los archivos afectados para reflejar la nueva estructura.

Con este cambio, la estructura física del proyecto DX12 cumple con la organización y jerarquía profesional C++23 establecida en la documentación.
