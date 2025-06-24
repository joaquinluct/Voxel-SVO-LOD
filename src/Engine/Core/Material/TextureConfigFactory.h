// TextureConfigFactory.h
#pragma once

#include "..\Interfaces\ITextureConfig.h" // Asumo que esta ruta es correcta
#include <map>
#include <functional> // Para std::function
#include <memory>
#include <windows.h> // Para OutputDebugStringA (ya que lo usas aquí)

enum TextureType {
    VOXEL_BASIC,
    VOXEL_4K,
    // ... otros tipos de textura ...
    UNKNOWN_TEXTURE_TYPE // Añadir un valor para el default
};

class TextureConfigFactory {
public:
    // Alias para el tipo de función que crea una instancia de ITextureConfig
    using CreatorFunction = std::function<std::unique_ptr<ITextureConfig>()>;

    static std::map<TextureType, CreatorFunction>& GetCreators();

    // Registra una función creadora para un tipo de textura dado
    static bool Register(TextureType type, CreatorFunction creator) {
        // Verifica si el tipo ya está registrado
        if (GetCreators().count(type)) {
            OutputDebugStringA("Advertencia: Tipo de textura ya registrado.\n");
            return false;
        }
        GetCreators()[type] = creator;
        return true;
    }

    // Crea una instancia de la estrategia basada en el tipo
    static std::unique_ptr<ITextureConfig> CreateTextureConfig(TextureType type) {
        auto it = GetCreators().find(type);
        if (it != GetCreators().end()) {
            return it->second(); // Llama a la función creadora registrada
        }
        OutputDebugStringA("Error: Tipo de textura no registrado.\n");
        return nullptr;
    }

//private:
    //// Mapa estático para almacenar las funciones creadoras (DECLARACIÓN)
    //static std::map<TextureType, CreatorFunction> s_creators;
};