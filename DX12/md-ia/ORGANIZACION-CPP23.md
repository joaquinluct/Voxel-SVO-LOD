# Organización Profesional C++23 - Proyecto DirectX 12

## Estructura de Directorios Recomendada

### Estructura Principal
```
DX12/
??? src/
?   ??? Core/              # Núcleo del motor (independiente de DX)
?   ??? Graphics/          # Sistemas gráficos específicos de DX12
?   ??? Engine/            # Lógica del motor de alto nivel
?   ??? Math/              # Matemáticas y utilidades
?   ??? Platform/          # Código específico de plataforma
?   ??? Utils/             # Utilidades generales
?   ??? Main/              # Punto de entrada
??? include/               # Headers públicos
?   ??? Core/
?   ??? Graphics/
?   ??? Engine/
??? interfaces/            # Interfaces puras (contratos)
??? generated/             # Código generado desde YAML
?   ??? Configs/
?   ??? Defines/
?   ??? Assets/
??? precompiled/           # PCH (Precompiled Headers)
```

## Organización por Categorías

### 1. CORE (Independiente de API gráfica)
```cpp
// Core/Foundation/
??? Types.h              // Tipos fundamentales del motor
??? Memory/              // Gestión de memoria
?   ??? Allocator.h
?   ??? Pool.h
?   ??? SmartPointers.h
??? Containers/          // Contenedores optimizados
?   ??? Array.h
?   ??? HashMap.h
?   ??? String.h
??? Threading/           // Primitivas de threading
?   ??? Thread.h
?   ??? Mutex.h
?   ??? Atomic.h
?   ??? ThreadPool.h
??? Reflection/          // Sistema de reflexión
    ??? TypeInfo.h
    ??? Registry.h
```

### 2. GRAPHICS (Específico DirectX 12)
```cpp
// Graphics/DX12/
??? Device/              // Gestión del dispositivo DX12
?   ??? DX12Device.h
?   ??? DX12Factory.h
?   ??? DX12Adapter.h
?   ??? DX12SwapChain.h
??? Resources/           // Recursos gráficos
?   ??? Buffer.h
?   ??? Texture.h
?   ??? RenderTarget.h
?   ??? DepthStencil.h
??? Pipeline/            // Estados de pipeline y PSO
?   ??? PipelineState.h         // PSO (Pipeline State Object)
?   ??? GraphicsPSO.h          // Graphics Pipeline State
?   ??? ComputePSO.h           // Compute Pipeline State
?   ??? RootSignature.h        // Root Signatures
?   ??? Shader.h               // Shader management
??? Commands/            // Command lists y queues
?   ??? CommandList.h
?   ??? CommandQueue.h
?   ??? CommandAllocator.h
?   ??? CommandSignature.h
??? Descriptors/         // Heaps de descriptores
?   ??? DescriptorHeap.h
?   ??? DescriptorManager.h
?   ??? CBV_SRV_UAV_Heap.h
?   ??? Sampler_Heap.h
?   ??? RTV_DSV_Heap.h
??? Synchronization/     // Sincronización DX12
?   ??? Fence.h                // CPU-GPU synchronization
?   ??? FenceManager.h         // Fence pool and management
?   ??? GPUTimer.h             // GPU timing queries
?   ??? FrameSync.h            // Frame synchronization
??? Debug/               // Debug y profiling DX12
    ??? DX12DebugLayer.h
    ??? PIXMarkers.h
    ??? GPUValidation.h
```

### 3. ENGINE (Lógica de alto nivel)
```cpp
// Engine/
??? Managers/            // Managers del motor
?   ??? RenderManager.h
?   ??? ResourceManager.h
?   ??? SceneManager.h
?   ??? InputManager.h
??? Components/          # Sistema ECS
?   ??? Transform.h
?   ??? Mesh.h
?   ??? Material.h
??? Systems/             # Sistemas del ECS
?   ??? RenderSystem.h
?   ??? UpdateSystem.h
??? World/               # Mundo y escenas
    ??? Scene.h
    ??? Entity.h
    ??? World.h
```

