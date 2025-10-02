#pragma once
#include <array>
#include <Config/BiomeBaseConfig.h>
#include <Defines/Enums/Biome.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

class BiomeItem {
public:
    // Estructura clave: 3 Zonas x 3 Texturas = 9 rutas de imagen por Dominio.
    // Usaremos un array 2D simple: ImagePaths[Zona][Textura]
    using ImagePaths = std::array<
        std::array<std::string, static_cast<size_t>(ZoneTexture::Count)>, // 3 Texturas por Zona
        static_cast<size_t>(BiomeZone::Count)                            // 3 Zonas
    >;

private:
    std::string m_name;
    int m_id;

    ImagePaths m_land_images;
    ImagePaths m_water_images;

    std::shared_ptr<BiomeBaseConfig> m_config; // Configuración del bioma asociada
public:
    BiomeItem() = default;
    BiomeItem(std::string name, int id, std::shared_ptr<BiomeBaseConfig> config) : m_name(std::move(name)), m_id(id), m_config(config) {}
    ~BiomeItem() {};
    // -----------------------------------------------------------
    // Métodos para establecer/obtener las rutas de las imágenes
    // -----------------------------------------------------------

    // Método para configurar una de las 18 rutas (9 terrestres + 9 acuáticas)
    void set_texture_path(BiomeDomain domain, BiomeZone zone, ZoneTexture tex_index, const std::string& path) {
        size_t z_idx = static_cast<size_t>(zone);
        size_t t_idx = static_cast<size_t>(tex_index);

        if (z_idx >= static_cast<size_t>(BiomeZone::Count) || t_idx >= static_cast<size_t>(ZoneTexture::Count)) {
            throw std::out_of_range("Índice de Zona o Textura inválido.");
        }

        if (domain == BiomeDomain::Land) {
            m_land_images[z_idx][t_idx] = path;
        }
        else if (domain == BiomeDomain::Water) {
            m_water_images[z_idx][t_idx] = path;
        }
    }

    [[nodiscard]] const std::string& get_texture_path(BiomeDomain domain, BiomeZone zone, ZoneTexture tex_index) const {
        size_t z_idx = static_cast<size_t>(zone);
        size_t t_idx = static_cast<size_t>(tex_index);

        if (z_idx >= static_cast<size_t>(BiomeZone::Count) || t_idx >= static_cast<size_t>(ZoneTexture::Count)) {
            throw std::out_of_range("Índice de Zona o Textura inválido.");
        }

        if (domain == BiomeDomain::Land) {
            return m_land_images[z_idx][t_idx];
        }

        return m_water_images[z_idx][t_idx];
    }

    [[nodiscard]] const std::string& get_name() const { return m_name; }
    [[nodiscard]] int get_id() const { return m_id; }
};
