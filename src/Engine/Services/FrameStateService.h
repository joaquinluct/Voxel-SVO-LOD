#pragma once

#include <../Includes/FrameStateForward.h>
#include <atomic>
#include <Config/PassConfigBase.h>
#include <Defines/FrameStateDefinition.h>
#include <IService.h>
#include <map>
#include <memory>
#include <mutex>
#include <RenderManager/Pipeline/PipelineState.h>
#include <RenderState/FrameStates/FrameStateBase.h>
#include <string>
#include <string_view>
#include <vector>
#include <windows.h>

using namespace FrameStateDefinition;

class FrameStateService : public IService
{
private:
    bool m_initialized = false;
    std::shared_ptr<PassConfigBase> m_renderPassConfig;

    std::shared_ptr<PipelineStateManager> m_pipelineState;

    std::map<std::string_view, std::shared_ptr<FrameStateBuffer>> m_data;

    FrameStateBase* GetWriteState(FrameStateBuffer* buffer);
    FrameStateBase* GetReadState(FrameStateBuffer* buffer);

    //std::vector<std::unique_lock<std::mutex>> LockAll();
    //void UnlockAll(std::vector<std::unique_lock<std::mutex>>& locks); // Método opcional para desbloquear

public:
    FrameStateService();
    ~FrameStateService() override;
    // Constructor de copia
    /*std::shared_ptr<FrameStateService> Clone() {
      return std::make_shared<FrameStateService>();
    }*/


    HRESULT Init() override;
    void Shutdown() override;

    const std::string& GetServiceName() const override {
        static const std::string name = "FrameStateService";
        return name;
    }
    static const std::string& GetStaticServiceName() {
        static const std::string name = "FrameStateService";
        return name;
    }

    // Método a exitinguir heredados de la antigua RenderStateManager
    std::shared_ptr<PassConfigBase> GetRenderPassConfig() const { return m_renderPassConfig; }
    void SetRenderPassConfig(std::shared_ptr<PassConfigBase> config) { m_renderPassConfig = config; }

    // Métodos para manejar datos por hilo
    CameraFrameState* CameraState(bool FromReadBuffer = true);
    CommonFrameState* CommonState(bool FromReadBuffer = true);
    LightFrameState* LightingState(bool FromReadBuffer = true);
    MaterialFrameState* MaterialState(bool FromReadBuffer = true);
    SkyboxFrameState* SkyboxState(bool FromReadBuffer = true);
    WaterFrameState* WaterState(bool FromReadBuffer = true);
    TerrainFrameState* TerrainState(bool FromReadBuffer = true);
    TimeFrameState* TimeState(bool FromReadBuffer = true);
    //MeshFrameState* MeshState(bool FromReadBuffer = true);
    //DeviceFrameState* DeviceState(bool FromReadBuffer = true);
    PassFrameState* PassState(bool FromReadBuffer = true);
    ViewportFrameState* ViewportState(bool FromReadBuffer = true);
    ShaderFrameState* ShaderState(bool FromReadBuffer = true);
    PipelineFrameState* PipelineState(bool FromReadBuffer = true);
    RenderFrameState* RenderState(bool FromReadBuffer = true);

    // Operaciones genéricas
    std::shared_ptr<FrameStateBuffer> CreateState(std::string_view stateName);
    void SetData(std::string_view stateName, FrameStateBase* data);
    //void UpdateData(std::string_view stateName, FrameStateBase* data);

    void SetPipelineState(std::shared_ptr<PipelineStateManager> pipeline) { m_pipelineState = pipeline; }
    PipelineStateManager* GetPipelineState() const { return m_pipelineState.get(); }

    template<typename T>
    T* GetData(std::string_view stateName, bool FromReadBuffer = true) {
        auto it = m_data.find(stateName);
        if (it != m_data.end()) {
            FrameStateBuffer* threadData = it->second.get();
            if (FromReadBuffer) {
                return static_cast<T*>(threadData->buffers[threadData->readIndex.load()]);
            }
            if (!threadData->buffers[threadData->writeIndex.load()]) {
                threadData->buffers[threadData->writeIndex.load()] = threadData->buffers[threadData->readIndex.load()];
            }
            return static_cast<T*>(threadData->buffers[threadData->writeIndex.load()]);
        }
        CreateState(stateName);
        auto it2 = m_data.find(stateName);
        if (it2 != m_data.end()) {
            FrameStateBuffer* threadData = it2->second.get();
            int readIdx = threadData->readIndex.load();
            int writeIdx = threadData->writeIndex.load();
            if (FromReadBuffer) {
                threadData->buffers[readIdx] = new T();
                threadData->buffers[readIdx]->SetPipelineState(m_pipelineState);
                return static_cast<T*>(threadData->buffers[readIdx]);
            }
            threadData->buffers[writeIdx] = new T();
            threadData->buffers[writeIdx]->SetPipelineState(m_pipelineState);
            return static_cast<T*>(threadData->buffers[writeIdx]);
        }
        return nullptr;
        //throw std::runtime_error("State not found: " + std::string(stateName));
    };

    /*template<typename T>
    T& GetData(std::shared_ptr<FrameStateBuffer> threadData) {
        return static_cast<T&>(threadData->buffers[threadData->writeIndex]);
    }*/

    // Métodos para el doble buffering	
    void SwapBuffers();
    void SwapBuffer(std::string_view stateName);
    void SwapBuffersContent();

    FrameStateBuffer* GetBuffer(std::string_view stateName);

    // Métodos para bloqueo y desbloqueo explícito
    //void LockAll();
    std::vector<std::unique_lock<std::mutex>> LockAll();
    void UnlockAll(std::vector<std::unique_lock<std::mutex>>& locks);
    std::unique_lock<std::mutex> LockRenderState();
    void UnlockAll();
    void Lock(std::string_view stateName);
    void Unlock(std::string_view stateName);
};
