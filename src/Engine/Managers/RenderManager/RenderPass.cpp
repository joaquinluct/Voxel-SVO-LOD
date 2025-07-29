#include "RenderPass.h"

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