// TextureConfigFactory.cpp
#include "TextureConfigFactory.h" // Incluye el header donde se declara la clase

std::map<TextureType, TextureConfigFactory::CreatorFunction>& TextureConfigFactory::GetCreators() {
    static std::map<TextureType, TextureConfigFactory::CreatorFunction> s_creators_instance; // ¡Aquí está la definición y la inicialización garantizada!
    return s_creators_instance;
}