## Convenciones de Naming y Headers

### Headers de Inclusión Estándar
```cpp
// Archivo: Core/Foundation/Types.h
#pragma once

// 1. Estándar C++23 primero
#include <cstdint>
#include <memory>
#include <string_view>
#include <span>
#include <expected>
#include <ranges>

// 2. Platform específico
#ifdef _WIN32
    #include <windows.h>
    #include <d3d12.h>
    #include <dxgi1_6.h>
#endif

// 3. Third party
#include <DirectXMath.h>

// 4. Motor includes
#include "Core/Macros.h"
#include "Core/Config.h"
```

### Tipos Fundamentales del Motor
```cpp
// Core/Foundation/Types.h
#pragma once

namespace Engine {
    // Tipos básicos con tamaño garantizado
    using int8   = std::int8_t;
    using int16  = std::int16_t;
    using int32  = std::int32_t;
    using int64  = std::int64_t;
    using uint8  = std::uint8_t;
    using uint16 = std::uint16_t;
    using uint32 = std::uint32_t;
    using uint64 = std::uint64_t;
    
    using f32 = float;
    using f64 = double;
    
    // String types C++23
    using String = std::string;
    using StringView = std::string_view;
    using WString = std::wstring;
    using WStringView = std::wstring_view;
    
    // Smart pointers
    template<typename T>
    using UniquePtr = std::unique_ptr<T>;
    
    template<typename T>
    using SharedPtr = std::shared_ptr<T>;
    
    template<typename T>
    using WeakPtr = std::weak_ptr<T>;
    
    // Result types C++23
    template<typename T, typename E = std::error_code>
    using Expected = std::expected<T, E>;
    
    // Containers C++23
    template<typename T>
    using Span = std::span<T>;
    
    template<typename T, size_t N>
    using Array = std::array<T, N>;
}
```

### Sistema de Configuración y Context
```cpp
// Core/Context/EngineContext.h
#pragma once

#include "Core/Types.h"

namespace Engine {
    struct EngineContext {
        f32 deltaTime = 0.0f;
        bool isInitialized = false;
        bool isRunning = true;
        uint32 frameCount = 0;
        
        // Platform specific
        void* windowHandle = nullptr;
        uint32 windowWidth = 1920;
        uint32 windowHeight = 1080;
        
        // Graphics context (se poblará en DX12)
        void* graphicsDevice = nullptr;
        void* commandQueue = nullptr;
    };
    
    struct GraphicsContext {
        // Forward declarations para evitar includes de DX12
        struct Device;
        struct CommandQueue;
        struct SwapChain;
        
        SharedPtr<Device> device;
        SharedPtr<CommandQueue> directQueue;
        SharedPtr<CommandQueue> computeQueue;
        SharedPtr<CommandQueue> copyQueue;
        SharedPtr<SwapChain> swapChain;
    };
}
```

### Enums y Constants Modernas C++23
```cpp
// Core/Enums/GraphicsEnums.h
#pragma once

#include <cstdint>

namespace Engine::Graphics {
    // Enum classes fuertemente tipadas
    enum class ResourceType : uint8 {
        Buffer,
        Texture1D,
        Texture2D,
        Texture3D,
        TextureCube
    };
    
    enum class ShaderStage : uint8 {
        Vertex   = 1 << 0,
        Pixel    = 1 << 1,
        Compute  = 1 << 2,
        Geometry = 1 << 3,
        Hull     = 1 << 4,
        Domain   = 1 << 5
    };
    
    // Bitflags con operadores
    constexpr ShaderStage operator|(ShaderStage lhs, ShaderStage rhs) {
        return static_cast<ShaderStage>(
            static_cast<uint8>(lhs) | static_cast<uint8>(rhs)
        );
    }
    
    // Constants como constexpr
    constexpr uint32 MAX_RENDER_TARGETS = 8;
    constexpr uint32 MAX_VERTEX_STREAMS = 16;
    constexpr uint32 MAX_DESCRIPTOR_TABLES = 32;
    
    // DX12 specific constants
    constexpr uint32 MAX_FRAMES_IN_FLIGHT = 3;
    constexpr uint32 FENCE_TIMEOUT_MS = 1000;
}
```

