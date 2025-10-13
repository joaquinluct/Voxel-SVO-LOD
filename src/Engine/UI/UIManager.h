#pragma once

#include <memory>
#include <Engine/Rendering/RenderCommand.h>
#include <Engine/Core/EngineContext.h>
#include <Managers/ManagerBase.h>

class UICanvas;

class UIManager : public ManagerBase {
public:
    UIManager();
    ~UIManager() override;

    HRESULT Init(EngineContext* context) override;
    void Shutdown() override;
    void Update(float dt) override;

    void FillCommandBuffer(CommandBuffer& buffer);

private:
    std::shared_ptr<UICanvas> m_root;
};
