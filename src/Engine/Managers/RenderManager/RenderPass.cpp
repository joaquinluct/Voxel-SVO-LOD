#include "RenderPass.h"
#include <Assets/Base/MeshAsset.h>
#include <variant>

RenderPass::RenderPass() : 
	type(), id(0), name(""), active(false), debug(false)
{

}

RenderPass::~RenderPass() {

}

std::vector<std::shared_ptr<PipelineOperation>> RenderPass::BeginPass()
{
	return {};
}

std::vector<std::shared_ptr<PipelineOperation>> RenderPass::ExecPass(std::shared_ptr<MeshAsset> mesh)
{
	return {};
}

std::vector<std::shared_ptr<PipelineOperation>> RenderPass::EndPass()
{
	return {};
}

std::map<std::string, std::shared_ptr<MeshAsset>> RenderPass::GetMeshes(GameRenderManager* gameRenderManager)
{
	return {}; 
}

void RenderPass::Draw(std::shared_ptr<MeshAsset> mesh) {
	if (mesh->GetDrawType() == Mesh::DrawType::DrawIndexed) {
		PipelineDrawIndexedData drawData = {};
		drawData.numIndexes = mesh->GetIndexCount();
		AddOperation(PipelineOperationType::Device_drawIndexed, drawData);
	} else if (mesh->GetDrawType() == Mesh::DrawType::DrawIndexed) {
		PipelineDrawData drawData = {};
		drawData.vertexCount = mesh->GetVertexCount();
		AddOperation(PipelineOperationType::Device_draw, drawData);
	}
}