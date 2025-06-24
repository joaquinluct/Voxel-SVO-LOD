#pragma once

// Define tipos de assets que el AssetManager puede cargar
enum class AssetType {
    Unknown,
    Texture,
    Mesh,
    Skybox, // El asset compuesto que crearemos después
    // ... otros tipos de assets (Material, Shader, AudioClip, etc.)
};