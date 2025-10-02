#pragma once

#include <type_traits>

// 2. Función de conversión estática (para evitar el static_cast en el código de uso)
//    Esta función encapsula la conversión explícita.
//    Se utiliza 'consteval' si el compilador lo soporta y quieres que la conversión
//    sea siempre en tiempo de compilación cuando sea posible (C++20/23).
template <typename EnumT, typename IntT>
[[nodiscard]] constexpr EnumT to_enum(IntT valor) noexcept {
    // Asegúrate de que IntT es el tipo subyacente correcto para mayor seguridad,
    // o al menos que sea convertible a él.
    using UnderlyingType = std::underlying_type_t<EnumT>;

    // La conversión es deliberadamente 'static_cast' para indicar que es
    // una conversión insegura (el valor 'valor' podría no ser un enumerador válido)
    // pero que el programador está asumiendo el riesgo.
    return static_cast<EnumT>(static_cast<UnderlyingType>(valor));
}
