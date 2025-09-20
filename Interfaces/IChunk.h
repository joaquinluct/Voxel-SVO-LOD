#pragma once
#include <Defines/TerrainChunk.h>
#include <DirectXMath.h>
#include <IMesh.h>
#include <Windows.h>

class MeshAsset;

// Interfaz base abstracta para cualquier tipo de chunk

class IChunk : public IMesh {
public:
    virtual ~IChunk() = default;

    // Métodos de ciclo de vida del chunk
    //virtual void Load() = 0;
    virtual HRESULT Init() = 0;
    virtual void Update() = 0;
    virtual void Render() = 0;
    virtual void Shutdown() = 0;

    // Métodos para acceder a la información del chunk
    virtual TerrainChunk::ChunkID GetID() const = 0;
    virtual DirectX::XMFLOAT3 GetPosition() const = 0;
    virtual int GetCurrentLOD() const = 0;
    virtual void UpdateLOD(int lodLevel) = 0;
    virtual void SetCurrentLOD(int lodLevel) = 0;

    //virtual void SetMesh(std::shared_ptr<MeshAsset> mesh) = 0;
    //virtual std::shared_ptr<MeshAsset> GetMesh() = 0;

    virtual bool IsDirty() const = 0;
    virtual void SetDirty(bool dirty) = 0;
    //virtual bool IsLoaded() const = 0;
};
