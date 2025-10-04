#pragma once

// C++14 Standard Library (Compatible)
#include <cstdint>
#include <memory>
#include <string>
#include <system_error>
#include <stdexcept>

namespace Engine {

// Forward declarations
enum class EngineError : std::uint32_t;

// ? Fundamental types (C++14 compatible)
using f32 = float;
using f64 = double;
using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;
using uintptr_t = std::uintptr_t;
using intptr_t = std::intptr_t;

// ? String types - C++14 compatible
using String = std::string;
using WString = std::wstring;

// C++14 compatible StringView (simple implementation)
template<typename CharT>
class basic_string_view {
private:
    const CharT* m_data;
    size_t m_size;

public:
    using value_type = CharT;
    using size_type = size_t;
    using const_pointer = const CharT*;

    constexpr basic_string_view() noexcept : m_data(nullptr), m_size(0) {}
    constexpr basic_string_view(const CharT* str) noexcept 
        : m_data(str), m_size(str ? strlen_constexpr(str) : 0) {}
    constexpr basic_string_view(const CharT* str, size_type len) noexcept 
        : m_data(str), m_size(len) {}

    constexpr const_pointer data() const noexcept { return m_data; }
    constexpr size_type size() const noexcept { return m_size; }
    constexpr size_type length() const noexcept { return m_size; }
    constexpr bool empty() const noexcept { return m_size == 0; }

private:
    static constexpr size_t strlen_constexpr(const CharT* str) {
        return *str ? 1 + strlen_constexpr(str + 1) : 0;
    }
};

using StringView = basic_string_view<char>;
using WStringView = basic_string_view<wchar_t>;

// ? Smart pointers - C++14 compatible  
template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

// ? IMPLEMENTACIÓN PROPIA DE Expected PARA C++14
template<typename T, typename E = EngineError>
class Expected {
private:
    bool m_hasValue;
    
    // Aligned storage para T y E
    union Storage {
        T value;
        E error;
        
        // Constructores/destructores explícitos
        Storage() {} 
        ~Storage() {} 
    } m_storage;

public:
    // ? Constructor con valor (C++14 compatible)
    Expected(const T& val) : m_hasValue(true) {
        new (&m_storage.value) T(val);
    }
    
    Expected(T&& val) : m_hasValue(true) {
        new (&m_storage.value) T(std::move(val));
    }
    
    // ? Constructor con error (C++14 compatible)
    Expected(const E& err) : m_hasValue(false) {
        new (&m_storage.error) E(err);
    }
    
    Expected(E&& err) : m_hasValue(false) {
        new (&m_storage.error) E(std::move(err));
    }
    
    // ? Destructor (C++14 compatible)
    ~Expected() {
        if (m_hasValue) {
            m_storage.value.~T();
        } else {
            m_storage.error.~E();
        }
    }
    
    // ? Copy constructor (C++14 compatible)
    Expected(const Expected& other) : m_hasValue(other.m_hasValue) {
        if (m_hasValue) {
            new (&m_storage.value) T(other.m_storage.value);
        } else {
            new (&m_storage.error) E(other.m_storage.error);
        }
    }
    
    // ? Move constructor (C++14 compatible)
    Expected(Expected&& other) noexcept : m_hasValue(other.m_hasValue) {
        if (m_hasValue) {
            new (&m_storage.value) T(std::move(other.m_storage.value));
        } else {
            new (&m_storage.error) E(std::move(other.m_storage.error));
        }
    }
    
    // ? Assignment operators (C++14 compatible)
    Expected& operator=(const Expected& other) {
        if (this != &other) {
            this->~Expected();
            new (this) Expected(other);
        }
        return *this;
    }
    
    Expected& operator=(Expected&& other) noexcept {
        if (this != &other) {
            this->~Expected();
            new (this) Expected(std::move(other));
        }
        return *this;
    }
    
    // ? Interface methods (C++14 compatible)
    bool has_value() const noexcept { return m_hasValue; }
    explicit operator bool() const noexcept { return m_hasValue; }
    
    const T& value() const& { 
        if (!m_hasValue) throw std::runtime_error("Expected has no value");
        return m_storage.value; 
    }
    
    T& value() & { 
        if (!m_hasValue) throw std::runtime_error("Expected has no value");
        return m_storage.value; 
    }
    
    T&& value() && { 
        if (!m_hasValue) throw std::runtime_error("Expected has no value");
        return std::move(m_storage.value); 
    }
    
    const E& error() const& { 
        if (m_hasValue) throw std::runtime_error("Expected has no error");
        return m_storage.error; 
    }
    
    E& error() & { 
        if (m_hasValue) throw std::runtime_error("Expected has no error");
        return m_storage.error; 
    }
    
    // ? Convenience operators (C++14 compatible)
    const T& operator*() const& { return value(); }
    T& operator*() & { return value(); }
    T&& operator*() && { return std::move(value()); }
    
    const T* operator->() const { return &value(); }
    T* operator->() { return &value(); }
};

// ? Error types (C++14 compatible)
enum class EngineError : uint32 {
    None = 0,
    InitializationFailed,
    ConfigurationError,
    GraphicsError,
    WindowError,
    ResourceError
};

// ? C++14 Error category para EngineError
class EngineErrorCategory : public std::error_category {
public:
    const char* name() const noexcept override {
        return "EngineError";
    }
    
    std::string message(int ev) const override {
        switch (static_cast<EngineError>(ev)) {
            case EngineError::None:
                return "No error";
            case EngineError::InitializationFailed:
                return "Initialization failed";
            case EngineError::ConfigurationError:
                return "Configuration error";
            case EngineError::GraphicsError:
                return "Graphics error";
            case EngineError::WindowError:
                return "Window error";
            case EngineError::ResourceError:
                return "Resource error";
            default:
                return "Unknown error";
        }
    }
};

// ? Global error category instance (C++14 compatible)
inline const EngineErrorCategory& engine_error_category() {
    static EngineErrorCategory instance;
    return instance;
}

// ? Make EngineError work with std::error_code (C++14 compatible)
inline std::error_code make_error_code(EngineError e) {
    return std::error_code{static_cast<int>(e), engine_error_category()};
}

// ? Helper functions para Expected (C++14 compatible)
template<typename T>
Expected<T> make_expected(const T& value) {
    return Expected<T>(value);
}

template<typename T>
Expected<T> make_expected(T&& value) {
    return Expected<T>(std::move(value));
}

template<typename T, typename E>
Expected<T, E> make_unexpected(const E& error) {
    return Expected<T, E>(error);
}

} // namespace Engine

// ? C++14 Error code integration - SINTAXIS CORREGIDA
namespace std {
    template<> struct is_error_code_enum<Engine::EngineError> : true_type {};
}
