#pragma once
#include "IGPUCreateCommand.h"
#include "Assets/Base/TextureAsset.h"

// Comando para crear una textura D3D11
class CreateTextureGPUCommand : public IGPUCreateCommand {
public:
    std::shared_ptr<TextureAsset> m_textureAsset; // La instancia de Texture que se va a inicializar
    std::string m_filePath;

    CreateTextureGPUCommand(std::shared_ptr<TextureAsset> textureAsset, const std::string& filePath)
        : m_textureAsset(textureAsset), m_filePath(filePath) {
    }

    HRESULT Execute(ID3D11Device* pDevice, ID3D11DeviceContext* pImmediateContext) override {
        // Asegurarse de que el asset sigue existiendo y es válido
        if (!m_textureAsset) {
            OutputDebugStringA("WARNING: CreateTextureGPUCommand: Texture asset is null or expired.\n");
            return E_FAIL;
        }
        // Este es el paso CRÍTICO: La creación de recursos D3D11 ocurre en el hilo principal
        return m_textureAsset->InitD3D11Resources(pDevice, m_filePath);
    }
};