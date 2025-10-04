# Integración del Generador YAML ? C++

## Contexto

Tienes un proyecto separado que convierte archivos .yaml en archivos .h y .cpp que necesita ejecutarse antes de compilar el motor DirectX 12.

## Opciones de Integración

### OPCIÓN 1: Proyecto en la Solución (RECOMENDADO)

#### Ventajas
? **Integración completa con Visual Studio**  
? **Control total del orden de compilación**  
? **Debugging del generador**  
? **IntelliSense para el código del generador**  
? **Gestión unificada de dependencias**

#### Estructura de Solución Propuesta
```
DirectX-Voxelado.sln
??? DirectX-Voxelado          # Proyecto principal (motor DX12)
??? YAMLGenerator             # Proyecto generador YAML?C++
??? YAMLGenerator.Tests       # Tests del generador (opcional)
```

#### Pre-Build Events
En el proyecto principal, agregar Pre-Build Event:
```bash
# En Project Properties ? Build Events ? Pre-Build Event
$(SolutionDir)x64\$(Configuration)\YAMLGenerator.exe "$(ProjectDir)Resources\Config" "$(ProjectDir)generated"
```

#### Configuración de Dependencias
```xml
<!-- En DirectX-Voxelado.vcxproj -->
<ItemGroup>
  <ProjectReference Include="..\YAMLGenerator\YAMLGenerator.vcxproj">
    <Project>{GUID-DEL-GENERADOR}</Project>
    <LinkLibraryDependencies>false</LinkLibraryDependencies>
    <UseLibraryDependencyInputs>false</UseLibraryDependencyInputs>
  </ProjectReference>
</ItemGroup>
```

### OPCIÓN 2: Ejecutable Externo con MSBuild

#### Custom Target en .vcxproj
```xml
<!-- Agregar al final del .vcxproj del motor -->
<Target Name="GenerateFromYAML" BeforeTargets="ClCompile">
  <Message Text="Generando código desde YAML..." Importance="high" />
  
  <Exec Command="&quot;$(SolutionDir)Tools\YAMLGenerator.exe&quot; &quot;$(ProjectDir)Resources\Config&quot; &quot;$(ProjectDir)generated&quot;"
        ContinueOnError="false"
        WorkingDirectory="$(ProjectDir)" />
        
  <Message Text="Generación YAML completada." Importance="high" />
</Target>

<!-- Incluir archivos generados automáticamente -->
<Target Name="IncludeGeneratedFiles" BeforeTargets="ClCompile">
  <ItemGroup>
    <ClInclude Include="generated\Configs\**\*.h" />
    <ClCompile Include="generated\Configs\**\*.cpp" />
  </ItemGroup>
</Target>
```

## Implementación del "Troyano" Automático

### Enfoque Moderno: Runtime Code Generation

```cpp
// Core/CodeGeneration/YAMLProcessor.h
#pragma once

#include "Core/Types.h"

namespace Engine::CodeGen {
    class YAMLProcessor {
    public:
        // Detectar cambios en archivos YAML
        static bool NeedsRegeneration(const StringView& configPath);
        
        // Ejecutar generador y recompilar
        static Expected<bool> RegenerateAndReload();
        
        // Hot reload de configuraciones
        static Expected<bool> HotReloadConfigs();
        
    private:
        static String GetGeneratorPath();
        static Expected<bool> ExecuteGenerator(const String& inputPath, const String& outputPath);
        static bool CheckFileTimestamps();
    };
}
```

### Implementación del Auto-Regenerador

