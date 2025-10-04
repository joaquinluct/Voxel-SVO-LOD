#include "ConstantsBufferFrameState.h"
#include <cstring>
#include <d3d11.h>
#include <Defines/Enums/Matrix.h>
#include <Defines/Matrix/MatrixDefinition.h>
#include <Defines/Structs/Pipeline/PipelineResources.h>
#include <variant>
#include <vector>
#include <Windows.h>
#include <wrl/client.h>

bool ConstantsBufferFrameState::HasOperations() const
{
    return !constantBuffers.empty();
}

void ConstantsBufferFrameState::ClearOperations()
{
    constantBuffers.clear();
}

std::vector<PipelineConstantBufferResource>& ConstantsBufferFrameState::GetConstantBuffers()
{
    return constantBuffers;
}

void ConstantsBufferFrameState::AddMapUnmapOperation(const PipelineConstantBufferResource& resource) {
    constantBuffers.push_back(resource);
}

void ConstantsBufferFrameState::MapUnmapOperation(ID3D11DeviceContext* context, ID3D11Buffer* buffer, const MatrixDefinition::AnyMatrixBuffer& data)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = context->Map(
        buffer,
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mappedResource
    );

    if (FAILED(hr)) {
        return;
    }

    std::visit([&mappedResource](auto matrix) {
        size_t size = matrix.Size();
        std::memcpy(mappedResource.pData, &matrix, size);
        }, data);


    context->Unmap(buffer, 0);
}

void ConstantsBufferFrameState::UpdateSubResourceOperation(ID3D11DeviceContext* context, ID3D11Buffer* buffer, const MatrixDefinition::AnyMatrixBuffer& data) {
    context->UpdateSubresource(buffer, 0, nullptr, &data, 0, 0);
}

void ConstantsBufferFrameState::ExecuteMapUnmapOperations(ID3D11DeviceContext* context, std::vector<PipelineConstantBufferResource>& constantBuffers)
{
    for (auto& cbResource : constantBuffers) {
        if (!cbResource.buffer) continue;
        ID3D11Buffer* res = cbResource.buffer.Get();
        if (!res) continue;

        if (cbResource.bufferType == MatrixBufferTypeEnum::Static) {
            UpdateSubResourceOperation(context, res, cbResource.data);
        }
        if (cbResource.bufferType == MatrixBufferTypeEnum::Dynamic) {
            MapUnmapOperation(context, res, cbResource.data);
        }
    }
}
