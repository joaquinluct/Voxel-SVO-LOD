#pragma once

#include <cstdint>
#include <d3d11.h>
#include <string>
#include <variant>

// Este namespace define los tipos de samplers que se pueden usar en los shaders
// Tiene dos tipos principales: RegularSampler y ShadowMapSampler, cada uno con su propia configuración de D3D11_SAMPLER_DESC.
// También incluye funciones para obtener un sampler basado en su tipo o nombre.
// Además, se usa std::variant y std::optional para manejar los samplers de manera flexible.

namespace ShaderSampler
{
    enum class Type : uint8_t
    {
        None = 0,
        RegularSampler = 1 << 0,
        ShadowMapSampler = 1 << 1,
        //Sampler2DArray = 1 << 2,
        //Sampler3D = 1 << 3,
        //SamplerBuffer = 1 << 4,
        //SamplerExternalOES = 1 << 5, // Android only
    };
    inline constexpr Type operator|(Type a, Type b) noexcept
    {
        return static_cast<Type>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
    }

    struct RegularSampler
    {
        D3D11_SAMPLER_DESC GetDesc() {
            D3D11_SAMPLER_DESC samplerDesc = {};
            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // Tipo de filtrado (bilineal, trilineal, etc.)
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;    // Comportamiento de "envoltura" para coordenadas U
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;    // Comportamiento de "envoltura" para coordenadas V
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;    // Comportamiento de "envoltura" para coordenadas W (para 3D/cubemaps)

            samplerDesc.MipLODBias = 0.0f;
            samplerDesc.MaxAnisotropy = 1; // O más si usas filtrado anisotrópico

            samplerDesc.BorderColor[0] = 0;
            samplerDesc.BorderColor[1] = 0;
            samplerDesc.BorderColor[2] = 0;
            samplerDesc.BorderColor[3] = 0;

            samplerDesc.MinLOD = 0;
            samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // Usar todos los mipmaps
            return samplerDesc;
        }
    };

    struct ShadowMapSampler
    {
        D3D11_SAMPLER_DESC GetDesc() {
            D3D11_SAMPLER_DESC samplerDesc = {};

            // Usamos un filtro de comparación para PCF (Percentage-Closer Filtering)
            samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;

            // Comportamiento de "borde" para evitar que las sombras se envuelvan
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

            // El color del borde debe ser 1.0f (blanco) para que los píxeles fuera del mapa
            // de sombras no se consideren en sombra.
            samplerDesc.BorderColor[0] = 1.0f;
            samplerDesc.BorderColor[1] = 1.0f;
            samplerDesc.BorderColor[2] = 1.0f;
            samplerDesc.BorderColor[3] = 1.0f;

            // La función de comparación. LESS significa que el test pasa si la profundidad
            // del píxel actual es menor que la del mapa de sombras.
            samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

            samplerDesc.MipLODBias = 0.0f;
            samplerDesc.MaxAnisotropy = 1;
            samplerDesc.MinLOD = 0;
            samplerDesc.MaxLOD = 0; // Usar solo el nivel 0 de los mipmaps

            return samplerDesc;
        }
    };

    struct UISampler
    {
        D3D11_SAMPLER_DESC GetDesc() {
            D3D11_SAMPLER_DESC samplerDesc = {};
            samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; // Filtrado por punto para UI
            samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

            samplerDesc.MipLODBias = 0.0f;
            samplerDesc.MaxAnisotropy = 1; // O más si usas filtrado anisotrópico

            samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS; // No usamos comparación para UI

            samplerDesc.BorderColor[0] = 0;
            samplerDesc.BorderColor[1] = 0;
            samplerDesc.BorderColor[2] = 0;
            samplerDesc.BorderColor[3] = 0;

            samplerDesc.MinLOD = 0;
            samplerDesc.MaxLOD = D3D11_FLOAT32_MAX; // Usar todos los mipmaps
            return samplerDesc;
        }
    };

    // Estructura con la definición de un sampler
    struct SamplerDefinition
    {
        std::string name = "";
        int slot = 0;
        D3D11_SAMPLER_DESC desc = {};
    };

    using SamplerVariant = std::variant<RegularSampler, ShadowMapSampler, UISampler>;
    /*using OptionalSamplerVariant = std::optional<SamplerVariant>;

    OptionalSamplerVariant GetSampler(Type type)
    {
        switch (type)
        {
        case Type::RegularSampler:
            return RegularSampler();
        case Type::ShadowMapSampler:
            return ShadowMapSampler();
        default:
            return std::nullopt;
        }
    }*/

    SamplerVariant GetSampler(std::string samplerName);

} // namespace ShaderSampler
