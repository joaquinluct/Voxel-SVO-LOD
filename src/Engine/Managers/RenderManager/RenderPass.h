#pragma once

#include <Windows.h>
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <Defines/Pipeline.h>
#include <SceneManager.h>
#include <Defines/Pass.h>
#include <IRenderPass.h>
#include <RenderManager/Pipeline/RenderPipelineExecutor.h>

class MeshAsset;
class FrameStateService;

class RenderPass : public IRenderPass
{
private:
    // Tipo de pase de renderizado
    RenderPassType type;
    // Identificador del pase de renderizado
    int id;
    // Nombre del pase de renderizado
    std::string name;
    // Indica si el pase está activo
    bool active = true;
    // Indica si el pase es de depuración
    bool debug = false;

    std::vector<PipelineOperationType> m_operations;
    std::vector<PipelineOperationType> m_initialOperations;

	FrameStateService* m_frameStateService;
    RenderPipeline::RenderPipelineExecutor* m_executor = nullptr;
public:
    RenderPass();
    RenderPass(const void* self) :type(), id(0), name("") {};
    RenderPass(RenderPassType type, int id, const std::string& name)
        : type(type), id(id), name(name) {
    };

    ~RenderPass() override;

    HRESULT Init() override { return S_OK; };
    std::vector<PipelineOperationType> BeginPass(const MeshAsset* mesh, FrameStateService* renderState) override;
    std::vector<PipelineOperationType> ExecPass(const MeshAsset* mesh, FrameStateService* renderState) override;
    std::vector<PipelineOperationType> EndPass() override;
	std::map<std::string, std::shared_ptr<MeshAsset>> GetMeshes(const SceneManager* SceneManager, FrameStateService* renderState) override;
    std::shared_ptr<PassConfigBase> GetConfig() const { return nullptr; }
    
    const int GetId() const { return id; }
     
    bool IsActive() const override { return active; }

    void Activate() override { active = true; }
    void Deactivate() override { active = false; }

    //void Draw(const MeshAsset* mesh, std::optional<PipelineParameter> pipelineParameter = std::nullopt);

    void AddOperation(PipelineOperationType operationType);
    void AddInitialOperation(PipelineOperationType operationType);

	void ClearOperations() { m_operations.clear(); };
    std::vector<PipelineOperationType> GetOperations() { return m_operations; };
    std::vector<PipelineOperationType> GetInitialOperations() { return m_initialOperations; };

	void SetFrameStateService(FrameStateService* frameStateService) { m_frameStateService = frameStateService; };    
    void SetExecutor(RenderPipeline::RenderPipelineExecutor* executor) { m_executor = executor; }
};
