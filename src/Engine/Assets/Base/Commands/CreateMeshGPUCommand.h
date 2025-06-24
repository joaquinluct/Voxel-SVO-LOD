// IGPUCreateCommand.h
#pragma once
#include "IGPUCreateCommand.h"
#include "Assets/Base/MeshAsset.h"

// Comando para crear una malla D3D11
template <typename TVertex>
class CreateMeshGPUCommand : public IGPUCreateCommand {
public:
    std::shared_ptr<MeshAsset> m_meshAsset;
    std::vector<TVertex> m_vertices;
    std::vector<WORD> m_indices;

	CreateMeshGPUCommand() = delete;
    
    CreateMeshGPUCommand(std::shared_ptr<MeshAsset> meshAsset, const std::vector<TVertex>& vertices, const std::vector<WORD>& indices)
        : m_meshAsset(meshAsset), m_vertices(vertices), m_indices(indices) {
    }

    HRESULT Execute(ID3D11Device* pDevice, ID3D11DeviceContext* pImmediateContext) override {
        if (!m_meshAsset) {
            OutputDebugStringA("WARNING: CreateMeshGPUCommand: Mesh asset is null or expired.\n");
            return E_FAIL;
        }
        return m_meshAsset->InitD3D11Resources(pDevice, m_vertices, m_indices);
    }
};