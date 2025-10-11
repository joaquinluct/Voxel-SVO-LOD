#pragma once
#include "TerrainEngineBase.h"
#include <Core/Config/Game/System/TerrainConfig.h>
#include <d3d11.h>
#include <Defines/TerrainChunk.h>
#include <DirectXMath.h>
#include <Engine/Core/Defines/VertexDefinition.h>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * Interfaz base para todos los motores de geometría del terreno.
 * Define los métodos comunes que deben implementar tanto TessellationGeometryEngine
 * como AggregatedGeometryEngine.
 */
class TerrainGeometryEngineBase : public TerrainEngineBase {
public:
    virtual ~TerrainGeometryEngineBase() = default;

    /**
     * Inicializa el engine de geometría con la configuración del terreno.
     * @param config Configuración del terreno generada desde YAML
     */
    virtual void Init(TerrainConfig* config) = 0;

    /**
     * Genera la malla para un chunk específico del terreno.
     * @param globalVertexPool Pool global de vértices compartido entre chunks
     * @param vertexLookup Mapa para evitar duplicar vértices
     * @param localIndices Índices locales para el chunk actual
     * @param chunkId Identificador del chunk a procesar
     * @param zone Zona del buffer (para ring buffering, por defecto 0)
     */
    virtual void GenerateMesh(
        std::vector<VertexDefinition::TextureMapVertex>& globalVertexPool,
        std::unordered_map<size_t, UINT>& vertexLookup,
        std::vector<UINT>& localIndices,
        const TerrainChunk::ChunkID& chunkId,
        int zone = 0) = 0;

    /**
     * Obtiene el nombre del asset de malla asociado a este engine.
     * @return Nombre del mesh asset que usa este geometry engine
     */
    virtual std::string GetMeshAssetName() const = 0;

    /**
     * Prepara el engine para el frame actual (opcional).
     * @param cameraPos Posición de la cámara para cálculos de LOD
     */
    virtual void PrepareForFrame(const DirectX::XMFLOAT3& cameraPos) {}

    /**
     * Ejecuta el dibujado específico del engine (opcional).
     * Usado para engines que requieren pases de renderizado especiales.
     */
    virtual void Draw() {}

    /**
     * Vincula recursos específicos al contexto de dispositivo (opcional).
     * @param context Contexto de DirectX 11
     */
    virtual void Bind(ID3D11DeviceContext* context) {}

protected:
    /**
     * Configuración del terreno compartida entre todos los engines.
     */
    TerrainConfig* m_config = nullptr;
};
