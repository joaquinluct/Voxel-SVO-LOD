// RenderPipelineExecutor.h
#pragma once

#include <d3d11.h>
#include <wrl/client.h> // For Microsoft::WRL::ComPtr
#include <string>       // For std::unique_ptr
#include <memory>       // For std::unique_ptr
#include <vector>       // For potential use with multiple buffers/views
#include <variant>

// Incluye las definiciones de tus operaciones y parámetros
#include <Defines/Pipeline.h>
#include <Game/Systems/RenderPipeline/Pipeline/InputAssemblyStage.h>
#include <Game/Systems/RenderPipeline/Pipeline/OutputMergerStage.h>
#include <Game/Systems/RenderPipeline/Pipeline/PixelShaderStage.h>
#include <Game/Systems/RenderPipeline/Pipeline/RasterizerStage.h>
#include <Game/Systems/RenderPipeline/Pipeline/VertexShaderStage.h>

// Puedes añadir una clase para operaciones de contexto directas como DrawIndexed si no están en una etapa específica
// #include "DeviceContextOperations.h" // If you have such a class

namespace RenderPipeline {

    class RenderPipelineExecutor {
    private:
        InputAssemblyStage* m_inputAssemblyStage;
        VertexShaderStage* m_vertexShaderStage;
        PixelShaderStage* m_pixelShaderStage;
        RasterizerStage* m_rasterizerStage;
        OutputMergerStage* m_outputMergerStage;
        // Si tienes una clase para operaciones directas del contexto (Draw, ClearState, UpdateSubresource)
        // Por ahora, integraré DrawIndexed directamente si no tienes esa clase separada.
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context; // Necesario para DrawIndexed, UpdateSubresource etc.
        Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain; // For Present (if OutputMergerStage doesn't handle it directly)

        std::shared_ptr<PipelineStore> m_pipelineStore;
    public:
        // Constructor: toma punteros a las instancias de las etapas.
        // Permite que las etapas sean gestionadas externamente (ej. por tu Renderer o RenderManager).
        RenderPipelineExecutor(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<PipelineStore> pipelineStore) {
			m_context = context;
            m_pipelineStore = pipelineStore;
			m_inputAssemblyStage = new InputAssemblyStage(context);
			m_vertexShaderStage = new VertexShaderStage(context);
			m_pixelShaderStage = new PixelShaderStage(context);
			m_rasterizerStage = new RasterizerStage(context);
			m_outputMergerStage = new OutputMergerStage(context);
        }

        RenderPipelineExecutor(
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, // Main context
            Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain,   // For presentation
            InputAssemblyStage* iaStage,
            VertexShaderStage* vsStage,
            PixelShaderStage* psStage,
            RasterizerStage* rsStage,
            OutputMergerStage* omStage
        );

        // Método para ejecutar una sola operación
        void ExecuteOperation(std::shared_ptr<PipelineOperation>& operation);
    };
}