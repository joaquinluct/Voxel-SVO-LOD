#include "UpdateConstantBuffersJob.h"
#include <Defines/Contants/FrameState.h>
#include <Defines/Matrix/Light.h>
#include <Defines/Matrix/MaterialMatrix.h>
#include <Defines/Matrix/MatrixDefinition.h>
#include <Defines/Matrix/MatrixDefinitionBase.h>
#include <Defines/Structs/PipelineResources.h>
#include <Defines/Types/ThreadTypes.h>
#include <DirectXMath.h>
#include <Game/Systems/Terrain.h>
#include <Game/Systems/World.h>
#include <IJob.h>
#include <Managers/RenderState/FrameStates/ConstantsBufferFrameState.h>
#include <map>
#include <memory>
#include <Services/FrameStateService.h>
#include <string>
#include <variant>
#include <vector>

static bool IsContantBuffersFirstTime = true;

bool UpdateConstantBuffersJob::Execute(JobContext* context)
{
    //if (isProcessing || !context->frameStateService->TryLock(FRAME_STATE_CONSTANT_BUFFERS)) {
    if (isProcessing) {
        return false;
    }
    isProcessing = true;

    // Ejectuar el padre, que hace una pequeña pausa
    // para dar tiempo al job a encolarse antes de resolverse
    IJob::Execute(context);
    std::vector<ShaderResource*> shaders = context->shaders;

    if (!shaders.size()) {
        isProcessing = false;
        return false;
    }

    float farPlane = context->cameraManager->GetCurrentCamera()->GetFarPlane();

    MatrixParams matrixParams = {};
    matrixParams.cameraPosition = context->cameraManager->GetCurrentCameraPosition();
    matrixParams.viewMatrix = XMMatrixTranspose(context->cameraManager->GetCurrentViewMatrix());
    matrixParams.projectionMatrix = XMMatrixTranspose(context->cameraManager->GetCurrentProjectionMatrix());
    matrixParams.worldMatrix = DirectX::XMMatrixIdentity();
    //matrixParams.timeData = context->chronos->GetCurrentDateTime();
    LightMatrixParams lightParams = {};
    lightParams.lightColor = context->ligting->GetLightColor();
    lightParams.lightDirection = context->ligting->GetLightDirection();
    lightParams.fogStartDistance = farPlane - (farPlane / 1.1); // Empieza a desvanecer la geometría
    lightParams.fogEndDistance = farPlane; // Totalmente gris, ocultando el fallo del LOD
    //lightParams.fogDensity = -100.0f; // Controla la densidad general de la niebla
    lightParams.fogDensity = -10000.0f; // Para que no aparezca en el suelo
    lightParams.fogColor = XMFLOAT3(0.1f, 0.1f, 0.11f);
    lightParams.fogHeightFalloff = 0.2f; // Pequeño valor (ej. 0.05 a 0.5) para controlar la rapidez con la que se disipa la niebla hacia el cielo.
    /*Light::DirectionalLight dLightParams = {};
    dLightParams.Color = context->ligting->GetLightColor();
    dLightParams.Direction = context->ligting->GetLightDirection();*/
    MaterialMatrixParams materialParams = {};
    materialParams.materialAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    materialParams.materialRoughness = 1.0f;
    materialParams.materialMetallic = 0.01f;
    materialParams.materialAO = 0.7f;
    materialParams.materialF0 = XMFLOAT3(0.04f, 0.04f, 0.04f);
    //materialParams.terrainBlenderData = context->world->GetTerrain()->GetTerrainBlenderData();
    //materialParams.terrain2BlenderData = context->world->GetTerrain()->GetTerrain2BlenderData();
    materialParams.textureTransform = XMFLOAT4(0.03f, 0.03f, 0.0f, 0.0f);
    //materialParams.textureTransform = DirectXUtils::ToXMFLOAT4(context->world->GetTerrain()->GetTerrainMesh()->GetTextureTransforms());
    WaterMatrixParams waterParams = {};
    waterParams.waterData = {};
    waterParams.waterData.waterColor = XMFLOAT4(0.0f, 0.3f, 0.5f, 1.0f);

    //waterParams.waterInstancingData = context->water->


    std::map<std::string, std::shared_ptr<IMatrixParams>> params{};

    params["BaseParams"] = std::make_shared<MatrixParams>(matrixParams);
    params["LightParams"] = std::make_shared<LightMatrixParams>(lightParams);
    params["MaterialParams"] = std::make_shared<MaterialMatrixParams>(materialParams);
    params["WaterParams"] = std::make_shared<WaterMatrixParams>(waterParams);

    while (context->frameStateService->IsRendering()) {
    }

    for (auto& shader : shaders) {
        auto& cBuffers = shader->constantBuffers;
        if (!cBuffers.size()) {
            continue;
        }
        bool hasOperations = false;
        for (auto& cBuffer : cBuffers) {
            std::string name = cBuffer.first;
            PipelineConstantBufferResource resource = cBuffer.second;
            std::visit([&](auto m) {
                size_t size = m.Size();
                m.SetMatrixData(params);
                resource.data = m;
                if (!hasOperations) {
                    context->frameStateService->ConstantBuffersState(false)->ClearOperations();
                    hasOperations = true;
                }
                context->frameStateService->ConstantBuffersState(false)->AddMapUnmapOperation(resource);
                }, resource.data);
        }
    }

    if (IsContantBuffersFirstTime) {
        context->frameStateService->SwapBufferContent(FRAME_STATE_CONSTANT_BUFFERS);
        IsContantBuffersFirstTime = false;
    }
    else {
        context->frameStateService->SwapBuffer(FRAME_STATE_CONSTANT_BUFFERS);
    }


    isProcessing = false;
    return true;
}
