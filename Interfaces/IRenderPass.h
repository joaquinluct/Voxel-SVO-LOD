#pragma once

//#include <Defines/Pipeline.h>
#include <IInitializable.h>
#include <Managers/RenderManager/Pipeline/ConcreteOperations.h>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <Windows.h>
//#include <SceneManager.h>

class FrameStateService;
class SceneManager;
class MeshAsset;
class PassConfigBase;

class IRenderPass : public IInitializable
{
private:
    std::string m_name;
public:
    virtual ~IRenderPass() = default;
    virtual std::vector<PipelineOperationType> BeginPass(const MeshAsset* mesh, FrameStateService* renderState) = 0;
    virtual std::vector<PipelineOperationType> ExecPass(const MeshAsset* mesh, FrameStateService* renderState) = 0;
    virtual std::vector<PipelineOperationType> EndPass() = 0;
    virtual std::map<std::string, std::shared_ptr<MeshAsset>> GetMeshes(const SceneManager* SceneManager, FrameStateService* renderState) = 0;
    virtual void Activate() = 0;
    virtual void Deactivate() = 0;
    virtual bool IsActive() const = 0;
    virtual std::string GetName() const { return m_name; }
    virtual void SetName(const std::string& name) { m_name = name; }

    // Mesh


    virtual std::shared_ptr<PassConfigBase> GetConfig() const = 0;
};