### Macros y Utilities
```cpp
// Core/Macros.h
#pragma once

// Debug/Release macros
#ifdef _DEBUG
    #define ENGINE_DEBUG 1
    #define ENGINE_ASSERT(expr) assert(expr)
    #define ENGINE_DEBUG_ONLY(code) code
#else
    #define ENGINE_DEBUG 0
    #define ENGINE_ASSERT(expr) ((void)0)
    #define ENGINE_DEBUG_ONLY(code) ((void)0)
#endif

// Platform detection
#ifdef _WIN32
    #define ENGINE_PLATFORM_WINDOWS 1
    #define ENGINE_FORCE_INLINE __forceinline
    #define ENGINE_NO_INLINE __declspec(noinline)
#else
    #define ENGINE_PLATFORM_WINDOWS 0
    #define ENGINE_FORCE_INLINE inline
    #define ENGINE_NO_INLINE
#endif

// DX12 specific
#define ENGINE_DX12_ENABLED 1

// Utility macros C++23
#define ENGINE_ENUM_FLAGS(EnumType) \
    constexpr EnumType operator|(EnumType lhs, EnumType rhs) { \
        return static_cast<EnumType>( \
            static_cast<std::underlying_type_t<EnumType>>(lhs) | \
            static_cast<std::underlying_type_t<EnumType>>(rhs) \
        ); \
    }

// Non-copyable/movable helpers
#define ENGINE_NON_COPYABLE(ClassName) \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete;

#define ENGINE_NON_MOVABLE(ClassName) \
    ClassName(ClassName&&) = delete; \
    ClassName& operator=(ClassName&&) = delete;
```

## Componentes DirectX 12 Específicos

### Fence y Sincronización
```cpp
// Graphics/DX12/Synchronization/Fence.h
#pragma once

#include "Core/Types.h"
#include <d3d12.h>

namespace Engine::Graphics::DX12 {
    class Fence {
    public:
        Fence(ID3D12Device* device, uint64 initialValue = 0);
        ~Fence();
        
        // Signal from GPU
        void Signal(ID3D12CommandQueue* queue, uint64 fenceValue);
        
        // Wait on CPU
        void WaitForFenceValue(uint64 fenceValue, uint32 timeoutMs = INFINITE);
        
        // Check completion without blocking
        bool IsFenceComplete(uint64 fenceValue) const;
        
        uint64 GetNextFenceValue() { return ++m_fenceValue; }
        uint64 GetCurrentFenceValue() const { return m_fenceValue; }
        
        ID3D12Fence* GetD3D12Fence() const { return m_fence.Get(); }
        
    private:
        Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
        HANDLE m_fenceEvent;
        uint64 m_fenceValue;
    };
}
```

