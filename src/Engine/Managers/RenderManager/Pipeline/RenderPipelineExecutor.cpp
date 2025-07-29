#include "RenderPipelineExecutor.h"

namespace RenderPipeline {

    RenderPipelineExecutor::RenderPipelineExecutor(
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
        Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain,
        InputAssemblyStage* iaStage,
        VertexShaderStage* vsStage,
        PixelShaderStage* psStage,
        RasterizerStage* rsStage,
        OutputMergerStage* omStage
    ) :
        m_context(context),
        m_swapChain(swapChain),
        m_inputAssemblyStage(iaStage),
        m_vertexShaderStage(vsStage),
        m_pixelShaderStage(psStage),
        m_rasterizerStage(rsStage),
        m_outputMergerStage(omStage)
    {
        m_outputMergerStage = new OutputMergerStage(context);
    }

    void RenderPipelineExecutor::ExecuteOperation(std::shared_ptr<PipelineOperation>& operation) {

        PipelineOperationType operationType = operation->GetOperationType();

        switch (operationType) {
        case PipelineOperationType::Device_ClearRenderTargetView : {
            if (m_outputMergerStage) {
                PipelineRenderTargetViewData rtvData = operation->GetOperationParam<PipelineRenderTargetViewData>();
                DirectX::XMFLOAT4 cColor = rtvData.clearColor;
                const FLOAT* clearColor = new FLOAT[4]{ cColor.x, cColor.y, cColor.z, cColor.w };
                m_outputMergerStage->ClearRenderTargetView(rtvData.data, clearColor);
                delete[] clearColor;
                operation->SetResult(S_OK);
            }
            break;
        }
        }
        /*
        case PipelineOperationType::Mesh_Render_SetInputLayout: {
            if (m_inputAssemblyStage != nullptr) {
                auto layoutData = operation->GetOperationParam<PipelineLayoutData>();
                if (layoutData && layoutData->data) {
                    m_inputAssemblyStage->SetInputLayout(layoutData->data);
                }
            }
            break;
        }

        case PipelineOperationType::Mesh_Render_SetVertexBuffer: {
            if (m_inputAssemblyStage) {
                auto vertexData = operation->GetOperationParam<PipelineSetVertexBufferData>();
                if (vertexData && vertexData->vertexBuffer) {
                    Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer = vertexData->vertexBuffer;
                    m_inputAssemblyStage->SetVertexBuffers(
                        vertexData->startSlot,
                        vertexData->numBuffers,
                        pBuffer.Get(),
                        &vertexData->stride,
                        &vertexData->offset
                    );
                }
            }
            break;
        }
        case PipelineOperationType::Mesh_Render_SetIndexBuffer: {
            if (m_inputAssemblyStage) {
                auto indexData = operation->GetOperationParam<PipelineSetIndexBufferData>();
                if (indexData && indexData->indexBuffer) {
                    m_inputAssemblyStage->SetIndexBuffer(
                        indexData->indexBuffer,
                        indexData->format,
                        indexData->offset
                    );
                }
            }
            break;
        }
        case PipelineOperationType::Mesh_Render_SetPrimitiveToplogy: {
            if (m_inputAssemblyStage) {
                auto topoData = operation->GetOperationParam<PipelinePrimitiveTopologyData>();
                if (topoData) {
                    m_inputAssemblyStage->SetPrimitiveTopology(topoData->data);
                }
            }
            break;
        }
        case PipelineOperationType::Mesh_Render_SetVertexShader:
        case PipelineOperationType::Device_Init_SetVertexShader: {
            if (m_vertexShaderStage) {
                auto shaderData = operation->GetOperationParam<PipelineVertexShaderData>();
                if (shaderData && shaderData->data) {
                    m_vertexShaderStage->SetShader(shaderData->data);
                }
            }
            break;
        }
        case PipelineOperationType::Mesh_Render_SetPixelShader:
        case PipelineOperationType::Device_Init_SetPixelShader: {
            if (m_pixelShaderStage) {
                auto shaderData = operation->GetOperationParam<PipelinePixelShaderData>();
                if (shaderData && shaderData->data) {
                    m_pixelShaderStage->SetShader(shaderData->data);
                }
            }
            break;
        }
        case PipelineOperationType::Mesh_Render_SetTexture: {
            if (m_pixelShaderStage) {
                auto textureData = operation->GetOperationParam<PipelineTextureData>();
                if (textureData && textureData->data.size()) {
                    std::vector<ID3D11ShaderResourceView*> textures = {};
                    for (auto texturePair : textureData->data) {
                        textures.push_back(texturePair.second);
                    }
                    m_pixelShaderStage->SetShaderResources(textureData->startSlot, textureData->numTextures, textures.data());
                }
            }
            break;
        }
        case PipelineOperationType::Mesh_Render_SetSampler: {
            if (m_pixelShaderStage) {
                auto samplerData = operation->GetOperationParam<PipelineSamplerSateData>();
                if (samplerData && samplerData->data) {
                    Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler = samplerData->data;
                    m_pixelShaderStage->SetSamplers(samplerData->startSlot, samplerData->numSamplers, &pSampler);
                }
            }
            break;
        }
        case PipelineOperationType::Device_SetRasterizedState:
        case PipelineOperationType::Device_Init_RasterizedState: {
            if (m_rasterizerStage) {
                auto rasterData = operation->GetOperationParam<PipelineRasteriezeData>();
                if (rasterData && rasterData->state) {
                    m_rasterizerStage->SetState(rasterData->state);
                }
            }
            break;
        }
        case PipelineOperationType::Device_SetViewport: {
            if (m_rasterizerStage) {
                auto viewportData = operation->GetOperationParam<PipelineViewPortData>();
                if (viewportData) {
                    m_rasterizerStage->SetViewport(viewportData->desc);
                }
            }
            break;
        }
        case PipelineOperationType::Device_ClearRenderTargetView: {
            if (m_outputMergerStage) {
                auto rtvData = operation->GetOperationParam<PipelineRenderTargetViewData>();
                if (rtvData && rtvData->data) {
                    const FLOAT* clearColor = new FLOAT[4]{ rtvData->clearColor.x, rtvData->clearColor.y, rtvData->clearColor.z, rtvData->clearColor.w };
                    m_outputMergerStage->ClearRenderTargetView(rtvData->data, clearColor);
                    delete[] clearColor;
                }
            }
            break;
        }
        case PipelineOperationType::Device_ClearDepthStencilView: {
            if (m_outputMergerStage) {
                auto stencilData = operation->GetOperationParam<PipelineDeepStencilData>();
                if (stencilData && stencilData->data) {
                    m_outputMergerStage->ClearDepthStencilView(
                        stencilData->data, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
                }
            }
            break;
        }
        case PipelineOperationType::Device_SetDepthStencilState: {
            break;
        }
        case PipelineOperationType::Device_EnabledBledingState:
        case PipelineOperationType::Device_DisabledBledingState:
        case PipelineOperationType::Device_Init_BledingState: {
            break;
        }
        case PipelineOperationType::Device_PresentSwapChain: {
            if (m_outputMergerStage) {
                auto swapChainData = operation->GetOperationParam<PipelinePresebtSwapChain>();
                if (swapChainData) {
                    m_outputMergerStage->Present(swapChainData->data, swapChainData->syncInterval, swapChainData->flags);
                }
            }
            break;
        }
        case PipelineOperationType::Device_drawIndexed: {
            if (m_context) {
                auto drawData = operation->GetOperationParam<PipelineDrawIndexedData>();
                if (drawData) {
                    m_context->DrawIndexed(drawData->numIndexes, 0, 0);
                }
            }
            break;
        }
        case PipelineOperationType::Device_ResetContextState: {
            if (m_context) {
                m_context->ClearState();
            }
            break;
        }
        case PipelineOperationType::Mesh_Render_BindConstantsBuffers:
        case PipelineOperationType::Mesh_Init_SamplerState:
        case PipelineOperationType::Mesh_Init_InputLayout:
        case PipelineOperationType::Mesh_Init_MatrixBuffer:
        case PipelineOperationType::Mesh_Render_RestoreState:
        case PipelineOperationType::Mesh_Render_SaveState:
        case PipelineOperationType::Device_draw:
        case PipelineOperationType::Unknown:
        case PipelineOperationType::Count:
            break;
        }*/
    }
}