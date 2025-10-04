#pragma once

// C++14 Standard Library
#include <cassert>

// Debug/Release macros - C++14 enhanced
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

// DirectX 12 specific
#define ENGINE_DX12_ENABLED 1

// C++14 constexpr utilities
#define ENGINE_CONSTEVAL constexpr
#define ENGINE_CONSTEXPR constexpr

// Utility macros C++14
#define ENGINE_ENUM_FLAGS(EnumType) \
    constexpr EnumType operator|(EnumType lhs, EnumType rhs) noexcept { \
        return static_cast<EnumType>( \
            static_cast<std::underlying_type_t<EnumType>>(lhs) | \
            static_cast<std::underlying_type_t<EnumType>>(rhs) \
        ); \
    } \
    constexpr EnumType operator&(EnumType lhs, EnumType rhs) noexcept { \
        return static_cast<EnumType>( \
            static_cast<std::underlying_type_t<EnumType>>(lhs) & \
            static_cast<std::underlying_type_t<EnumType>>(rhs) \
        ); \
    }

// Non-copyable/movable helpers - C++14 style
#define ENGINE_NON_COPYABLE(ClassName) \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete;

#define ENGINE_NON_MOVABLE(ClassName) \
    ClassName(ClassName&&) = delete; \
    ClassName& operator=(ClassName&&) = delete;

#define ENGINE_DEFAULT_MOVE(ClassName) \
    ClassName(ClassName&&) = default; \
    ClassName& operator=(ClassName&&) = default;

// C++14 compatible logging
#ifdef _WIN32
    #include <windows.h>
    #include <sstream>
    
    // Simple logging for C++14
    template<typename T>
    void ENGINE_LOG_IMPL(const T& message) {
        std::ostringstream oss;
        oss << "[ENGINE] " << message << "\n";
        OutputDebugStringA(oss.str().c_str());
    }
    
    template<typename T>
    void ENGINE_LOG_ERROR_IMPL(const T& message) {
        std::ostringstream oss;
        oss << "[ENGINE ERROR] " << message << "\n";
        OutputDebugStringA(oss.str().c_str());
    }
    
    #define ENGINE_LOG(msg) ENGINE_LOG_IMPL(msg)
    #define ENGINE_LOG_ERROR(msg) ENGINE_LOG_ERROR_IMPL(msg)
    #define ENGINE_LOG_WARNING(msg) ENGINE_LOG_IMPL(std::string("[WARNING] ") + (msg))
#else
    #define ENGINE_LOG(msg) ((void)0)
    #define ENGINE_LOG_ERROR(msg) ((void)0)
    #define ENGINE_LOG_WARNING(msg) ((void)0)
#endif

// C++14 compatible attributes (fallback for older compilers)
#if __cpp_attributes >= 200809
    #define ENGINE_NODISCARD [[nodiscard]]
    #define ENGINE_MAYBE_UNUSED [[maybe_unused]]
    #define ENGINE_FALLTHROUGH [[fallthrough]]
#else
    #define ENGINE_NODISCARD
    #define ENGINE_MAYBE_UNUSED
    #define ENGINE_FALLTHROUGH
#endif

#define ENGINE_DEPRECATED(msg) 

// Contract-like assertions (C++14 compatible)
#define ENGINE_EXPECTS(cond) ENGINE_ASSERT(cond)
#define ENGINE_ENSURES(cond) ENGINE_ASSERT(cond)

// Performance hints (C++14 compatible - use compiler-specific attributes if available)
#if defined(__GNUC__) || defined(__clang__)
    #define ENGINE_LIKELY __builtin_expect(!!(x), 1)
    #define ENGINE_UNLIKELY __builtin_expect(!!(x), 0)
#else
    #define ENGINE_LIKELY
    #define ENGINE_UNLIKELY
#endif
