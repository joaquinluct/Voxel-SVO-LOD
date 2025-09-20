#pragma once

#include "FrameStateBase.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl/client.h>

class Material;

class MaterialFrameState : public FrameStateBase {
public:
    std::map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>> GetConstantBuffers() const { return constantsBuffers; }
    Microsoft::WRL::ComPtr<ID3D11Buffer> GetConstantBuffer(const std::string& name) const {
        auto it = constantsBuffers.find(name);
        if (it != constantsBuffers.end()) {
            return it->second;
        }
        return nullptr; // or handle the case where the buffer is not found
    }
    void SetConstantBuffer(const std::string& name, Microsoft::WRL::ComPtr<ID3D11Buffer> buffer) { constantsBuffers[name] = buffer; }
    void SetConstantBuffers(const std::map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>>& buffers) { constantsBuffers = buffers; }

    UINT GetNumTextures() const { return numTextures; }
    void SetNumTextures(UINT num) { numTextures = num; }

    DirectX::XMFLOAT4 GetTextureTransform() const { return textureTransform; }
    void SetTextureTransform(const DirectX::XMFLOAT4& transform) { textureTransform = transform; }

private:
    std::map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>> constantsBuffers;

    UINT numTextures = 0;
    DirectX::XMFLOAT4 textureTransform = { 1.0f, 1.0f, 0.0f, 0.0f }; // ScaleX, ScaleY, OffsetX, OffsetY

    // POR IMPLEMENTAR
    DirectX::XMFLOAT4 ambientColor = { 0.2f, 0.2f, 0.2f, 1.0f };
    DirectX::XMFLOAT4 diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT4 specularColor = { 1.0f, 1.0f, 1.0f, 1.0f };
    float shininess = 32.0f; // Specular exponent
    bool hasDiffuseTexture = false;
    bool hasSpecularTexture = false;
    bool hasNormalTexture = false;
    bool hasAlphaTexture = false;
    bool isTransparent = false;
    bool isDoubleSided = false;
    bool useVertexColors = false;
    bool useLighting = true;
    bool usePBR = false; // Physically Based Rendering
    bool useEmissive = false; // Emissive material property
    bool useAO = false; // Ambient Occlusion
    bool useHeightMap = false; // Height map for parallax mapping
    bool useReflection = false; // Reflection mapping
    bool useRefraction = false; // Refraction mapping
    bool useSubsurfaceScattering = false; // Subsurface scattering effect
    bool useAnisotropicReflection = false; // Anisotropic reflection effect
    bool useClearCoat = false; // Clear coat layer effect
    bool useSheen = false; // Sheen effect for fabrics and similar materials
    bool useTransmission = false; // Transmission effect for transparent materials
    bool useIridescence = false; // Iridescence effect for materials like soap bubbles or oil slicks
    bool useDetailMap = false; // Detail texture mapping for added surface detail
    bool useTessellation = false; // Tessellation for increased mesh detail
    bool useDisplacementMap = false; // Displacement mapping for actual geometry modification
    bool useFog = false; // Fog effect for atmospheric perspective
    bool useShadowMapping = false; // Shadow mapping for dynamic shadows
    bool useScreenSpaceReflections = false; // Screen space reflections for real-time reflections
    bool useVolumetricEffects = false; // Volumetric effects like fog, smoke, or light shafts
    bool usePostProcessingEffects = false; // Post-processing effects like bloom, color grading, etc.
    bool useCustomShader = false; // Flag to indicate if a custom shader is used
};

