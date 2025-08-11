#pragma once

#include <Windows.h>
#include <vector>
#include <map>
#include <memory>
#include <IInitializable.h>
#include <Defines/Pipeline.h>
//#include <Managers/RenderManager/GameRenderManager.h>

class GameRenderManager;
class MeshAsset;

class IRenderPass : public IInitializable
{
public:
	virtual ~IRenderPass() = default;
	virtual std::vector<std::shared_ptr<PipelineOperation>> BeginPass() = 0;
	virtual std::vector<std::shared_ptr<PipelineOperation>> ExecPass(std::shared_ptr<MeshAsset> mesh) = 0;
	virtual std::vector<std::shared_ptr<PipelineOperation>> EndPass() = 0;
	virtual std::map<std::string, std::shared_ptr<MeshAsset>> GetMeshes(GameRenderManager* gameRenderManager) = 0;
	virtual void Activate() = 0;
	virtual void Deactivate() = 0;
	virtual bool IsActive() const = 0;
};
