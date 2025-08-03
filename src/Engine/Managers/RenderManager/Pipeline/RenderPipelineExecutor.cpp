#include "RenderPipelineExecutor.h"
#include "Services/Material.h"
#include "Defines/Matrix/MatrixDefinitionBase.h"

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
        case PipelineOperationType::Device_ClearRenderTargetView: {
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
        case PipelineOperationType::Device_ClearDepthStencilView: {
            if (m_outputMergerStage) {
                PipelineDepthStencilData stencilData = operation->GetOperationParam<PipelineDepthStencilData>();
                if (stencilData.data) {
                    /*m_outputMergerStage->ClearDepthStencilView(
                        stencilData.data, stencilData.clearFlags, 1.0f, 0);*/
                    m_outputMergerStage->ClearDepthStencilView(
                        stencilData.data, stencilData.clearFlags, stencilData.depth, stencilData.stencil);
                    operation->SetResult(S_OK);
                }
            }
            break;
        }
        case PipelineOperationType::Device_SetViewport: {
            if (m_rasterizerStage) {
                PipelineViewPortData viewportData = operation->GetOperationParam<PipelineViewPortData>();
                if (viewportData.desc.Height) {
                    m_rasterizerStage->SetViewport(viewportData.desc);
                    operation->SetResult(S_OK);
                }
            }
            break;
        }
        case PipelineOperationType::Device_PresentSwapChain: {
            if (m_outputMergerStage) {
                PipelinePresentSwapChain swapChainData = operation->GetOperationParam<PipelinePresentSwapChain>();
                if (swapChainData.data) {
                    m_outputMergerStage->Present(swapChainData.data, swapChainData.syncInterval, swapChainData.flags);
                    operation->SetResult(S_OK);
                }
            }
            break;
        }
        case PipelineOperationType::Mesh_Render_SetVertexBuffer: {
            if (m_inputAssemblyStage) {
                auto vertexData = operation->GetOperationParam<PipelineSetVertexBufferData>();
                if (vertexData.vertexBuffer) {
                    //Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer = vertexData.vertexBuffer;
                    m_inputAssemblyStage->SetVertexBuffers(
                        vertexData.startSlot,
                        vertexData.numBuffers,
                        vertexData.vertexBuffer,
                        //pBuffer,
                        &vertexData.stride,
                        &vertexData.offset
                    );
                    operation->SetResult(S_OK);
                }
            }
            break;
        }
        case PipelineOperationType::Mesh_Render_SetIndexBuffer: {
            if (m_inputAssemblyStage) {
                auto indexData = operation->GetOperationParam<PipelineSetIndexBufferData>();
                if (indexData.indexBuffer) {
                    m_inputAssemblyStage->SetIndexBuffer(
                        indexData.indexBuffer,
                        indexData.format,
                        indexData.offset
                    );
                    operation->SetResult(S_OK);
                }
            }
            break;
        }
        case PipelineOperationType::Device_Init_SetRenderTargetView: {
            if (m_outputMergerStage) {
                auto rtvData = operation->GetOperationParam<PipelineSetRenderTargetsData>();
                if (rtvData.targetView) {
                    ID3D11RenderTargetView* tView = rtvData.targetView.Get();
                    ID3D11DepthStencilView* sView = rtvData.stencilView.Get();
                    bool isColorPass = rtvData.isColorPass;
					m_outputMergerStage->SetRenderTargets( isColorPass ? 1 : 0, &tView, sView);
                    operation->SetResult(S_OK);
                }
            }
			break;
        }
        case PipelineOperationType::Device_SetRasterizedState:
        case PipelineOperationType::Device_Init_RasterizedState: {
            if (m_rasterizerStage) {
                auto rasterData = operation->GetOperationParam<PipelineRasteriezeData>();
                if (rasterData.state) {
                    m_rasterizerStage->SetState(rasterData.state);
                }
            }
            break;
        }
        case PipelineOperationType::Mesh_Render_SetVertexShader:
        case PipelineOperationType::Device_Init_SetVertexShader: {
            if (m_vertexShaderStage) {
                auto shaderData = operation->GetOperationParam<PipelineVertexShaderData>();
                if (shaderData.data) {
                    m_vertexShaderStage->SetShader(shaderData.data);
                    operation->SetResult(S_OK);
                }
            }
            break;
        }
        case PipelineOperationType::Mesh_Render_SetPixelShader:
        case PipelineOperationType::Device_Init_SetPixelShader: {
            if (m_pixelShaderStage) {
                auto shaderData = operation->GetOperationParam<PipelinePixelShaderData>();
                if (shaderData.data) {
                    m_pixelShaderStage->SetShader(shaderData.data);
                    operation->SetResult(S_OK);
                }
            }
            break;
        }
        case PipelineOperationType::Mesh_Render_SetInputLayout: {
            if (m_inputAssemblyStage != nullptr) {
                auto layoutData = operation->GetOperationParam<PipelineLayoutData>();
                if (layoutData.data) {
                    m_inputAssemblyStage->SetInputLayout(layoutData.data);
                    operation->SetResult(S_OK);
                }
            }
            break;
        }
        case PipelineOperationType::Mesh_Render_SetTexture: {
            if (m_pixelShaderStage) {
                auto textureData = operation->GetOperationParam<PipelineTextureData>();
                if (textureData.data.size()) {
                    m_pixelShaderStage->SetShaderResources(textureData.startSlot, textureData.numTextures, textureData.data.data());
                }
            }
            break;
        }
        case PipelineOperationType::Mesh_Render_SetSampler: {
            if (m_pixelShaderStage) {
                auto samplerData = operation->GetOperationParam<PipelineSamplerSateData>();
                if (samplerData.data) {
                    ID3D11SamplerState* samplerStates[] = { samplerData.data.Get() };
                    m_pixelShaderStage->SetSamplers(samplerData.startSlot, samplerData.numSamplers, samplerStates);
                }
            }
            break;
        }
        case PipelineOperationType::Mesh_Render_SetPrimitiveToplogy: {
            if (m_inputAssemblyStage) {
                auto topoData = operation->GetOperationParam<PipelinePrimitiveTopologyData>();
                if (topoData.data) {
                    m_inputAssemblyStage->SetPrimitiveTopology(topoData.data);
                }
            }
            break;
        }
        case PipelineOperationType::Device_drawIndexed: {
            if (m_context) {
                PipelineDrawIndexedData drawData = operation->GetOperationParam<PipelineDrawIndexedData>();
                m_context->DrawIndexed(drawData.numIndexes, 0, 0);
                operation->SetResult(S_OK);
            }
            break;
        }
        case PipelineOperationType::Device_SetConstantsBufferState: {

            PipelineMatrixBufferData param = operation->GetOperationParam<PipelineMatrixBufferData>();

            std::map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>>& constantBuffers = param.constantsBuffers;
            std::map<std::string, std::pair<int, MatrixDefinition::AnyMatrixBuffer>> matrices = param.matrices;

            D3D11_MAPPED_SUBRESOURCE mapped = {};

            for (const auto& [matrixName, matrixPair] : param.matrices) {
				int nSlot = matrixPair.first;

                auto bufferComPtrIt = constantBuffers.find(matrixName);
                if (bufferComPtrIt == constantBuffers.end()) {
                    // Si no se encuentra el buffer (por ejemplo, no se creó durante la inicialización),
                    // lo saltamos y continuamos con el siguiente.
                    continue;
                }
                ID3D11Buffer* pBuffer = bufferComPtrIt->second.Get();
                // D3D11_MAP_WRITE_DISCARD es eficiente si el buffer se actualiza cada frame.
                HRESULT hr = m_context->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
                if (FAILED(hr)) {
                    // Manejar error de mapeo, quizás con un log o assert.
                    continue;
                }
                std::string matrixType = "";
                std::visit([&](auto currentMatrixStruct) {
                    currentMatrixStruct.SetMatrixData(param.data);
                    matrixType = currentMatrixStruct.MatrixType();
                    size_t size = currentMatrixStruct.Size();
                    memcpy(mapped.pData, &currentMatrixStruct, size);
                }, matrixPair.second);

                m_context->Unmap(pBuffer, 0);

                if (matrixType == MATRIX_TYPE_VERTEX.data() || matrixType == MATRIX_TYPE_MIXED.data()) {
                    // Estos buffers contienen matrices de transformación que suelen usarse en el Vertex Shader.
                    m_context->VSSetConstantBuffers(nSlot, 1, &pBuffer);
                }

                if (matrixType == MATRIX_TYPE_PIXEL.data() || matrixType == MATRIX_TYPE_MIXED.data()) {
                    // Estos buffers contienen datos que son cruciales para los cálculos de iluminación
                    // y propiedades de superficie, que se realizan en el Pixel Shader.
                    m_context->PSSetConstantBuffers(nSlot, 1, &pBuffer);
                }
            }

            /*PipelineMatrixBufferData param = operation->GetOperationParam<PipelineMatrixBufferData>();
            Material* material = param.material;
            MatrixDefinitionBase::MatrixParams matrixParams = param.data;
            material->SetConstantBuffers(m_context, matrixParams);
            material->Apply(m_context);*/
        }
        }
            
   //         int slot = 0;

			//PipelineMaterialBufferData param = operation->GetOperationParam<PipelineMaterialBufferData>();

   //         if ((param.oper & static_cast<unsigned int>(PipelineMatrixBufferType::WorldMatrix)) != 0) {
   //             ID3D11Buffer* pBuffer = ;
   //             HRESULT hr = m_context->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
   //             if (FAILED(hr)) {
   //                 // Manejar error de mapeo, quizás con un log o assert.
   //                 continue;
   //             }
   //         }

   //         
   //         std::map<std::string, std::pair<int, std::unique_ptr<MatrixDefinition::AnyMatrixBuffer>>>& matrices = matrixShaders[shaderName];

   //         for (const auto& [matrixName, matrix] : matrices) {
   //             // Buscamos el ID3D11Buffer correspondiente en nuestro mapa de buffers globales

   //             auto bufferComPtrIt = constantBuffers.find(matrixName);
   //             if (bufferComPtrIt == constantBuffers.end()) {
   //                 // Si no se encuentra el buffer (por ejemplo, no se creó durante la inicialización),
   //                 // lo saltamos y continuamos con el siguiente.
   //                 continue;
   //             }

   //             // 'nSlot' es el registro (ej. b0, b1, b2) que el shader espera para este buffer
   //             int nSlot = matrix.first;
   //             // Obtenemos el puntero raw del ComPtr para usarlo con los métodos de DirectX
   //             ID3D11Buffer* pBuffer = bufferComPtrIt->second.Get();

   //             // Mapeamos el buffer de constante para escritura.
   //             // D3D11_MAP_WRITE_DISCARD es eficiente si el buffer se actualiza cada frame.
   //             HRESULT hr = context->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
   //             if (FAILED(hr)) {
   //                 // Manejar error de mapeo, quizás con un log o assert.
   //                 continue;
   //             }

   //             std::string matrixType = "";

   //             // Usamos std::visit para aplicar el método SetMatrixData correcto
   //             // a la estructura de buffer de constante específica (MatrixBufferType, DirectionalLight, etc.).
   //             std::visit([&](auto& currentMatrixStruct) {
   //                 // Llamamos a SetMatrixData para rellenar la estructura con los datos actuales
   //                 // de MatrixParams. Cada estructura sabe qué datos de MatrixParams necesita.
   //                 currentMatrixStruct.SetMatrixData(matrixParams);
   //                 matrixType = currentMatrixStruct.MatrixType();
   //                 // Copiamos los datos de nuestra estructura C++ a la memoria mapeada de la GPU.
   //                 // Asegúrate de que el tamaño de la estructura coincida con el tamaño del buffer en la GPU.
   //                 memcpy(mapped.pData, &currentMatrixStruct, sizeof(std::decay_t<decltype(currentMatrixStruct)>));
   //                 }, *matrix.second); // Accedemos al contenido del unique_ptr<AnyMatrixBuffer>

   //             // Desmapeamos el buffer para que la GPU pueda acceder a los datos actualizados.
   //             context->Unmap(pBuffer, 0);

   //             // --- ENLACE DE LOS CONSTANT BUFFERS A LOS SHADERS ---
   //             // Aquí decidimos a qué estadio del pipeline se enlaza cada buffer.
   //             // Lo más común es:
   //             // - Matrices de transformación (World, View, Projection): Vertex Shader.
   //             // - Datos de cámara, luz, material: Pixel Shader (para cálculos de iluminación).

   //             if (matrixType == MATRIX_TYPE_VERTEX.data() || matrixType == MATRIX_TYPE_MIXED.data()) {
   //                 // Estos buffers contienen matrices de transformación que suelen usarse en el Vertex Shader.
   //                 context->VSSetConstantBuffers(nSlot, 1, &pBuffer);
   //             }

   //             if (matrixType == MATRIX_TYPE_PIXEL.data() || matrixType == MATRIX_TYPE_MIXED.data()) {
   //                 // Estos buffers contienen datos que son cruciales para los cálculos de iluminación
   //                 // y propiedades de superficie, que se realizan en el Pixel Shader.
   //                 context->PSSetConstantBuffers(nSlot, 1, &pBuffer);
   //             }
   //             // Nota: Si un buffer como "CameraData" también se necesitara en el Vertex Shader (ej. para billboarding),
   //             // podrías añadir otra línea: context->VSSetConstantBuffers(nSlot, 1, &pBuffer);
   //         }       
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