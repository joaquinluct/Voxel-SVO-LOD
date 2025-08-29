#pragma once

#include <DirectXMath.h>
#include <memory>
#include <vector>
#include "FrameStateBase.h"
#include <Util/DateTime.h>
#include <Defines/TimeDefinition.h>
#include <Defines/Mesh.h>
#include <Assets/Base/MeshAsset.h>

struct PipelineSetVertexBufferData;

class MeshFrameState : public FrameStateBase {
public:	
	void SetNumberOfInstances(UINT count) { numberOfInstances = count; }
	UINT GetNumberOfInstances() const { return numberOfInstances; }
	const MeshAsset* GetMesh() const { return meshAsset; }
	void SetMeshAsset(const MeshAsset* asset) { meshAsset = asset; }
private:
	const MeshAsset* meshAsset = nullptr;
	PipelineSetVertexBufferData* vertexBufferData = nullptr;	
	PipelineSetIndexBufferData* indexBufferData = nullptr;
	UINT numberOfInstances = 0;
};

