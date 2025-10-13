#pragma once
#include <Game/Systems/Terrain/Defines/TerrainGeometryEngineBase.h>
#include <vector>
#include <unordered_map>
#include <DirectXMath.h>
#include <wrl.h>
#include <Engine/Core/Defines/VertexDefinition.h>
#include <Defines/TerrainChunk.h>

struct InstanceCB {
    DirectX::XMFLOAT4X4 instanceWorld;
    float LodFactor;
    float padding[3];
};

class TessellationGeometryEngine : public TerrainGeometryEngineBase {
public:
    TessellationGeometryEngine() = default;
    ~TessellationGeometryEngine();
    void Init(TerrainConfig* config) override;
    void GenerateMesh(
        std::vector<VertexDefinition::TextureMapVertex>& globalVertexPool,
        std::unordered_map<size_t, UINT>& vertexLookup,
        std::vector<UINT>& localIndices,
        const TerrainChunk::ChunkID& chunkId,
        int zone = 0) override;
    std::string GetMeshAssetName() const override;
    void PrepareForFrame(const DirectX::XMFLOAT3& cameraPos) override;
    void Draw() override;
    void Bind(ID3D11DeviceContext* context) override;

    // Método específico para renderizar chunks con tessellation
    void RenderChunks(const std::vector<TerrainChunk::ChunkID>& chunkIds, const DirectX::XMFLOAT3& cameraPos);
private:
    std::vector<DirectX::XMFLOAT4X4> m_instanceMatrices; // simple instance store

    // D3D resources
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vs;
    Microsoft::WRL::ComPtr<ID3D11HullShader> m_hs;
    Microsoft::WRL::ComPtr<ID3D11DomainShader> m_ds;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_ps;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_patchVB;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_patchIB;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_instanceCB;
    UINT m_indexCount = 0;
};
