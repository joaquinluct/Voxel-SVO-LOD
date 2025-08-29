// TerrainAsset.cpp
#include "TerrainAsset.h"
#include <type_traits>
#include <DDSTextureLoader.h> // Para CreateDDSTextureFromFile
#include <windows.h> // Para OutputDebugStringA
#include <Assets/Base/VertexAsset.h>
#include <AssetLocator/AssetLocator.h>
#include <DefineLocator/DefineLocator.h>
#include <ManagerLocator/ManagerLocator.h>
#include <REGISTER_ASSET_MACRO.h>
#include <Util/Text/Text.h>
#include <Assets/Base/TextureAsset.h>
#include <Assets/Base/ObjFormat/ObjUtil.h>
#include <Defines/Mesh.h>
#include <Defines/Vector.h>
#include <Assets/Base/MeshAsset.h>
#include <TinyObjLoader/tiny_obj_loader.h>
#include <Defines/VertexDefinition.h>
#include <TerrainAssetConfigBase.h>


REGISTER_ASSET_TYPE(TerrainAsset, "TerrainAsset")

TerrainAsset::TerrainAsset()
    : m_terrainConfig(nullptr), m_material(nullptr), m_shadowMaterial(nullptr) {
}

TerrainAsset::~TerrainAsset() {
    Shutdown();
}

void TerrainAsset::SetConfig(std::shared_ptr<ConfigBase> config) {
    m_terrainConfig = std::dynamic_pointer_cast<TerrainAssetConfigBase>(config);
}

HRESULT TerrainAsset::Init() {
    if (!m_terrainConfig) {
        return S_OK;
    }
    m_shaderName = StringToWstring(m_terrainConfig->shader);

    // Inicializar el material
    InitMaterial();
    InitShadowMaterial();

	m_meshAsset = AssetLocator::GetMeshAsset("TerrainMesh");

    // NOTA: Los búferes dinámicos se crean en el TerrainPass,
    // que es quien conoce el tamaño máximo del terreno.
    return S_OK;
}

void TerrainAsset::InitMaterial() {
    m_textureTransforms = m_terrainConfig->texture_transforms;
    m_material = new Material();
    if (!m_material) {
        OutputDebugStringA("TerrainAsset::InitMaterial - ERROR: Failed to create Material.\n");
        return;
    }
    m_material->SetShaderName(m_shaderName);
    m_material->Init();

    std::string textureAssetName = m_terrainConfig->texture;

    if (!textureAssetName.empty() && textureAssetName != "none") {
        m_textureAsset = AssetLocator::GetTextureAsset(textureAssetName);
        m_textureAsset->SetTextureView(m_material);
        m_material->SetTextureType(m_textureAsset->GetTextureType());
        if (m_textureTransforms.size() == 4) {
            XMFLOAT4 textureTransforms = XMFLOAT4(m_textureTransforms[0], m_textureTransforms[1], m_textureTransforms[2], m_textureTransforms[3]);
            m_material->SetTextureTranforms(textureTransforms);
        }
    }
}

void TerrainAsset::InitShadowMaterial() {
    if (!m_terrainConfig->cast_shadows) {
        return;
    }
    m_shadowMaterial = new Material();
    m_shadowMaterial->SetShaderName(StringToWstring(m_terrainConfig->shader_shadows));
    m_shadowMaterial->Init();
}

HRESULT TerrainAsset::CreateDynamicBuffers(UINT vertexTypeSize, UINT maxVertices, UINT maxIndexes) {
    m_vertexTypeSize = vertexTypeSize;

    // Crear el Vertex Buffer dinámico
    D3D11_BUFFER_DESC vbDesc = {};
    vbDesc.Usage = D3D11_USAGE_DYNAMIC;
    vbDesc.ByteWidth = maxVertices * m_vertexTypeSize;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT hr = ManagerLocator::GetDeviceManager()->GetDevice()->CreateBuffer(&vbDesc, nullptr, &m_vertexBuffer);
    if (FAILED(hr)) {
        return hr;
    }

    // Crear el Index Buffer dinámico
    D3D11_BUFFER_DESC ibDesc = {};
    ibDesc.Usage = D3D11_USAGE_DYNAMIC;
    ibDesc.ByteWidth = maxIndexes * sizeof(UINT);
    ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = ManagerLocator::GetDeviceManager()->GetDevice()->CreateBuffer(&ibDesc, nullptr, &m_indexBuffer);
    if (FAILED(hr)) {
        return hr;
    }

    return S_OK;
}

XMFLOAT4 TerrainAsset::GetTextureTransforms() {
    XMFLOAT4 defaultTransform(1.0f, 1.0f, 0.0f, 0.0f);
    if (m_textureTransforms.empty()) {
        // Si no hay transformaciones de textura, obtenemos las del asset de textura
        defaultTransform = m_material->GetTextureTranforms();
    }
    else {
        defaultTransform = XMFLOAT4(m_textureTransforms[0], m_textureTransforms[1], m_textureTransforms[2], m_textureTransforms[3]);
    }
    return defaultTransform;
}

void TerrainAsset::Shutdown() {
    if (m_vertexBuffer) { m_vertexBuffer.Reset(); }
    if (m_indexBuffer) { m_indexBuffer.Reset(); }
    if (m_material) { SafeShutDown(m_material); }
    if (m_shadowMaterial) { SafeShutDown(m_shadowMaterial); }
}