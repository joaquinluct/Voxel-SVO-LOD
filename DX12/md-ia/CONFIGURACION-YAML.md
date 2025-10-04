# Sistema de Configuración YAML

## Propósito

Documentación completa del sistema **YamlToStruct** que convierte archivos YAML en clases C++ con registro automático en ConfigLocator.

## Arquitectura del Sistema YamlToStruct

### Flujo de Trabajo
```
index_config.yaml ? BatchProcessor ? Archivos YAML ? YamlToStruct.exe ? Clases C++ (.h/.cpp)
```

### Estructura del Index Config
```yaml
# DX12/src/Yaml/Config/index_config.yaml
BatchProcessor:
    description: 'DirectX-Voxel project config files'
    workDirectory: 'C:\Users\joaqu\source\repos\DirectX-Voxelado\Resources\Config'
    files:
      - Assets\assets_shader.yaml
      - engine_config.yaml
      - services_config.yaml
      # ... más archivos
```

## Estructura de Archivos YAML

### Sección Config (Datos)
Define la jerarquía de configuraciones con propiedades específicas:

```yaml
Config:
  Asset:
    Shader:
      TerrainShader:
        type: "ShaderAsset"
        shader_path: "C:\\...\\TerrainShader.hlsl"
        vertex_def: "TextureMapVertex"
        matrix_slots: [MatrixBufferType, DirectionalLight, CameraData]
        sampler_slots: [baseSampler, ShadowMapSampler]
        needs_shadow: true
        dependencies: [ShaderAsset]
```

### Sección Definition (Metadatos)
Define cómo generar las clases C++:

```yaml
Definition:
  Default:
    language: 1                    # C++
    type: "Class"                 # Tipo de generación
    needs_register: true          # Auto-registro en Locators
    file_level: 3                # Nivel de jerarquía para nombres
    register_include: "REGISTER_CONFIG_TYPE"
    register_macro: "REGISTER_CONFIG_MACRO"
    interface_name: "IAssetShaderConfig"
    class_suffix: "Config"        # Sufijo de clase
    class_prefix: ""              # Prefijo de clase
    inherits_properties: [type, shader_path, vertex_def, ...]
    output_path: 'c:\...\Config\Assets\'
```

## Tipos de Configuraciones Soportadas

### 1. Engine Managers (engine_config.yaml)
```yaml
Config:
  Base:
    Managers:
      Engine:
        managers_init_order: [DeviceManager, InputManager, RenderManager]
        managers_render_order: [UIManager, DeviceManager]
        pipeline_states_init: [SwapChainMain, RasterizedMainColorPass]
      
      DeviceManager:
        vsync: true
        fullscreen: false
        debug_device: true
        swap_chain_buffer_count: 2
        dependencies: [None]
```

### 2. Services (services_config.yaml)
```yaml
Config:
  Services:
    Service:
      services_init: [Keyboard, Mouse, Material, ThreadPool]
      
    Keyboard:
      enabled: true
      dependencies: [KeyBindings]
      
    ChunkService:
      enabled: true
      dependencies: [CameraManager]
```

### 3. Assets (assets_*.yaml)
```yaml
Config:
  Asset:
    Shader:
      PBRShader:
        type: "ShaderAsset"
        shader_path: "C:\\...\\PBRShader.hlsl"
        vertex_def: "TextureMapVertex"
        matrix_slots: [MatrixBufferType, DirectionalLight, CameraData]
        sampler_slots: [baseSampler]
        needs_shadow: false
        name: "PBRShader"
        dependencies: [ShaderAsset]
```

## Generación Automática de C++

### Patrón de Naming Generated
Basado en `file_level` y `class_suffix`:

- **file_level: 3** + Config/Asset/Shader/TerrainShader ? **TerrainShaderConfig**
- **file_level: 2** + Config/Services/Keyboard ? **KeyboardConfig**
- **file_level: 3** + Config/Base/Managers/DeviceManager ? **DeviceManagerConfig**

