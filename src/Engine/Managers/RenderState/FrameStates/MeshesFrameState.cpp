#include "MeshesFrameState.h"
#include <Assets/Base/MeshAssetBase.h>
#include <cstdint>
#include <cstring>
#include <d3d11.h>
#include <Defines/Structs/PipelineResources.h>
#include <utility>
#include <vector>
#include <Windows.h>

bool MeshesFrameState::HasOperations() const
{
    return !meshesResources.empty();
}

void MeshesFrameState::ClearOperations()
{
    meshesResources.clear();
}


void MeshesFrameState::AddMeshesOperation(std::vector<MeshResource*>& meshes) {
    meshesResources = meshes;
}

void MeshesFrameState::ExecuteMapUnmapOperations(ID3D11DeviceContext* context)
{
    for (MeshResource* meshResource : meshesResources) {
        MeshAssetBase* mesh = meshResource->mesh;

        int index = mesh->GetReadIndex();

        ID3D11Buffer* vB = mesh->GetVertexBuffer(index).Get();
        ID3D11Buffer* iB = mesh->GetIndexBuffer(index).Get();

        if (!vB || !iB) continue;

        /*std::vector<uint8_t> vertexData = std::move(mesh->GetVertexData());
        std::vector<uint16_t> indexData = std::move(mesh->GetIndexData());*/

        if (mesh->GetVertexData(index).empty() || mesh->GetIndexData(index).empty()) continue;

        D3D11_MAPPED_SUBRESOURCE mappedVertices;
        D3D11_MAPPED_SUBRESOURCE mappedIndices;

        HRESULT hr = context->Map(vB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertices);
        if (FAILED(hr)) return;
        hr = context->Map(iB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIndices);
        if (FAILED(hr)) {
            context->Unmap(vB, 0);
            return;
        }

        memcpy(mappedVertices.pData, mesh->GetVertexData(index).data(), mesh->GetVertexData(index).size());
        memcpy(mappedIndices.pData, mesh->GetIndexData(index).data(), mesh->GetIndexData(index).size() * sizeof(uint16_t));

        context->Unmap(vB, 0);
        context->Unmap(iB, 0);
    }
}
