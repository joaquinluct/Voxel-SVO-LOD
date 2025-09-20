#include "RenderPass.h"
#include <Assets/Base/MeshAsset.h>
#include <Defines/Mesh.h>
#include <Services/FrameStateService.h>

RenderPass::RenderPass() : type(), id(0), name(""), active(false), debug(false), m_frameStateService{}, m_initialOperations{}, m_operations{} {}

RenderPass::~RenderPass() {

}

std::vector<PipelineOperationType> RenderPass::BeginPass(const MeshAsset* mesh, FrameStateService* renderState)
{
    return {};
}

std::vector<PipelineOperationType> RenderPass::ExecPass(const MeshAsset* mesh, FrameStateService* renderState)
{
    return {};
}

std::vector<PipelineOperationType> RenderPass::EndPass()
{
    return {};
}

std::map<std::string, std::shared_ptr<MeshAsset>> RenderPass::GetMeshes(const SceneManager* SceneManager, FrameStateService* renderState)
{
    return {};
}

void RenderPass::AddOperation(PipelineOperationType operationType) {
    //m_operations.push_back(operationType);	
    //m_executor->ExecuteOperation(operationType, m_frameStateService, {});
}
void RenderPass::AddInitialOperation(PipelineOperationType operationType) {
    //m_initialOperations.push_back(operationType);
    //m_executor->ExecuteOperation(operationType, m_frameStateService, {});
}

inline static PipelineOperationType GetOperationTypeFromDrawType(Mesh::DrawType drawType) {
    switch (drawType) {
    case Mesh::DrawType::DrawIndexed:
        return PipelineOperationType::Draw_DrawIndexed;
    case Mesh::DrawType::Draw:
        return PipelineOperationType::Draw_Draw;
    case Mesh::DrawType::DrawInstanced:
        return PipelineOperationType::Draw_DrawInstanced;
    default:
        return PipelineOperationType::Draw_DrawIndexed; // Valor por defecto
    }
}

//void RenderPass::Draw(const MeshAsset* mesh, std::optional<PipelineParameter> pipelineParameter) {
//
//	PipelineOperationType operation = GetOperationTypeFromDrawType(mesh->GetDrawType());
//
//	if (!pipelineParameter.has_value()) {
//		if (mesh->GetDrawType() == Mesh::DrawType::DrawIndexed) {
//			PipelineDrawIndexedData drawData = {};
//			drawData.numIndexes = mesh->GetIndexCount();
//			pipelineParameter = drawData;
//		} else if (mesh->GetDrawType() == Mesh::DrawType::Draw) {
//			PipelineDrawData drawData = {};
//			drawData.vertexCount = mesh->GetVertexCount();
//			pipelineParameter = drawData;
//		}
//		else if (mesh->GetDrawType() == Mesh::DrawType::DrawInstanced) {
//			PipelineDrawInstancedData drawData = {};
//			drawData.instanceCount = mesh->GetInstanceCount();
//			drawData.vertexCountPerInstance = mesh->GetVertexCount();
//			drawData.indexCountPerInstance = mesh->GetIndexCount();
//			pipelineParameter = drawData;
//		}
//	}
//
//	if (pipelineParameter.has_value()) {
//		AddOperation(operation, pipelineParameter.value());
//	} else {
//		// Manejo de error en caso de que pipelineParameter siga sin valor
//		OutputDebugStringA("ERROR: pipelineParameter no tiene un valor válido.\n");
//	}
//}
