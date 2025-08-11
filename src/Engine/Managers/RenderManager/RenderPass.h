#pragma once

#include <string>
#include <vector>
#include <memory>
#include <IRenderPass.h>
#include <Defines/Pass.h>
#include <Defines/Pipeline.h>
#include <Managers/RenderManager/GameRenderManager.h>

class MeshAsset;

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

    std::vector<std::shared_ptr<PipelineOperation>> m_operations;
    std::vector<std::shared_ptr<PipelineOperation>> m_initialOperations;
public:
    RenderPass();
    RenderPass(const void* self) :type(), id(0), name("") {};
    RenderPass(RenderPassType type, int id, const std::string& name)
        : type(type), id(id), name(name) {
    };

    ~RenderPass() override;

    HRESULT Init() override { return S_OK; };
    std::vector<std::shared_ptr<PipelineOperation>> BeginPass() override;
    std::vector<std::shared_ptr<PipelineOperation>> ExecPass(std::shared_ptr<MeshAsset> mesh) override;
    std::vector<std::shared_ptr<PipelineOperation>> EndPass() override;
	std::map<std::string, std::shared_ptr<MeshAsset>> GetMeshes(GameRenderManager* gameRenderManager) override;

    const int GetId() const { return id; }
     
    bool IsActive() const override { return active; }

    void Activate() override { active = true; }
    void Deactivate() override { active = false; }

    void Draw(std::shared_ptr<MeshAsset> mesh);

    int AddOperation(const PipelineOperation& operation) {
        m_operations.push_back(std::make_shared<PipelineOperation>(operation));
        return operation.GetPriority();
    }
    int AddOperation(PipelineOperationType operationType, PipelineParameter operationParam = {}, PipelineData operationOutData =
        {}) {
        PipelineOperation* operation = new PipelineOperation(operationType, operationParam, operationOutData, 0);

        std::shared_ptr<PipelineOperation> oper = std::make_shared<PipelineOperation>(operation);

        m_operations.push_back(std::make_shared<PipelineOperation>(operation));

        return operation->GetPriority();
    }
    int AddInitialOperation(PipelineOperationType operationType, PipelineParameter operationParam = {}) {
        PipelineOperation* operation = new PipelineOperation(operationType, operationParam);
        std::shared_ptr<PipelineOperation> oper = std::make_shared<PipelineOperation>(operation);
        m_initialOperations.push_back(std::make_shared<PipelineOperation>(operation));
        return operation->GetPriority();
    }

	void ClearOperations() { m_operations.clear(); };
    std::vector<std::shared_ptr<PipelineOperation>> GetOperations() { return m_operations; };
    std::vector<std::shared_ptr<PipelineOperation>> GetInitialOperations() { return m_initialOperations; };
};