### Clases Generadas
```cpp
// Generado automáticamente desde YAML
class TerrainShaderConfig : public IAssetShaderConfig {
public:
    std::string type = "ShaderAsset";
    std::string shader_path = "C:\\...\\TerrainShader.hlsl";
    std::string vertex_def = "TextureMapVertex";
    std::vector<std::string> matrix_slots;
    std::vector<std::string> sampler_slots;
    bool needs_shadow = true;
    std::string name = "TerrainShader";
    std::vector<std::string> dependencies;
    
    static std::string GetStaticConfigName() { return "TerrainShader"; }
    // ... métodos automáticos
};

// Auto-registro generado
REGISTER_CONFIG_TYPE(TerrainShaderConfig, "TerrainShader")
```

## Factory Pattern con ConfigLocator

### Acceso Multi-Instancia
```cpp
// Una clase puede tener múltiples configuraciones
auto terrainShader = ConfigLocator::GetConfig<TerrainShaderConfig>("TerrainShader");
auto pbrShader = ConfigLocator::GetConfig<TerrainShaderConfig>("PBRShader");  
auto shadowShader = ConfigLocator::GetConfig<TerrainShaderConfig>("ShadowMapShader");

// Diferentes tipos de configuración
auto deviceConfig = ConfigLocator::GetConfig<DeviceManagerConfig>("DeviceManager");
auto keyboardConfig = ConfigLocator::GetConfig<KeyboardConfig>("Keyboard");
```

### Integración con Managers
```cpp
// Los Managers obtienen su configuración automáticamente
class DeviceManager : public ManagerBase {
public:
    HRESULT Init(EngineContext* context) override {
        auto config = ConfigLocator::GetConfig<DeviceManagerConfig>("DeviceManager");
        
        m_vsync = config->vsync;
        m_debugDevice = config->debug_device;
        m_bufferCount = config->swap_chain_buffer_count;
        
        // Usar configuración para inicialización...
    }
};
```

## Características Avanzadas del Sistema

### 1. Dependencies Resolution
```yaml
KeyboardManager:
  dependencies: [Keybindings, Keyboard]
  
# El generador crea automáticamente el orden de inicialización
```

### 2. Herencia de Propiedades
```yaml
Definition:
  inherits_properties: [type, enabled, dependencies]
  
# Todas las clases generadas heredan automáticamente estas propiedades
```

### 3. Registro Automático
```yaml
needs_register: true
register_macro: "REGISTER_CONFIG_MACRO"

# Genera automáticamente el código de registro en ConfigLocator
```

### 4. Output Path Configurable
```yaml
output_path: 'c:\Users\joaqu\source\repos\DirectX-Voxelado\src\Engine\Core\Config\Assets\'

# Controla dónde se generan los archivos .h/.cpp
```

## Ventajas del Sistema YamlToStruct

? **Configuración total**: Desde managers hasta shaders, todo configurable  
? **Factory automático**: ConfigLocator maneja múltiples instancias automáticamente  
? **Registro automático**: Sin código boilerplate  
? **Typesafe**: Clases C++ fuertemente tipadas  
? **Dependencies**: Resolución automática de dependencias  
? **Hot-reload ready**: Cambios en YAML ? regeneración ? recarga  
? **Multi-level**: Soporte para jerarquías complejas  

## Consideraciones para DX12

El sistema YamlToStruct es **perfecto para la migración DX12** porque:

- **Independiente de API**: Las configuraciones son agnósticas a DX11/DX12
- **Fácil migración**: Cambiar configuraciones sin tocar código C++
- **Nuevas configuraciones**: Agregar configs específicas de DX12 (PSO, Root Signatures, etc.)
- **Coexistencia**: Permitir configs DX11 y DX12 simultáneamente durante la migración

### Ejemplo de Configuración DX12
```yaml
Config:
  Graphics:
    Pipeline:
      TerrainPSO:
        type: "GraphicsPipelineState"
        root_signature: "TerrainRootSignature"
        vertex_shader: "TerrainVS"
        pixel_shader: "TerrainPS"
        input_layout: "TextureMapVertex"
        render_target_formats: [DXGI_FORMAT_R8G8B8A8_UNORM]
        depth_format: DXGI_FORMAT_D32_FLOAT
        primitive_topology: D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE
