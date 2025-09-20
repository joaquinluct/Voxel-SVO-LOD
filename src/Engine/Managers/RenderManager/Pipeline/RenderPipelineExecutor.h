// RenderPipelineExecutor.h
#pragma once

#include "Stages/InputAssemblyStage.h"
#include "Stages/OutputMergerStage.h"
#include "Stages/PixelShaderStage.h"
#include "Stages/RasterizerStage.h"
#include "Stages/VertexShaderStage.h"
#include <d3d11.h>
//#include <Defines/Pipeline.h>
#include <functional>
#include <Managers/DeviceManager.h>
#include <memory>
#include <Services/FrameStateService.h>
#include <unordered_map>
#include <utility>
#include <vector>
#include <wrl/client.h>

class ManagerLocator;

namespace RenderPipeline {

    class RenderPipelineExecutor {
    private:
        InputAssemblyStage* m_inputAssemblyStage;
        VertexShaderStage* m_vertexShaderStage;
        PixelShaderStage* m_pixelShaderStage;
        RasterizerStage* m_rasterizerStage;
        OutputMergerStage* m_outputMergerStage;

        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context; // Necesario para DrawIndexed, UpdateSubresource etc.
        Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain; // For Present (if OutputMergerStage doesn't handle it directly)

        // Lista de operaciones base
        std::unordered_map<PipelineOperationType, std::pair<std::shared_ptr<FrameStateService>, std::function<void(PipelineOperationType, FrameStateService*, PipelineOperBase*)>>> m_operationsMap = {};

        // Lista de operaciones a ejecutar en orden
        std::vector<PipelineOperation*> m_renderOperations = {};

        std::shared_ptr<DeviceManager> m_deviceManager;

        void RegisterOperations(FrameStateService* frameState);
    public:
        ~RenderPipelineExecutor() {};

        // Constructor: toma punteros a las instancias de las etapas.
        // Permite que las etapas sean gestionadas externamente (ej. por tu Renderer o RenderManager).
        RenderPipelineExecutor(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain);

        RenderPipelineExecutor(
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, // Main context
            Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain,   // For presentation
            InputAssemblyStage* iaStage,
            VertexShaderStage* vsStage,
            PixelShaderStage* psStage,
            RasterizerStage* rsStage,
            OutputMergerStage* omStage
        );

        void AddOperation(PipelineOperationType operationType);
        void ExecuteOperation(PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter);
        void Execute(FrameStateService* frameState);
        void ClearOperations();

        InputAssemblyStage* GetInputAssemblyStage() { return m_inputAssemblyStage; }
        VertexShaderStage* GetVertexShaderStage() { return m_vertexShaderStage; }
        PixelShaderStage* GetPixelShaderStage() { return m_pixelShaderStage; }
        RasterizerStage* GetRasterizerStage() { return m_rasterizerStage; }
        OutputMergerStage* GetOutputMergerStage() { return m_outputMergerStage; }
        // Ejecuta todas las operaciones en orden
        // Limpia todas las operaciones registradas
    };
}
