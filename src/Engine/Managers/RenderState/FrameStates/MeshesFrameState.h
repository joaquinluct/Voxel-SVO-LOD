#pragma once

#include "FrameStateBase.h"
#include <d3d11.h>
#include <Defines/Structs/PipelineResources.h>
#include <vector>

class MeshesFrameState : public FrameStateBase {
public:
    void AddMeshesOperation(std::vector<MeshResource*>& meshes);    
    void ExecuteMapUnmapOperations(ID3D11DeviceContext* context);
    bool HasOperations() const;
    void ClearOperations();
private:
    std::vector<MeshResource*> meshesResources;
};

