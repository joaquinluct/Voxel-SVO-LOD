// RenderPipelineExecutor.h
#pragma once

#include <d3d11.h>
#include <wrl/client.h> // For Microsoft::WRL::ComPtr
#include <string>       // For std::unique_ptr
#include <memory>       // For std::unique_ptr
#include <vector>       // For potential use with multiple buffers/views
#include <variant>
#include <unordered_map>
#include <map> // For storing pipeline states if needed

// Incluye las definiciones de tus operaciones y parámetros
#include <RenderManager/Pipeline/PipelineState.h>
#include <Defines/Pipeline.h>
#include "Stages/InputAssemblyStage.h"
#include "Stages/OutputMergerStage.h"
#include "Stages/PixelShaderStage.h"
#include "Stages/RasterizerStage.h"
#include "Stages/VertexShaderStage.h"

// Puedes añadir una clase para operaciones de contexto directas como DrawIndexed si no están en una etapa específica
// #include "DeviceContextOperations.h" // If you have such a class

class FrameStateService;

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

        // Lista de operaciones base
		std::unordered_map<PipelineOperationType, std::pair<std::shared_ptr<FrameStateService>, std::function<void(PipelineOperationType, FrameStateService*)>>> m_operationsMap = {};

		// Lista de operaciones a ejecutar en orden
        std::vector<PipelineOperation*> m_renderOperations = {};

		void RegisterOperations(FrameStateService* frameState);
    public:
        ~RenderPipelineExecutor() {};

        // Constructor: toma punteros a las instancias de las etapas.
        // Permite que las etapas sean gestionadas externamente (ej. por tu Renderer o RenderManager).
        RenderPipelineExecutor(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain) {
			m_context = context;
			m_swapChain = swapChain;
			m_inputAssemblyStage = new InputAssemblyStage(context);
			m_vertexShaderStage = new VertexShaderStage(context);
			m_pixelShaderStage = new PixelShaderStage(context);
			m_rasterizerStage = new RasterizerStage(context);
			m_outputMergerStage = new OutputMergerStage(context);
            RegisterOperations(nullptr);
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

        void AddOperation(PipelineOperationType operationType);
        void ExecuteOperation(PipelineOperationType operationType, FrameStateService* frameState);
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