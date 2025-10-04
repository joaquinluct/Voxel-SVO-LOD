# Sistema de Locators (Service Locator Pattern)

## Propósito

El sistema de Locators implementa el patrón Service Locator para gestionar la creación, registro y acceso a diferentes tipos de objetos del motor de manera desacoplada y centralizada.

## Arquitectura de Locators

### Tipos de Locators Implementados

1. **ServiceLocator** - Servicios base del motor
2. **ManagerLocator** - Managers específicos del motor  
3. **ConfigLocator** - Configuraciones generadas desde YAML
4. **AssetLocator** - Assets y recursos
5. **PipelineStateLocator** - Estados de pipeline de renderizado
6. **RenderPassLocator** - Pases de renderizado
7. **DefineLocator** - Definiciones y constantes

### Patrón Base Común

Todos los locators siguen la misma estructura:

```cpp
class [Tipo]Locator {
public:
    // Singleton pattern
    static [Tipo]Locator& GetInstance();
    
    // Registro de creadores con lambdas
    static void Register[Tipo]Creator(...);
    
    // Obtención tipada por template
    template<typename T>
    static std::shared_ptr<T> Get[Tipo]();
    
    // Inicialización en lotes
    static HRESULT Initialize[Tipo]s(...);
    
    // Limpieza
    static void Shutdown();
    
private:
    static std::map<std::string, [Tipo]Entry>& Get[Tipo]Entries();
};
```

### Sistema de Registro Automático

#### Macros de Registro
- **REGISTER_SERVICE_TYPE(ClassName, "StringName")**
- **REGISTER_MANAGER_TYPE(ClassName, "StringName")**  
- **REGISTER_CONFIG_TYPE(ClassName, "StringName")**

#### Lambdas Generadas Automáticamente
Cada macro genera lambdas para:
- **Creación**: Instanciación del objeto
- **Inicialización**: Llamada al método Init()
- **Renderizado**: Llamada al método Render() (si implementa IRenderable)
- **Actualización**: Llamada al método Update() (si implementa IUpdatable)

### Interfaces Base

#### Jerarquía de Interfaces
```cpp
// Base común
class IService { ... };
class IConfig { ... };
class ManagerBase : public IService { ... };

// Interfaces funcionales
class IInitializable { 
    virtual HRESULT Init() = 0; 
};

class IEngineDependentInitializable { 
    virtual HRESULT Init(EngineContext* context) = 0; 
};

class IRenderable { 
    virtual void Render() = 0; 
};

class IUpdatable { 
    virtual void Update(float deltaTime) = 0; 
};
```

### Ventajas del Sistema

? **Desacoplamiento**: Los objetos no necesitan conocer cómo crear sus dependencias  
? **Registro automático**: Las macros registran automáticamente las clases al arranque  
? **Tipado seguro**: Templates garantizan type safety en tiempo de compilación  
? **Ciclo de vida gestionado**: Inicialización, renderizado y actualización centralizados  
? **Configuración externa**: Orden de inicialización definido en YAML  

### Flujo de Trabajo

1. **Registro**: Las macros registran automáticamente los creadores al inicio del programa
2. **Creación**: Los locators crean instancias usando las lambdas registradas
3. **Inicialización**: Se inicializan en orden específico (definido en YAML)
4. **Uso**: Acceso tipado a través de `Get[Tipo]<ClassName>()`
5. **Limpieza**: Shutdown centralizado de todos los objetos

### Ejemplo de Uso

```cpp
// Registro automático (se ejecuta al inicio del programa)
REGISTER_MANAGER_TYPE(DeviceManager, "DeviceManager")

// Uso posterior en el código
auto deviceManager = ManagerLocator::GetManager<DeviceManager>();
if (deviceManager) {
    deviceManager->CreateDevice();
}
```

## Consideraciones para DX12

El sistema de Locators es **independiente de la API gráfica**, lo que facilita la migración:

- Los Managers pueden cambiar internamente de DX11 a DX12
- Las interfaces permanecen estables  
- El sistema de registro no requiere modificaciones
- La configuración YAML controla qué implementaciones usar
