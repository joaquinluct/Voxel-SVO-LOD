#pragma once

#include <d3d11.h>

struct PBRTextures {
    ID3D11ShaderResourceView* diffuseTexture = nullptr;
    ID3D11ShaderResourceView* normalTexture = nullptr;
    ID3D11ShaderResourceView* specularTexture = nullptr;
    // Agrega ms texturas segn sea necesario (roughness, metallic, ambient occlusion, etc.)
};

enum TextureType {
    NONE,
    VOXEL_BASIC,
    VOXEL_4K,
};

