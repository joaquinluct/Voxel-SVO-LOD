#pragma once

#include "FrameStateBase.h"
#include <Assets/Base/MeshAsset.h>
#include <vector>

class MeshFrameState : public FrameStateBase {
public:
    MeshFrameState() : FrameStateBase(), m_currentMeshIndex(0), m_currentMesh(nullptr), m_meshes{}, meshAsset{ nullptr } {}
    void SetNumberOfInstances(UINT count) { numberOfInstances = count; }
    UINT GetNumberOfInstances() const { return numberOfInstances; }
    MeshAssetBase* GetNextMesh() {
        m_currentMeshIndex++;
        return GetMesh();
    }
    MeshAssetBase* GetMesh() {
        if (m_meshes.size() == 0) return nullptr;
        if (m_currentMeshIndex > m_meshes.size() + 1) m_currentMeshIndex = 0;
        return m_meshes[m_currentMeshIndex];
    }
    void AddMesh(MeshAssetBase* mesh) { m_meshes.push_back(mesh); }
    MeshAssetBase* GetCurrentMesh() { return GetMesh(); }
    void ClearMeshes() { m_meshes.clear(); m_currentMeshIndex = 0; }
    bool HasMeshes() { return m_meshes.size(); }
private:
    int m_currentMeshIndex = -1;
    MeshAssetBase* m_currentMesh;
    std::vector<MeshAssetBase*> m_meshes{};
    MeshAsset* meshAsset = nullptr;
    PipelineSetVertexBufferData* vertexBufferData = nullptr;
    PipelineSetIndexBufferData* indexBufferData = nullptr;
    UINT numberOfInstances = 0;
};