```cpp
// Core/CodeGeneration/YAMLProcessor.cpp
#include "YAMLProcessor.h"
#include <filesystem>
#include <process.h>

namespace Engine::CodeGen {
    bool YAMLProcessor::NeedsRegeneration(const StringView& configPath) {
        namespace fs = std::filesystem;
        
        const auto yamlPath = fs::path(configPath);
        const auto generatedPath = fs::path("generated");
        
        if (!fs::exists(generatedPath)) return true;
        
        // Comparar timestamps de archivos YAML vs generados
        auto yamlTime = fs::last_write_time(yamlPath);
        
        for (const auto& entry : fs::recursive_directory_iterator(generatedPath)) {
            if (entry.path().extension() == ".h" || entry.path().extension() == ".cpp") {
                auto genTime = fs::last_write_time(entry);
                if (yamlTime > genTime) return true;
            }
        }
        
        return false;
    }
    
    Expected<bool> YAMLProcessor::RegenerateAndReload() {
        // 1. Ejecutar generador
        auto result = ExecuteGenerator("Resources/Config", "generated");
        if (!result.has_value()) {
            return std::unexpected(result.error());
        }
        
        // 2. Recargar configuraciones en runtime (si es posible)
        return HotReloadConfigs();
    }
    
    Expected<bool> YAMLProcessor::ExecuteGenerator(const String& inputPath, const String& outputPath) {
        String command = std::format(
            "\"{}\" \"{}\" \"{}\"", 
            GetGeneratorPath(), 
            inputPath, 
            outputPath
        );
        
        int result = std::system(command.c_str());
        
        if (result != 0) {
            return std::unexpected(std::error_code(result, std::system_category()));
        }
        
        return true;
    }
    
    String YAMLProcessor::GetGeneratorPath() {
        #ifdef _DEBUG
            return "Tools/YAMLGenerator_Debug.exe";
        #else
            return "Tools/YAMLGenerator_Release.exe";
        #endif
    }
}
```

### Integración en el Motor Principal

```cpp
// Engine/Engine.cpp - Modificar Init()
bool Engine::Init(EngineContext* context) {
    m_context = context;
    OutputDebugStringA("[Engine] Comenzando la inicialización.\n");
    
    // NUEVO: Verificar y regenerar configuraciones si es necesario
    if (CodeGen::YAMLProcessor::NeedsRegeneration("Resources/Config")) {
        OutputDebugStringA("[Engine] Detectados cambios en YAML. Regenerando...\n");
        
        auto result = CodeGen::YAMLProcessor::RegenerateAndReload();
        if (!result.has_value()) {
            OutputDebugStringA("[Engine] ERROR: Falló la regeneración de YAML.\n");
            return false;
        }
        
        OutputDebugStringA("[Engine] Regeneración completada.\n");
    }

    // Continuar con inicialización normal...
    if (!InitEngine(context)) {
        OutputDebugStringA("[Engine] Engine - ERROR.\n");
        return false;
    }
    
    // ... resto del código
}
```

## Sistema de Hot Reload Avanzado

### File Watcher para Desarrollo
```cpp
// Development/FileWatcher.h
#pragma once

#include "Core/Types.h"
#include <functional>

namespace Engine::Dev {
    class FileWatcher {
    public:
        using ChangeCallback = std::function<void(const String& filePath)>;
        
        FileWatcher(const String& watchPath, ChangeCallback callback);
        ~FileWatcher();
        
        void StartWatching();
        void StopWatching();
        
    private:
        String m_watchPath;
        ChangeCallback m_callback;
        HANDLE m_directoryHandle;
        bool m_isWatching = false;
        
        void WatchThread();
    };
}
```

### Uso en Desarrollo
```cpp
// En modo Debug, establecer watcher automático
#ifdef ENGINE_DEBUG
    auto fileWatcher = std::make_unique<FileWatcher>(
        "Resources/Config",
        [this](const String& changedFile) {
            OutputDebugStringA(std::format("[Engine] Archivo YAML modificado: {}\n", changedFile).c_str());
            
            // Regenerar automáticamente en desarrollo
            CodeGen::YAMLProcessor::RegenerateAndReload();
        }
    );
    
    fileWatcher->StartWatching();
#endif
```

## Recomendación Final

### Para Desarrollo: OPCIÓN 1 + Hot Reload
- **Proyecto YAMLGenerator en la solución**
- **Pre-Build Events automáticos**
- **File Watcher para desarrollo**
- **Hot Reload de configuraciones**

### Para Distribución: OPCIÓN 2
- **Generador como herramienta externa**
- **MSBuild targets**
- **Sin dependencias de desarrollo**

### Beneficios del Enfoque Híbrido
? **Desarrollo fluido**: Cambios en YAML se reflejan automáticamente  
? **Control total**: El motor puede regenerar cuando sea necesario  
? **Distribución limpia**: Sin dependencias innecesarias en release  
? **Debugging**: Puedes debuggear tanto el motor como el generador  

¿Quieres que implementemos alguna de estas opciones específicamente?
