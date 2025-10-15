#pragma once

#include <Engine/Rendering/RenderCommand.h>
#include <Defines/Types/ThreadTypes.h>

class SceneManager;

// SceneSystem: non-threaded facade for scene management logic. During the
// migration SceneManager will create a SceneSystem and forward Update/Fill
// calls to it. SceneSystem delegates back to the owning SceneManager for the
// concrete implementation, enabling an incremental migration to a system.
class SceneSystem {
public:
    SceneSystem() = default;
    ~SceneSystem() = default;

    HRESULT Init(SceneManager* owner);
    HRESULT PostInit();
    void Shutdown();

    void Update(float deltaTime);
    void FillCommandBuffer(CommandBuffer& buffer);
    // Migration helpers: moved from SceneManager
    void CreatePassOperations();
    void CreateScene();
    // Migration: additional scene helper functions migrated from SceneManager
    void CreateMeshOperations(RenderPassResource* pass, MeshResource* meshResource);
    void CreateShaderOperations(std::string_view shaderName);
    void CreateShaderInitialOperations(ShaderResource* shaderResource);
    void CreateMainOperations();
    std::vector<MeshResource*> GetPassMeshes(RenderPassResource* pass);

private:
    SceneManager* m_owner = nullptr;
};