### Pipeline State Object (PSO)
```cpp
// Graphics/DX12/Pipeline/PipelineState.h
#pragma once

#include "Core/Types.h"
#include <d3d12.h>

namespace Engine::Graphics::DX12 {
    class GraphicsPipelineState {
    public:
        GraphicsPipelineState() = default;
        ~GraphicsPipelineState() = default;
        
        // Builder pattern for PSO creation
        class Builder {
        public:
            Builder& SetRootSignature(ID3D12RootSignature* rootSig);
            Builder& SetVertexShader(const void* bytecode, size_t size);
            Builder& SetPixelShader(const void* bytecode, size_t size);
            Builder& SetInputLayout(const D3D12_INPUT_ELEMENT_DESC* elements, uint32 count);
            Builder& SetRenderTargetFormat(DXGI_FORMAT format, uint32 index = 0);
            Builder& SetDepthStencilFormat(DXGI_FORMAT format);
            Builder& SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology);
            
            Expected<SharedPtr<GraphicsPipelineState>> Build(ID3D12Device* device);
            
        private:
            D3D12_GRAPHICS_PIPELINE_STATE_DESC m_desc = {};
        };
        
        ID3D12PipelineState* GetD3D12PipelineState() const { return m_pipelineState.Get(); }
        
    private:
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
    };
    
    class ComputePipelineState {
    public:
        ComputePipelineState() = default;
        ~ComputePipelineState() = default;
        
        Expected<bool> Initialize(ID3D12Device* device, 
                                ID3D12RootSignature* rootSignature,
                                const void* computeShader, 
                                size_t shaderSize);
        
        ID3D12PipelineState* GetD3D12PipelineState() const { return m_pipelineState.Get(); }
        
    private:
        Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
    };
}
```

## Características C++23 Utilizadas

### 1. Modules (Experimental pero útil)
```cpp
// Engine.ixx (Module interface)
export module Engine.Core;

export import <memory>;
export import <string_view>;
export import <expected>;

export namespace Engine {
    class Device;
    class Renderer;
}
```

### 2. Ranges y Views
```cpp
// Utils/RangeUtils.h
#include <ranges>
#include <algorithm>

namespace Engine::Utils {
    template<typename Range, typename Predicate>
    auto filter_and_transform(Range&& range, Predicate pred) {
        return range 
            | std::views::filter(pred)
            | std::views::transform([](const auto& item) { 
                return item.process(); 
            });
    }
}
```

### 3. Expected para Error Handling
```cpp
// Graphics/Result.h
#include <expected>
#include <system_error>

namespace Engine::Graphics {
    enum class GraphicsError {
        DeviceCreationFailed,
        ResourceCreationFailed,
        CommandListFailed,
        FenceCreationFailed,
        PSOCreationFailed
    };
    
    template<typename T>
    using Result = std::expected<T, GraphicsError>;
    
    template<typename T>
    Result<T> CreateResource(const ResourceDesc& desc) {
        // Implementation...
        if (failed) {
            return std::unexpected(GraphicsError::ResourceCreationFailed);
        }
        return resource;
    }
}
```

### 4. Concepts para Templates
```cpp
// Core/Concepts.h
#include <concepts>

namespace Engine::Concepts {
    template<typename T>
    concept Renderable = requires(T t) {
        t.Render();
        { t.IsVisible() } -> std::convertible_to<bool>;
    };
    
    template<typename T>
    concept GraphicsResource = requires(T t) {
        typename T::NativeHandle;
        { t.GetNativeHandle() } -> std::convertible_to<typename T::NativeHandle>;
    };
    
    template<typename T>
    concept DX12Resource = requires(T t) {
        { t.GetD3D12Resource() } -> std::convertible_to<ID3D12Resource*>;
    };
}
```

## Precompiled Headers (PCH)

### Archivo PCH
```cpp
// precompiled/EnginePCH.h
#pragma once

// Standard library más usado
#include <memory>
#include <string>
#include <string_view>
#include <vector>
#include <array>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <ranges>
#include <expected>
#include <span>
#include <format>

// Platform
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <d3d12.h>
    #include <dxgi1_6.h>
    #include <d3dcompiler.h>
    #include <wrl/client.h>  // Para ComPtr
#endif

// Third party common
#include <DirectXMath.h>

// Engine foundation
#include "Core/Types.h"
#include "Core/Macros.h"
#include "Core/Context/EngineContext.h"
```

Esta estructura actualizada incluye todos los componentes esenciales de DirectX 12, incluyendo **Fence**, **PSO** (tanto Graphics como Compute), y una organización más completa para un motor gráfico profesional.
