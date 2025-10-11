#include "TessellationGeometryEngine.h"
#include <Windows.h>
#include <sstream>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <ManagerLocator/ManagerLocator.h>
#include <DeviceManager.h>

TessellationGeometryEngine::~TessellationGeometryEngine() {
    // ComPtrs auto-release
}

static HRESULT CreateBuffer(ID3D11Device* device, ID3D11Buffer** buf, const void* data, UINT byteWidth, D3D11_BIND_FLAG bindFlags, D3D11_USAGE usage = D3D11_USAGE_DEFAULT) {
    D3D11_BUFFER_DESC desc = {};
    desc.BindFlags = bindFlags;
    desc.ByteWidth = byteWidth;
    desc.CPUAccessFlags = (usage == D3D11_USAGE_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE : 0;
    desc.Usage = usage;
    D3D11_SUBRESOURCE_DATA sd = {};
    sd.pSysMem = data;
    return device->CreateBuffer(&desc, data ? &sd : nullptr, buf);
}

void TessellationGeometryEngine::Init(TerrainConfig* config) {
    m_config = config;
    OutputDebugStringA("[TessellationGeometryEngine] Init GPU path\n");

    auto deviceManager = ManagerLocator::GetDeviceManager();
    auto device = deviceManager->GetDevice().Get();
    if (!device) {
        OutputDebugStringA("[TessellationGeometryEngine] No D3D device available\n");
        return;
    }

    // Determinar el nombre del shader a usar: preferir indicación en TerrainConfig->geometry_engine
    std::string shaderName = "TerrainTessellation"; // default
    if (m_config) {
        if (!m_config->geometry_engine.empty()) {
            // Si el geometry_engine contiene 'Tessellation' usamos el shader por convención
            if (m_config->geometry_engine.find("Tessellation") != std::string::npos) {
                shaderName = "TerrainTessellation";
            } else {
                // fallback: conservar default o mapear según convención futura
                shaderName = "TerrainTessellation";
            }
        }
    }
    // TODO: Re-enable when AssetLocator is available
    std::string dbg = "[TessellationGeometryEngine] Requesting shader asset '" + shaderName + "'\n";
    OutputDebugStringA(dbg.c_str());
    // auto shaderAsset = AssetLocator::GetShaderAsset(shaderName);
    // if (!shaderAsset) {
    //     std::string err = "[TessellationGeometryEngine] Shader asset '" + shaderName + "' no encontrado. Asegurate de declararlo en Resources/Config/Assets/assets_shader.yaml\n";
    //     OutputDebugStringA(err.c_str());
    //     return;
    // }
    
    // Por ahora, inicializar los recursos básicos sin shaders
    OutputDebugStringA("[TessellationGeometryEngine] Initializing basic resources without shaders\n");

    // Garantizar que ShaderManager haya inicializado los shaders
    auto shaderManager = ManagerLocator::GetManager<ShaderManager>();
    if (shaderManager) {
        // Cargar por nombre para forzar inicializacion si hace falta
        std::wstring wShaderName(shaderName.begin(), shaderName.end());
        OutputDebugStringA(("[TessellationGeometryEngine] Loading shader via ShaderManager: " + shaderName + "\n").c_str());
        shaderManager->LoadShaderByName(wShaderName);
    }

    // TODO: Re-enable when AssetLocator is available
    // ID3DBlob* vsBlob = shaderAsset->GetVertexShaderBlob();
    // ID3DBlob* psBlob = shaderAsset->GetPixelShaderBlob(); 
    // ID3D11HullShader* hsObj = shaderAsset->GetHullShader();
    // ID3D11DomainShader* dsObj = shaderAsset->GetDomainShader();
    // ID3D11VertexShader* vsObj = shaderAsset->GetVertexShader();
    // ID3D11PixelShader* psObj = shaderAsset->GetPixelShader();

    // if (vsObj) m_vs = vsObj;
    // if (psObj) m_ps = psObj; 
    // if (hsObj) m_hs = hsObj;
    // if (dsObj) m_ds = dsObj;

    // ID3D11InputLayout* layoutFromAsset = shaderAsset->GetInputLayout();
    // TODO: Re-enable when shader loading is restored
    // if (layoutFromAsset) {
    //     m_inputLayout = layoutFromAsset;
    // } else {
        // Create a simple input layout for now
        D3D11_INPUT_ELEMENT_DESC layoutDesc[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        // TODO: Create with proper VS blob when available
        // device->CreateInputLayout(layoutDesc, ARRAYSIZE(layoutDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), m_inputLayout.GetAddressOf());
        OutputDebugStringA("[TessellationGeometryEngine] Warning: InputLayout creation temporarily disabled\n");
    // }

    // Create a unit quad patch (4 control points) in local space - positions only
    struct CP { float x,y,z; } quad[4] = {
        { 0.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f },
        { 1.0f, 0.0f, 1.0f },
    };
    CreateBuffer(device, (ID3D11Buffer**)m_patchVB.GetAddressOf(), quad, sizeof(quad), D3D11_BIND_VERTEX_BUFFER);

    // indices (two triangles) - domain shader will re-evaluate positions; indices still useful for base topology
    uint16_t indices[6] = { 0,1,2, 2,1,3 };
    m_indexCount = 6;
    CreateBuffer(device, (ID3D11Buffer**)m_patchIB.GetAddressOf(), indices, sizeof(indices), D3D11_BIND_INDEX_BUFFER);

    // Create instance constant buffer (dynamic)
    D3D11_BUFFER_DESC cbd = {};
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.ByteWidth = sizeof(InstanceCB);
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    device->CreateBuffer(&cbd, nullptr, m_instanceCB.GetAddressOf());

    // Los blobs/objetos son gestionados por ShaderAsset/ShaderManager; no liberamos aqui.

    OutputDebugStringA("[TessellationGeometryEngine] Shaders and buffers created\n");
}

void TessellationGeometryEngine::PrepareForFrame(const DirectX::XMFLOAT3& cameraPos) {
    // Prepare a single instance at camera position for demo
    DirectX::XMFLOAT4X4 m;
    DirectX::XMStoreFloat4x4(&m, DirectX::XMMatrixTranslation(cameraPos.x, cameraPos.y, cameraPos.z));
    m_instanceMatrices.clear();
    m_instanceMatrices.push_back(m);
    OutputDebugStringA("[TessellationGeometryEngine] PrepareForFrame: 1 instance enqueued\n");
}

void TessellationGeometryEngine::Draw() {
    auto deviceManager = ManagerLocator::GetDeviceManager();
    auto ctx = deviceManager->GetContext().Get();
    if (!ctx) {
        OutputDebugStringA("[TessellationGeometryEngine] No D3D context\n");
        return;
    }

    // Set input layout
    ctx->IASetInputLayout(m_inputLayout.Get());

    // Set primitive topology to patches with 4 control points
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

    UINT stride = sizeof(float) * 3; // x,y,z (SimpleVertex)
    UINT offset = 0;
    ID3D11Buffer* vbs[] = { m_patchVB.Get() };
    ctx->IASetVertexBuffers(0, 1, vbs, &stride, &offset);
    ctx->IASetIndexBuffer(m_patchIB.Get(), DXGI_FORMAT_R16_UINT, 0);

    // Set shaders
    ctx->VSSetShader(m_vs.Get(), nullptr, 0);
    ctx->HSSetShader(m_hs.Get(), nullptr, 0);
    ctx->DSSetShader(m_ds.Get(), nullptr, 0);
    ctx->PSSetShader(m_ps.Get(), nullptr, 0);

    // Draw instances
    for (size_t i = 0; i < m_instanceMatrices.size(); ++i) {
        // update instance CB
        D3D11_MAPPED_SUBRESOURCE mapped = {};
        if (SUCCEEDED(ctx->Map(m_instanceCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
            InstanceCB cb;
            cb.instanceWorld = m_instanceMatrices[i];
            cb.LodFactor = 8.0f; // example lod
            memcpy(mapped.pData, &cb, sizeof(cb));
            ctx->Unmap(m_instanceCB.Get(), 0);
        }
        ctx->DSSetConstantBuffers(1, 1, m_instanceCB.GetAddressOf());

        ctx->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);
    }

    // Unbind hull/domain shaders to avoid affecting following passes
    ctx->HSSetShader(nullptr, nullptr, 0);
    ctx->DSSetShader(nullptr, nullptr, 0);

    std::ostringstream ss;
    ss << "[TessellationGeometryEngine] Draw called. Instances=" << m_instanceMatrices.size() << "\n";
    OutputDebugStringA(ss.str().c_str());
}

void TessellationGeometryEngine::Bind(ID3D11DeviceContext* ctx) {
    if (!ctx) return;

    // Set input layout
    ctx->IASetInputLayout(m_inputLayout.Get());

    // Set primitive topology to patches with 4 control points
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

    UINT stride = sizeof(float) * 3; // x,y,z (SimpleVertex)
    UINT offset = 0;
    ID3D11Buffer* vbs[] = { m_patchVB.Get() };
    ctx->IASetVertexBuffers(0, 1, vbs, &stride, &offset);
    ctx->IASetIndexBuffer(m_patchIB.Get(), DXGI_FORMAT_R16_UINT, 0);

    // Set shaders (bind them but do not draw)
    ctx->VSSetShader(m_vs.Get(), nullptr, 0);
    ctx->HSSetShader(m_hs.Get(), nullptr, 0);
    ctx->DSSetShader(m_ds.Get(), nullptr, 0);
    ctx->PSSetShader(m_ps.Get(), nullptr, 0);
}

void TessellationGeometryEngine::GenerateMesh(
    std::vector<VertexDefinition::TextureMapVertex>& globalVertexPool,
    std::unordered_map<size_t, UINT>& vertexLookup,
    std::vector<UINT>& localIndices,
    const TerrainChunk::ChunkID& chunkId,
    int zone) {
    
    // Para tessellation, generamos una malla base simple de 4 vértices (quad)
    // que luego será subdividida por el tessellation shader
    
        // Calcular posición del chunk en el mundo
        float chunkSize = 32.0f; // Tamaño default del chunk
        // TODO: Obtener chunk_size de la configuración correcta cuando esté disponible
    
    float worldX = chunkId.x * chunkSize;
    float worldZ = chunkId.z * chunkSize;
    
        // Crear 4 vértices de esquina para el quad del chunk
        VertexDefinition::TextureMapVertex vertices[4];
        
        for (int i = 0; i < 4; ++i) {
            // Inicializar tangent y debugColor con valores por defecto
            vertices[i].tangent[0] = 1.0f; vertices[i].tangent[1] = 0.0f; vertices[i].tangent[2] = 0.0f;
            vertices[i].debugColor[0] = 1.0f; vertices[i].debugColor[1] = 1.0f; vertices[i].debugColor[2] = 1.0f; vertices[i].debugColor[3] = 1.0f;
        }
        
        // Esquina inferior izquierda
        vertices[0].position[0] = worldX; vertices[0].position[1] = 0.0f; vertices[0].position[2] = worldZ;
        vertices[0].normal[0] = 0.0f; vertices[0].normal[1] = 1.0f; vertices[0].normal[2] = 0.0f;
        vertices[0].texCoord[0] = 0.0f; vertices[0].texCoord[1] = 1.0f;
        
        // Esquina inferior derecha
        vertices[1].position[0] = worldX + chunkSize; vertices[1].position[1] = 0.0f; vertices[1].position[2] = worldZ;
        vertices[1].normal[0] = 0.0f; vertices[1].normal[1] = 1.0f; vertices[1].normal[2] = 0.0f;
        vertices[1].texCoord[0] = 1.0f; vertices[1].texCoord[1] = 1.0f;
        
        // Esquina superior izquierda
        vertices[2].position[0] = worldX; vertices[2].position[1] = 0.0f; vertices[2].position[2] = worldZ + chunkSize;
        vertices[2].normal[0] = 0.0f; vertices[2].normal[1] = 1.0f; vertices[2].normal[2] = 0.0f;
        vertices[2].texCoord[0] = 0.0f; vertices[2].texCoord[1] = 0.0f;
        
        // Esquina superior derecha
        vertices[3].position[0] = worldX + chunkSize; vertices[3].position[1] = 0.0f; vertices[3].position[2] = worldZ + chunkSize;
        vertices[3].normal[0] = 0.0f; vertices[3].normal[1] = 1.0f; vertices[3].normal[2] = 0.0f;
        vertices[3].texCoord[0] = 1.0f; vertices[3].texCoord[1] = 0.0f;
    
    // Agregar vértices al pool global
    UINT baseIndex = static_cast<UINT>(globalVertexPool.size());
    for (int i = 0; i < 4; ++i) {
        globalVertexPool.push_back(vertices[i]);
    }
    
    // Crear índices para dos triángulos que forman el quad
    // Triángulo 1: 0, 1, 2
    localIndices.push_back(baseIndex + 0);
    localIndices.push_back(baseIndex + 1); 
    localIndices.push_back(baseIndex + 2);
    
    // Triángulo 2: 2, 1, 3
    localIndices.push_back(baseIndex + 2);
    localIndices.push_back(baseIndex + 1);
    localIndices.push_back(baseIndex + 3);
    
    std::ostringstream ss;
    ss << "[TessellationGeometryEngine] Generated mesh for chunk (" << chunkId.x << ", " << chunkId.z 
       << ") with " << 4 << " vertices and " << 6 << " indices\n";
    OutputDebugStringA(ss.str().c_str());
}

std::string TessellationGeometryEngine::GetMeshAssetName() const {
    // Para tessellation, usamos un asset de malla específico
    return "TerrainTessellationPatch";
}

void TessellationGeometryEngine::RenderChunks(const std::vector<TerrainChunk::ChunkID>& chunkIds, const DirectX::XMFLOAT3& cameraPos) {
    if (chunkIds.empty()) {
        return;
    }
    
    auto deviceManager = ManagerLocator::GetDeviceManager();
    auto ctx = deviceManager->GetContext().Get();
    if (!ctx) {
        OutputDebugStringA("[TessellationGeometryEngine] No D3D context available for rendering\n");
        return;
    }
    
    // Preparar matrices de instancia para todos los chunks
    m_instanceMatrices.clear();
    m_instanceMatrices.reserve(chunkIds.size());
    
    float chunkSize = 32.0f; // TODO: Obtener de configuración
    
    for (const auto& chunkId : chunkIds) {
        DirectX::XMFLOAT4X4 worldMatrix;
        float worldX = chunkId.x * chunkSize;
        float worldZ = chunkId.z * chunkSize;
        
        // Crear matriz de transformación para este chunk
        DirectX::XMStoreFloat4x4(&worldMatrix, 
            DirectX::XMMatrixTranslation(worldX, 0.0f, worldZ));
        
        m_instanceMatrices.push_back(worldMatrix);
    }
    
    OutputDebugStringA(("[TessellationGeometryEngine] Rendering " + 
        std::to_string(chunkIds.size()) + " chunks with tessellation\n").c_str());
    
    // Configurar el pipeline de tessellation
    Bind(ctx);
    
    // Renderizar todos los chunks como instancias
    for (size_t i = 0; i < m_instanceMatrices.size(); ++i) {
        // Actualizar constant buffer de instancia
        if (m_instanceCB) {
            D3D11_MAPPED_SUBRESOURCE mapped = {};
            if (SUCCEEDED(ctx->Map(m_instanceCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
                InstanceCB cb;
                cb.instanceWorld = m_instanceMatrices[i];
                cb.LodFactor = 8.0f; // TODO: Calcular LOD basado en distancia a cámara
                memcpy(mapped.pData, &cb, sizeof(cb));
                ctx->Unmap(m_instanceCB.Get(), 0);
            }
            
            // Vincular constant buffer al domain shader
            ctx->DSSetConstantBuffers(1, 1, m_instanceCB.GetAddressOf());
        }
        
        // Dibujar como patch tessellated
        if (m_indexCount > 0) {
            ctx->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);
        }
    }
    
    // Limpiar pipeline de tessellation
    ctx->HSSetShader(nullptr, nullptr, 0);
    ctx->DSSetShader(nullptr, nullptr, 0);
}

// Global instance required by Terrain.cpp
TessellationGeometryEngine s_tessEngine;
