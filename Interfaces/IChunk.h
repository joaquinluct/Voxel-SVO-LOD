#pragma once
#include <Defines/CameraDefinition.h>
#include <Defines/TerrainChunk.h>
#include <DirectXMath.h>
#include <IDefine/IVertex.h>
#include <memory>
#include <vector>
#include <Windows.h>

// Forward declaration para evitar includes cíclicos
class ICamera;
namespace Util { struct BoundingBox; }

class IChunk {
public:
    virtual ~IChunk() = default;

    // --- Propiedades Espaciales y de Identificación ---
    virtual TerrainChunk::ChunkID GetID() const = 0;
    virtual DirectX::XMFLOAT3 GetCenter() const { return DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f}; }
    virtual const Util::BoundingBox& GetBoundingBox() const = 0;
    virtual float GetChunkSize() const = 0;

    // --- Control de Visibilidad y Distancia (CPU) ---
    virtual bool IsInFrustum(const std::vector<CameraDefinition::FrustumPlane>& frustumPlanes) const = 0;
    virtual float GetDistanceToCamera(std::shared_ptr<ICamera> camera) const = 0;

    // --- Control de Estado ---
    virtual bool IsDirty() const = 0; // Necesita actualización (LOD o Regeneración)
    virtual void SetDirty(bool dirty) = 0;

    // --- Control de LOD (Interfaz Genérica) ---
    // En el Chunk modular, esto activa la lógica de obtención de datos de LOD.
    virtual void UpdateLOD(float distance, int maxLODs) = 0;
    virtual int GetCurrentLOD() const = 0; // O factor de teselación si es GPU-LOD

    // --- Control de Generación Asíncrona (Para el Job System) ---
    // Indica si los datos de la CPU (malla, patch o parámetros) están listos.
    virtual bool HasCPUMeshDataReady() const = 0;

    // --- Control de Render (La clase concreta implementa su propio GetRenderData) ---
    // Este método es crucial para que el motor de geometría obtenga lo necesario para dibujar.
    // virtual RenderData GetRenderData() = 0; // Esto es mejor dejarlo en la clase concreta.

    // ... Métodos de Inicialización y Shutdown si son necesarios ...
    virtual HRESULT Init() = 0;
    // Opcionales: métodos que algunas implementaciones esperan sobreescribir.
    // Se proveen implementaciones por defecto para evitar errores de override
    // cuando las clases concretas añaden estos métodos.
    virtual void Render() { }
    virtual void Update() { }
    virtual void Shutdown() { }

    // Posición auxiliar (algunas implementaciones usan GetPosition en lugar de GetCenter)
    virtual DirectX::XMFLOAT3 GetPosition() const { return DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f}; }

    // Acceso a datos de malla en CPU (si aplica)
    virtual std::vector<IVertex> GetVertices() const { return {}; }
    virtual std::vector<UINT> GetIndexes() const { return {}; }
    virtual UINT GetVertexCount() const { return 0; }
    virtual UINT GetIndexCount() const { return 0; }

    // Helpers para manejar la región/buffer del chunk (algunas implementaciones la exponen)
    virtual void SetRegion(const TerrainChunk::ChunkBufferRegion& /*region*/) { }
    virtual const TerrainChunk::ChunkBufferRegion& GetRegion() const { static TerrainChunk::ChunkBufferRegion dummy{}; return dummy; }

    // Flags / readiness
    virtual bool HasBuffersReady() const { return false; }
    virtual bool IsVisibleInZone(int /*zone*/) const { return true; }

    // Compatibilidad con llamadas que establecen LOD directamente
    virtual void SetCurrentLOD(int /*lod*/) { }
};
