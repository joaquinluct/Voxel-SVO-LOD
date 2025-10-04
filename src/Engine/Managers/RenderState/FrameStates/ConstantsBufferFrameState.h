#pragma once

#include "FrameStateBase.h"
#include <d3d11.h>
#include <Defines/Matrix/MatrixDefinition.h>
#include <Defines/Structs/Pipeline/PipelineResources.h>
#include <vector>
#include <wrl/client.h>

class ConstantsBufferFrameState : public FrameStateBase {
public:
    void AddMapUnmapOperation(const PipelineConstantBufferResource& resource);
    void ExecuteMapUnmapOperations(ID3D11DeviceContext* context, std::vector<PipelineConstantBufferResource>& constantBuffers);
    void MapUnmapOperation(ID3D11DeviceContext* context, ID3D11Buffer* buffer, const MatrixDefinition::AnyMatrixBuffer& data);
    void UpdateSubResourceOperation(ID3D11DeviceContext* context, ID3D11Buffer* buffer, const MatrixDefinition::AnyMatrixBuffer& data);
    bool HasOperations() const;
    void ClearOperations();
    std::vector<PipelineConstantBufferResource>& GetConstantBuffers();
private:
    std::vector<PipelineConstantBufferResource> constantBuffers;
};

