#include "RenderPipelineExecutor.h"
#include "Stages/InputAssemblyStage.h"
#include "Stages/OutputMergerStage.h"
#include "Stages/PixelShaderStage.h"
#include "Stages/RasterizerStage.h"
#include "Stages/VertexShaderStage.h"
#include <../Includes/FrameStates.h>
#include <Assets/Base/MeshAssetBase.h>
#include <d3d11.h>
#include <Defines/Mesh.h>
//#include <Defines/Pipeline.h>
#include <dxgi.h>
#include <dxgiformat.h>
#include <exception>
#include <Locators/ManagerLocator/ManagerLocator.h>
#include <RenderState/FrameStates/MeshFrameState.h>
#include <Services/FrameStateService.h>
#include <Services/Material.h>
#include <string>
#include <Windows.h>
#include <wrl/client.h>

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
        m_deviceManager = ManagerLocator::GetDeviceManager();
        m_outputMergerStage = new OutputMergerStage(m_deviceManager);

        RegisterOperations(nullptr);
    }

    RenderPipelineExecutor::RenderPipelineExecutor(
        Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
        Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain)
    {
        m_context = context;
        m_swapChain = swapChain;
        m_deviceManager = ManagerLocator::GetDeviceManager();
        m_inputAssemblyStage = new InputAssemblyStage(m_deviceManager);
        m_vertexShaderStage = new VertexShaderStage(m_deviceManager);
        m_pixelShaderStage = new PixelShaderStage(m_deviceManager);
        m_rasterizerStage = new RasterizerStage(m_deviceManager);
        m_outputMergerStage = new OutputMergerStage(m_deviceManager);
        RegisterOperations(nullptr);
    }

    void RenderPipelineExecutor::RegisterOperations(FrameStateService* frameState) {
        m_operationsMap[PipelineOperationType::Device_Init_SetRenderTargetView].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            /*Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView = frameState->PipelineState()->GetCurrentDepthStencilView();
            ID3D11RenderTargetView* rtv = frameState->PipelineState()->GetCurrentRenderTargetView().Get();*/
            //this->GetOutputMergerStage()->SetRenderTargets(1, &rtv, pDepthStencilView.Get());
            if (parameter != nullptr) {
                auto param = dynamic_cast<PipelineSetRenderTargetsData*>(parameter);
                if (param != nullptr) {
                    this->GetOutputMergerStage()->SetRenderTargets(1, &param->targetView, param->stencilView.Get());
                }
            }
            };
        m_operationsMap[PipelineOperationType::Device_ClearRenderTargetView].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            /*const FLOAT* clearColor = frameState->DeviceState()->GetClearColor();
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView> view = frameState->PipelineState()->GetCurrentRenderTargetView();*/
            if (parameter != nullptr) {
                auto param = dynamic_cast<PipelineRenderTargetViewData*>(parameter);
                if (param != nullptr) {
                    const float* clearColor = new FLOAT[4]{ param->clearColor.x, param->clearColor.y, param->clearColor.z, param->clearColor.w };
                    this->GetOutputMergerStage()->ClearRenderTargetView(param->data, clearColor);

                }
            }
            //this->GetOutputMergerStage()->ClearRenderTargetView(view, clearColor);xz

            };
        m_operationsMap[PipelineOperationType::Device_Unbind_RenderTargetView].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            std::string dsName = frameState->GetPipelineState()->GetStencilState();
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView> ppRenderTargetViews = frameState->PipelineState()->GetCurrentRenderTargetView();
            Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView = frameState->PipelineState()->GetCurrentDepthStencilView();
            ID3D11RenderTargetView* rtv = frameState->PipelineState()->GetCurrentRenderTargetView().Get();
            this->GetOutputMergerStage()->SetRenderTargets(1, &rtv, pDepthStencilView.Get());
            };
        m_operationsMap[PipelineOperationType::Device_SetDepthStencilState].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            std::string dsName = frameState->GetPipelineState()->GetMainStencilState();
            this->GetOutputMergerStage()->SetDepthStencilState(frameState->PipelineState()->GetDepthStencilState(dsName), 1);
            };
        m_operationsMap[PipelineOperationType::Device_ClearDepthStencilView].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            //std::string dsName = frameState->PassState()->GetConfig().stencilDef;
            /*this->GetOutputMergerStage()->ClearDepthStencilView(frameState->PipelineState()->GetDepthStencilView(dsName)->stencilViewData, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
            };*/
            Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView = frameState->PipelineState()->GetCurrentDepthStencilView();
            this->GetOutputMergerStage()->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
            };
        m_operationsMap[PipelineOperationType::Device_ClearDepthView].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            /*std::string dsName = frameState->PassState()->GetConfig().stencilDef;
            this->GetOutputMergerStage()->ClearDepthStencilView(frameState->PipelineState()->GetDepthStencilView(dsName)->stencilViewData, D3D11_CLEAR_DEPTH, 1.0f, 0);
            };*/
            this->GetOutputMergerStage()->ClearDepthStencilView(frameState->PipelineState()->GetCurrentDepthStencilView(), D3D11_CLEAR_DEPTH, 1.0f, 0);
            };
        m_operationsMap[PipelineOperationType::Device_ClearStencilView].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            /*std::string dsName = frameState->GetPipelineState()->GetStencilDef();
            if (dsName.empty()) return;
            Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDepthStencilView = frameState->PipelineState()->GetDepthStencilView(dsName)->stencilViewData;
            this->GetOutputMergerStage()->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_STENCIL, 1.0f, 0);
            };*/
            this->GetOutputMergerStage()->ClearDepthStencilView(frameState->PipelineState()->GetCurrentDepthStencilView(), D3D11_CLEAR_STENCIL, 1.0f, 0);
            };
        m_operationsMap[PipelineOperationType::Device_SetRasterizedState].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            /*std::string rsName = frameState->GetPipelineState()->GetRasterizedState();
            this->GetRasterizerStage()->SetState(frameState->PipelineState()->GetRasterizerState(rsName));*/
            this->GetRasterizerStage()->SetState(frameState->PipelineState()->GetRasterizerState("RasterizedMainColorPass"));
            };
        m_operationsMap[PipelineOperationType::Device_EnabledBledingState].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            std::string blendName = frameState->PassState()->GetConfig().blendState;
            this->GetOutputMergerStage()->SetBlendState(frameState->PipelineState()->GetBlendState(blendName));
            };
        m_operationsMap[PipelineOperationType::Device_DisabledBledingState].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            std::string blendName = frameState->PassState()->GetConfig().blendState;
            this->GetOutputMergerStage()->SetBlendState(frameState->PipelineState()->GetBlendState(blendName));
            };
        m_operationsMap[PipelineOperationType::Device_SetViewport].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            /*std::string vpName = frameState->GetPipelineState()->GetMainViewport();
            const D3D11_VIEWPORT vp = frameState->PipelineState()->GetViewport(vpName);
            this->GetRasterizerStage()->SetViewport(&vp);*/

            if (parameter != nullptr) {
                auto param = dynamic_cast<PipelineViewPortData*>(parameter);
                if (param != nullptr) {
                    this->GetRasterizerStage()->SetViewport(&param->desc);
                }
            }


            };
        m_operationsMap[PipelineOperationType::Device_PresentSwapChain].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            this->GetOutputMergerStage()->Present(this->m_swapChain.Get(), 1, 0);
            };
        m_operationsMap[PipelineOperationType::Mesh_Render_SetPrimitiveToplogy]
            .second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            MeshAssetBase* mesh = frameState->MeshState()->GetCurrentMesh();
            if (!mesh) return;
            this->m_inputAssemblyStage->SetPrimitiveTopology(mesh->GetPrimitiveTopology());
            };
        m_operationsMap[PipelineOperationType::Device_draw].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            const MeshAssetBase* mesh = frameState->MeshState()->GetCurrentMesh();
            Mesh::DrawType drawType = mesh->GetDrawType();
            PipelineDrawIndexedData* param{};
            switch (drawType) {
            case Mesh::DrawType::Draw:
                m_deviceManager->GetContext()->Draw(mesh->GetVertexCount(), 0);
                break;
            case Mesh::DrawType::DrawIndexed:
                param = dynamic_cast<PipelineDrawIndexedData*>(parameter);

                //auto& paramBase = parameter.value();
                //PipelineDrawIndexedData param = static_cast<PipelineDrawIndexedData>(paramBase);
                if (param != nullptr) {
                    m_deviceManager->GetContext()->DrawIndexed(param->numIndexes, param->indexOffset, param->vertexOffset);
                    break;
                }

                m_deviceManager->GetContext()->DrawIndexed(mesh->GetIndexCount(), 0, 0);
                break;
            case Mesh::DrawType::DrawInstanced:
                m_deviceManager->GetContext()->DrawInstanced(mesh->GetVertexCount(), frameState->MeshState()->GetNumberOfInstances(), 0, 0);
                break;
            };
            };
        /*m_operationsMap[PipelineOperationType::Device_SetConstantsBufferState].second =
             [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
             MeshFrameState* meshState = frameState->MeshState();
             const MeshAssetBase* mesh = meshState->GetMesh();
             if (!mesh || !mesh->GetMaterial()) return;
             bool a = true;
             const Material* material = mesh->GetMaterial();
             };*/
        m_operationsMap[PipelineOperationType::Mesh_Render_SetTexture].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            MeshFrameState* meshState = frameState->MeshState();
            const MeshAssetBase* mesh = meshState->GetMesh();
            if (!mesh || !mesh->GetMaterial()) return;
            const Material* material = mesh->GetMaterial();
            if (!material) return;
            UINT numTextures = material->GetNumTextures();
            m_pixelShaderStage->SetShaderResources(6, numTextures, material->GetTextures().data());
            };
        m_operationsMap[PipelineOperationType::Mesh_Render_SetPixelShader].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            /*MeshFrameState* meshState = frameState->MeshState();
            const MeshAssetBase* mesh = meshState->GetMesh();
            if (!mesh || !mesh->GetMaterial()) return;
            const Material* material = mesh->GetMaterial();
            if (!material) return;
            Microsoft::WRL::ComPtr<ID3D11PixelShader> pShader = material->GetPixelShader();
            if (pShader) {
                m_pixelShaderStage->SetShader(pShader);
            }*/
            if (parameter != nullptr) {
                auto param = dynamic_cast<PipelinePixelShaderData*>(parameter);
                if (param != nullptr && param->data != nullptr) {
                    m_pixelShaderStage->SetShader(param->data);

                }
            }
            };
        m_operationsMap[PipelineOperationType::Mesh_Render_SetVertexShader].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            MeshFrameState* meshState = frameState->MeshState();
            const MeshAssetBase* mesh = meshState->GetMesh();
            if (!mesh || !mesh->GetMaterial()) return;
            const Material* material = mesh->GetMaterial();
            if (!material) return;
            Microsoft::WRL::ComPtr<ID3D11VertexShader> vShader = material->GetVertexShader();
            if (vShader) {
                m_vertexShaderStage->SetShader(vShader);
            }
            };
        m_operationsMap[PipelineOperationType::Mesh_Render_SetSampler].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            std::string samplerName = frameState->GetPipelineState()->GetShader();
            if (samplerName.empty()) return;
            Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler = frameState->PipelineState()->GetSamplerState(samplerName);
            ID3D11SamplerState* spr = sampler.Get();
            m_pixelShaderStage->SetSamplers(0, 1, &spr);
            };
        m_operationsMap[PipelineOperationType::Mesh_Render_SetInputLayout].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            try
            {
                MeshFrameState* meshState = frameState->MeshState();
                if (!meshState->HasMeshes()) return;

                const MeshAssetBase* mesh = meshState->GetMesh();
                if (!mesh || !mesh->GetMaterial()) return;
                const Material* material = mesh->GetMaterial();
                if (!material) return;
                Microsoft::WRL::ComPtr<ID3D11InputLayout> layout = material->GetInputLayout();
                if (layout) {
                    m_inputAssemblyStage->SetInputLayout(layout);
                }
            }
            catch (const std::exception&)
            {
                bool a = false;
            }
            };

        m_operationsMap[PipelineOperationType::Device_UpdateConstantsBufferResource].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            frameState->PipelineState()->UpdateAllConstantBuffers();
            };

        m_operationsMap[PipelineOperationType::Mesh_Render_SetVertexBuffer].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {

            //if (parameter != nullptr) {
            //    auto param = dynamic_cast<PipelineSetVertexBufferData*>(parameter);
            //    if (param != nullptr) {
            //        ID3D11Buffer* pBuffer = param->GetVertexBuffer();
            //        UINT stride = param->stride;
            //        UINT offset = param->offset;
            //        //ID3D11Buffer* pBuffer = param->vertexBuffer.Get();
            //        m_inputAssemblyStage->SetVertexBuffers(param->startSlot, param->numBuffers, pBuffer, &stride, &offset);
            //        return;
            //    }
            //}

            MeshFrameState* meshState = frameState->MeshState();
            MeshAssetBase* mesh = meshState->GetMesh();
            if (!mesh) return;
            Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer = mesh->GetVertexBuffer();
            if (vertexBuffer) {
                UINT stride = mesh->GetVertexTypeSize();
                UINT offset = 0;
                //ID3D11Buffer* pBuffer = vertexBuffer.Get();
                m_inputAssemblyStage->SetVertexBuffers(0, 1, vertexBuffer.Get(), &stride, &offset);
            }
            };

        m_operationsMap[PipelineOperationType::Mesh_Render_SetIndexBuffer].second =
            [this](PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
            MeshFrameState* meshState = frameState->MeshState();
            MeshAssetBase* mesh = meshState->GetMesh();
            if (!mesh) return;
            const Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer = mesh->GetIndexBuffer();
            if (indexBuffer) {
                DXGI_FORMAT format = DXGI_FORMAT_R32_UINT;
                UINT offset = 0;
                m_inputAssemblyStage->SetIndexBuffer(indexBuffer, format, offset);
            }
            };
        //m_operationsMap[PipelineOperationType::Mesh_Render_SetInputLayout].second =
        //m_operationsMap[PipelineOperationType::Device_SetConstantsBufferState].second =


            //std::map<int, std::pair<std::string, MatrixDefinition::AnyMatrixBuffer>> matrices();

            //std::map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>>& constantBuffers = param.constantsBuffers;
            /*std::map<int, std::pair<std::string, MatrixDefinition::AnyMatrixBuffer>> matrices = param.matrices;
            D3D11_MAPPED_SUBRESOURCE mapped = {};
            for (const auto& [slot, matrixPair] : matrices) {
                std::string matrixName = matrixPair.first;
                std::shared_ptr<MatrixDefinitionBase::MatrixParams> bParams = MatrixDefinitionBase::GetMatrixParams< MatrixDefinitionBase::MatrixParams>(param.params[MATRIX_PARAM_BASE.data()]);
                std::shared_ptr<MatrixDefinitionBase::LightMatrixParams> lParams = MatrixDefinitionBase::GetMatrixParams< MatrixDefinitionBase::LightMatrixParams>(param.params[MATRIX_PARAM_LIGHT.data()]);
                std::shared_ptr<MatrixDefinitionBase::MaterialMatrixParams> mParams = MatrixDefinitionBase::GetMatrixParams< MatrixDefinitionBase::MaterialMatrixParams>(param.params[MATRIX_PARAM_MATERIAL.data()]);
                std::shared_ptr<MatrixDefinitionBase::WaterMatrixParams> wParams = MatrixDefinitionBase::GetMatrixParams< MatrixDefinitionBase::WaterMatrixParams>(param.params[MATRIX_PARAM_WATER.data()]);
            }*/





            /*PipelineOperation* operation = nullptr;
            operation = new PipelineOperation(operationType);
            switch (operationType) {
                case PipelineOperationType::Device_ClearRenderTargetView:
                    operation->SetExecutor()


            }*/

    }

    void RenderPipelineExecutor::AddOperation(PipelineOperationType operationType) {
        /*PipelineOperation* operation = new PipelineOperation(operationType);
        m_operations.push_back(operation);*/
    }

    void RenderPipelineExecutor::Execute(FrameStateService* frameState) {

    }


    //  void RenderPipelineExecutor::ExecuteOperation(PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase parametere* frameState) {
  //      PipelineOperationType operationType = operation->GetOperationType();

  //      switch (operationType) {
  //      case PipelineOperationType::Device_ClearRenderTargetView: {
  //          if (m_outputMergerStage) {
  //              PipelineRenderTargetViewData rtvData = operation->GetOperationParam<PipelineRenderTargetViewData>();
  //              DirectX::XMFLOAT4 cColor = rtvData.clearColor;
  //              const FLOAT* clearColor = new FLOAT[4]{ cColor.x, cColor.y, cColor.z, cColor.w };
  //              m_outputMergerStage->ClearRenderTargetView(rtvData.data, clearColor);
  //              delete[] clearColor;
  //              operation->SetResult(S_OK);
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Device_ClearDepthStencilView: {
  //          if (m_outputMergerStage) {
  //              PipelineDepthStencilData stencilData = operation->GetOperationParam<PipelineDepthStencilData>();
  //              if (stencilData.stencilViewData) {
  //                  /*m_outputMergerStage->ClearDepthStencilView(
  //                      stencilData.data, stencilData.clearFlags, 1.0f, 0);*/
  //                  m_outputMergerStage->ClearDepthStencilView(
  //                      stencilData.stencilViewData, stencilData.clearFlags, stencilData.depth, stencilData.stencil);
  //                  operation->SetResult(S_OK);
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Device_SetDepthStencilState: {
  //          if (m_outputMergerStage) {
  //              PipelineSetencilStateData stencilStateData = operation->GetOperationParam<PipelineSetencilStateData>();
  //              if (stencilStateData.state) {
  //                  m_outputMergerStage->SetDepthStencilState(stencilStateData.state, 1);
  //                  operation->SetResult(S_OK);
  //              }
        //	}
  //          break;
  //      }
  //      case PipelineOperationType::Device_DisabledBledingState:
  //      case PipelineOperationType::Device_EnabledBledingState: {
  //          if (m_outputMergerStage) {
  //              PipelineBledingData blendStateData = operation->GetOperationParam<PipelineBledingData>();
  //              if (blendStateData.state) {
  //                  m_outputMergerStage->SetBlendState(blendStateData.state, nullptr, 0xFFFFFFFF);
  //                  operation->SetResult(S_OK);
  //              }
  //          }
  //          break;
        //}
  //       
  //      case PipelineOperationType::Device_SetViewport:
  //          if (m_rasterizerStage) {
  //              PipelineViewPortData viewportData = operation->GetOperationParam<PipelineViewPortData>();
  //              if (viewportData.desc.Height) {
  //                  m_rasterizerStage->SetViewport(viewportData.desc);
  //                  operation->SetResult(S_OK);
  //              }
  //          }
  //          break;
  //     
  //      case PipelineOperationType::Device_PresentSwapChain:
  //          if (m_outputMergerStage) {
  //              PipelinePresentSwapChain swapChainData = operation->GetOperationParam<PipelinePresentSwapChain>();
  //              if (swapChainData.data) {
  //                  m_outputMergerStage->Present(swapChainData.data, swapChainData.syncInterval, swapChainData.flags);
  //                  operation->SetResult(S_OK);
  //              }
  //          }
  //          break;
  //      
  //      case PipelineOperationType::Mesh_Render_SetVertexBuffer: 
  //          if (m_inputAssemblyStage) {
  //              auto vertexData = operation->GetOperationParam<PipelineSetVertexBufferData>();
  //              if (vertexData.vertexBuffer) {
  //                  //Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer = vertexData.vertexBuffer;
  //                  m_inputAssemblyStage->SetVertexBuffers(
  //                      vertexData.startSlot,
  //                      vertexData.numBuffers,
  //                      vertexData.vertexBuffer,
  //                      //pBuffer,
  //                      &vertexData.stride,
  //                      &vertexData.offset
  //                  );
  //                  operation->SetResult(S_OK);
  //              }
  //          }
  //          break;
  //      
  //      case PipelineOperationType::Mesh_Render_SetIndexBuffer: {
  //          if (m_inputAssemblyStage) {
  //              auto indexData = operation->GetOperationParam<PipelineSetIndexBufferData>();
  //              if (indexData.indexBuffer) {
  //                  m_inputAssemblyStage->SetIndexBuffer(
  //                      indexData.indexBuffer,
  //                      indexData.format,
  //                      indexData.offset
  //                  );
  //                  operation->SetResult(S_OK);
  //              }
  //          }
  //          break;
  //      }

        //case PipelineOperationType::Mesh_Render_ResetVertexBuffer:
  //          if (m_inputAssemblyStage) {
  //              m_inputAssemblyStage->ResetVertexBuffers();
  //          }
        //	break;

  //      case PipelineOperationType::Mesh_Render_ResetIndexBuffer:
  //          if (m_inputAssemblyStage) {                
  //              m_inputAssemblyStage->ResetVertexBuffers();
  //          }
  //          break;

  //      case PipelineOperationType::Device_Init_SetRenderTargetView: {
  //          if (m_outputMergerStage) {
  //              auto rtvData = operation->GetOperationParam<PipelineSetRenderTargetsData>();
  //              if (rtvData.targetView || rtvData.stencilView) {
  //                  ID3D11RenderTargetView* tView = rtvData.targetView.Get();
  //                  ID3D11DepthStencilView* sView = rtvData.stencilView.Get();
  //                  bool isColorPass = rtvData.isColorPass;
        //			m_outputMergerStage->SetRenderTargets( isColorPass ? 1 : 0, &tView, sView);
  //                  operation->SetResult(S_OK);
  //              }
  //          }
        //	break;
  //      }
  //      case PipelineOperationType::Device_ResetRenderTargetView: {
  //          if (m_outputMergerStage) {
  //              auto rtvData = operation->GetOperationParam<PipelineSetRenderTargetsData>();
  //              if (rtvData.targetView || rtvData.stencilView) {
  //                  ID3D11RenderTargetView* tView = rtvData.targetView.Get();
  //                  m_outputMergerStage->SetRenderTargets(1, &tView, nullptr);
  //                  operation->SetResult(S_OK);
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Device_SetRasterizedState:
  //      case PipelineOperationType::Device_Init_RasterizedState: {
  //          if (m_rasterizerStage) {
  //              auto rasterData = operation->GetOperationParam<PipelineRasteriezeData>();
  //              if (rasterData.state) {
  //                  m_rasterizerStage->SetState(rasterData.state);
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Mesh_Render_SetVertexShader:
  //      case PipelineOperationType::Device_Init_SetVertexShader: {
  //          if (m_vertexShaderStage) {
  //              auto shaderData = operation->GetOperationParam<PipelineVertexShaderData>();
  //              if (shaderData.data) {
  //                  m_vertexShaderStage->SetShader(shaderData.data);
  //                  operation->SetResult(S_OK);
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Mesh_Render_SetPixelShader:
  //      case PipelineOperationType::Device_Init_SetPixelShader: {
  //          if (m_pixelShaderStage) {
  //              auto shaderData = operation->GetOperationParam<PipelinePixelShaderData>();
  //              if (shaderData.data) {
  //                  m_pixelShaderStage->SetShader(shaderData.data);
  //                  operation->SetResult(S_OK);
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Mesh_Render_SetInputLayout: {
  //          if (m_inputAssemblyStage != nullptr) {
  //              auto layoutData = operation->GetOperationParam<PipelineLayoutData>();
  //              if (layoutData.data) {
  //                  m_inputAssemblyStage->SetInputLayout(layoutData.data);
  //                  operation->SetResult(S_OK);
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Mesh_Render_SetTexture: {
  //          if (m_pixelShaderStage) {
  //              auto textureData = operation->GetOperationParam<PipelineTextureData>();
  //              if (textureData.data.size()) {
  //                  m_pixelShaderStage->SetShaderResources(textureData.startSlot, textureData.numTextures, textureData.data.data());
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Mesh_Render_Reset_Textures: {
  //          if (m_pixelShaderStage) {
  //              m_pixelShaderStage->ResetShaderResources();
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Mesh_Render_SetSampler: {
  //          if (m_pixelShaderStage) {
  //              PipelineSamplerSateData samplerData = operation->GetOperationParam<PipelineSamplerSateData>();
  //              if (samplerData.data.size()) {
  //                  std::vector<ID3D11SamplerState*> rawSamplers;
  //                  rawSamplers.reserve(samplerData.data.size());
  //                  for (const auto& comPtr : samplerData.data) {
  //                      rawSamplers.push_back(comPtr.second.Get());
  //                  }
  //                  m_pixelShaderStage->SetSamplers(samplerData.startSlot, static_cast<UINT>(rawSamplers.size()), rawSamplers.data());
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Mesh_Render_Reset_Sampler: {
  //          if (m_pixelShaderStage) {
  //              m_pixelShaderStage->ResetSamplers();
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Mesh_Render_SetPrimitiveToplogy: {
  //          if (m_inputAssemblyStage) {
  //              auto topoData = operation->GetOperationParam<PipelinePrimitiveTopologyData>();
  //              if (topoData.data) {
  //                  m_inputAssemblyStage->SetPrimitiveTopology(topoData.data);
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Device_drawInstanced: {
  //          if (m_context) {
  //              PipelineDrawInstancedData drawData = operation->GetOperationParam<PipelineDrawInstancedData>();
  //              m_context->DrawIndexedInstanced(drawData.indexCountPerInstance, drawData.instanceCount, 0, 0, 0);
  //              operation->SetResult(S_OK);
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Device_drawIndexed: {
  //          if (m_context) {
  //              PipelineDrawIndexedData drawData = operation->GetOperationParam<PipelineDrawIndexedData>();
  //              m_context->DrawIndexed(drawData.numIndexes, drawData.indexOffset, drawData.vertexOffset);
  //              operation->SetResult(S_OK);
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Device_draw: {
  //          if (m_context) {
  //              PipelineDrawData drawData = operation->GetOperationParam<PipelineDrawData>();
  //              m_context->Draw(drawData.vertexCount, 0);
  //              operation->SetResult(S_OK);
  //          }
        //	break;
  //      }
  //      case PipelineOperationType::Device_SetConstantsBufferState: {

  //          PipelineMatrixBufferData param = operation->GetOperationParam<PipelineMatrixBufferData>();

  //          std::map<std::string, Microsoft::WRL::ComPtr<ID3D11Buffer>>& constantBuffers = param.constantsBuffers;
  //          std::map<int, std::pair<std::string, MatrixDefinition::AnyMatrixBuffer>> matrices = param.matrices;

  //          D3D11_MAPPED_SUBRESOURCE mapped = {};

  //          for (const auto& [slot, matrixPair] : matrices) {
        //		std::string matrixName = matrixPair.first;

        //		std::shared_ptr<MatrixDefinitionBase::MatrixParams> bParams = MatrixDefinitionBase::GetMatrixParams< MatrixDefinitionBase::MatrixParams>(param.params[MATRIX_PARAM_BASE.data()]);
  //              std::shared_ptr<MatrixDefinitionBase::LightMatrixParams> lParams = MatrixDefinitionBase::GetMatrixParams< MatrixDefinitionBase::LightMatrixParams>(param.params[MATRIX_PARAM_LIGHT.data()]);
  //              std::shared_ptr<MatrixDefinitionBase::MaterialMatrixParams> mParams = MatrixDefinitionBase::GetMatrixParams< MatrixDefinitionBase::MaterialMatrixParams>(param.params[MATRIX_PARAM_MATERIAL.data()]);
  //              std::shared_ptr<MatrixDefinitionBase::WaterMatrixParams> wParams = MatrixDefinitionBase::GetMatrixParams< MatrixDefinitionBase::WaterMatrixParams>(param.params[MATRIX_PARAM_WATER.data()]);

  //              if (matrixName == "TerrainBlendBuffer") {
  //                  mParams->terrainBlenderData = {};
        //			mParams->terrainBlenderData.dirtHeight = 0.0f; 
  //                  mParams->terrainBlenderData.grassHeight = 0.0f;
  //                  mParams->terrainBlenderData.slopeEnd = 1.0f;
  //                  mParams->terrainBlenderData.slopeStart = 0.7f;
  //                  mParams->terrainBlenderData.snowHeight = 50.0f;
  //                  mParams->terrainBlenderData.terrainScale = .008f;
        //		}
  //              if (matrixName == "Terrain2BlendBuffer") {
  //                  mParams->terrain2BlenderData = {};
  //                  mParams->terrain2BlenderData.grassTransitionHeight = 100.0f; // La hierba empieza desde la base
  //                  mParams->terrain2BlenderData.grassTransitionSlope = 0.2f;  // Se mezcla a pendientes moderadas

  //                  mParams->terrain2BlenderData.dirtTransitionHeight = 150.0f;  // La tierra empieza a aparecer a partir de 5m de altura
  //                  mParams->terrain2BlenderData.dirtTransitionSlope = 0.35f;   // Se mezcla en pendientes moderadas a altas

  //                  mParams->terrain2BlenderData.rockTransitionHeight = 310.0f; // La roca empieza a aparecer a 10m de altura
  //                  mParams->terrain2BlenderData.rockTransitionSlope = 0.6f;   // Se mezcla en pendientes altas

  //                  mParams->terrain2BlenderData.snowTransitionHeight = 500.0f; // La nieve aparece a partir de 15m de altura
  //                  mParams->terrain2BlenderData.snowTransitionSlope = 0.4f;   // La nieve se acumula en pendientes bajas

  //                  mParams->terrain2BlenderData.beachTransitionHeight = -100.0f; // La playa empieza desde la base
  //                  mParams->terrain2BlenderData.beachTransitionSlope = 0.2f; // Se mezcla en pendientes suaves

  //                  mParams->terrain2BlenderData.terrainScale = 0.005f; // Un valor bajo para que las texturas no se vean demasiado estiradas
  //              }
  //              if (matrixName == "WaterData") {
  //                  wParams->waterData.waveHeight = 0.3f;
  //                  wParams->waterData.waveSpeed = 0.3f;
  //                  wParams->waterData.waveLength = 25.0f;
  //                  wParams->waterData.waveFrequency = 0.8f;
  //                  wParams->waterData.waterDensity = 1.0f;
  //                  wParams->waterData.waterViscosity = 1.0f;
  //                  wParams->waterData.reflectionIntensity = 1.0f; // Valores por defecto para futuros efectos
  //                  wParams->waterData.refractionIndex = 1.333f; // El índice de refracción del agua
  //                  wParams->waterData.foamAmount = 0.0f; // Desactivado por ahora
  //                  wParams->waterData.causticsIntensity = 0.0f; // Desactivado por ahora
  //                  wParams->waterData.waterColor = { 0.0f, 0.5f, 0.8f, 1.0f };
  //                  wParams->waterData.isUnderwater = FALSE;
  //                  wParams->waterData.hasFoam = FALSE;
  //                  wParams->waterData.hasCaustics = FALSE;
  //                  wParams->waterData.isReflective = FALSE;
  //                  wParams->waterData.isRefractive = FALSE;
  //              }

  //              auto bufferComPtrIt = constantBuffers.find(matrixName);
  //              if (bufferComPtrIt == constantBuffers.end()) {
  //                  // Si no se encuentra el buffer (por ejemplo, no se creó durante la inicialización),
  //                  // lo saltamos y continuamos con el siguiente.
  //                  continue;
  //              }
  //              ID3D11Buffer* pBuffer = bufferComPtrIt->second.Get();
  //              // D3D11_MAP_WRITE_DISCARD es eficiente si el buffer se actualiza cada frame.
  //              HRESULT hr = m_context->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
  //              if (FAILED(hr)) {
  //                  // Manejar error de mapeo, quizás con un log o assert.
  //                  continue;
  //              }
  //              std::string matrixType = "";
  //              std::visit([&](auto currentMatrixStruct) {
  //                  std::map<std::string, std::shared_ptr<IMatrixParams>>& p = param.params;
  //                  currentMatrixStruct.SetMatrixData(p);
  //                  matrixType = currentMatrixStruct.MatrixType();
  //                  size_t size = currentMatrixStruct.Size();
  //                  memcpy(mapped.pData, &currentMatrixStruct, size);
  //              }, matrixPair.second);

  //              m_context->Unmap(pBuffer, 0);

  //              if (matrixType == MATRIX_TYPE_VERTEX.data() || matrixType == MATRIX_TYPE_MIXED.data()) {
  //                  // Estos buffers contienen matrices de transformación que suelen usarse en el Vertex Shader.
  //                  m_context->VSSetConstantBuffers(slot, 1, &pBuffer);
  //              }

  //              if (matrixType == MATRIX_TYPE_PIXEL.data() || matrixType == MATRIX_TYPE_MIXED.data()) {
  //                  // Estos buffers contienen datos que son cruciales para los cálculos de iluminación
  //                  // y propiedades de superficie, que se realizan en el Pixel Shader.
  //                  m_context->PSSetConstantBuffers(slot, 1, &pBuffer);
  //              }
  //          }
  //          break;
  //          /*PipelineMatrixBufferData param = operation->GetOperationParam<PipelineMatrixBufferData>();
  //          Material* material = param.material;
  //          MatrixDefinitionBase::MatrixParams matrixParams = param.data;
  //          material->SetConstantBuffers(m_context, matrixParams);
  //          material->Apply(m_context);*/
  //      }
  //      case PipelineOperationType::Device_UpdateConstantsBufferResource: {
  //          PipelineUpdateMatrixBufferData updateData = operation->GetOperationParam<PipelineUpdateMatrixBufferData>();

  //          m_context->UpdateSubresource(
  //              updateData.constantsBuffer.Get(),
  //              0,
  //              nullptr,
  //              &updateData.data->waterInstancingData.instanceWorldMatrix,
  //              0,
  //              0
        //	);
  //          break;
        //}


  //      case PipelineOperationType::Device_ResetConstantsBuffers: {
  //          if (m_context) {
  //              ID3D11Buffer* nullConstantBuffers[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = {};
  //              m_context->PSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, nullConstantBuffers);
  //              ID3D11Buffer* nullConstantBuffersVS[D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT] = {};
  //              m_context->VSSetConstantBuffers(0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT, nullConstantBuffersVS);
  //          }
        //	break;
  //      }
  //      }
  //          
  // //         int slot = 0;

        //	//PipelineMaterialBufferData param = operation->GetOperationParam<PipelineMaterialBufferData>();

  // //         if ((param.oper & static_cast<unsigned int>(PipelineMatrixBufferType::WorldMatrix)) != 0) {
  // //             ID3D11Buffer* pBuffer = ;
  // //             HRESULT hr = m_context->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
  // //             if (FAILED(hr)) {
  // //                 // Manejar error de mapeo, quizás con un log o assert.
  // //                 continue;
  // //             }
  // //         }

  // //         
  // //         std::map<std::string, std::pair<int, std::unique_ptr<MatrixDefinition::AnyMatrixBuffer>>>& matrices = matrixShaders[shaderName];

  // //         for (const auto& [matrixName, matrix] : matrices) {
  // //             // Buscamos el ID3D11Buffer correspondiente en nuestro mapa de buffers globales

  // //             auto bufferComPtrIt = constantBuffers.find(matrixName);
  // //             if (bufferComPtrIt == constantBuffers.end()) {
  // //                 // Si no se encuentra el buffer (por ejemplo, no se creó durante la inicialización),
  // //                 // lo saltamos y continuamos con el siguiente.
  // //                 continue;
  // //             }

  // //             // 'nSlot' es el registro (ej. b0, b1, b2) que el shader espera para este buffer
  // //             int nSlot = matrix.first;
  // //             // Obtenemos el puntero raw del ComPtr para usarlo con los métodos de DirectX
  // //             ID3D11Buffer* pBuffer = bufferComPtrIt->second.Get();

  // //             // Mapeamos el buffer de constante para escritura.
  // //             // D3D11_MAP_WRITE_DISCARD es eficiente si el buffer se actualiza cada frame.
  // //             HRESULT hr = context->Map(pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
  // //             if (FAILED(hr)) {
  // //                 // Manejar error de mapeo, quizás con un log o assert.
  // //                 continue;
  // //             }

  // //             std::string matrixType = "";

  // //             // Usamos std::visit para aplicar el método SetMatrixData correcto
  // //             // a la estructura de buffer de constante específica (MatrixBufferType, DirectionalLight, etc.).
  // //             std::visit([&](auto& currentMatrixStruct) {
  // //                 // Llamamos a SetMatrixData para rellenar la estructura con los datos actuales
  // //                 // de MatrixParams. Cada estructura sabe qué datos de MatrixParams necesita.
  // //                 currentMatrixStruct.SetMatrixData(matrixParams);
  // //                 matrixType = currentMatrixStruct.MatrixType();
  // //                 // Copiamos los datos de nuestra estructura C++ a la memoria mapeada de la GPU.
  // //                 // Asegúrate de que el tamaño de la estructura coincida con el tamaño del buffer en la GPU.
  // //                 memcpy(mapped.pData, &currentMatrixStruct, sizeof(std::decay_t<decltype(currentMatrixStruct)>));
  // //                 }, *matrix.second); // Accedemos al contenido del unique_ptr<AnyMatrixBuffer>

  // //             // Desmapeamos el buffer para que la GPU pueda acceder a los datos actualizados.
  // //             context->Unmap(pBuffer, 0);

  // //             // --- ENLACE DE LOS CONSTANT BUFFERS A LOS SHADERS ---
  // //             // Aquí decidimos a qué estadio del pipeline se enlaza cada buffer.
  // //             // Lo más común es:
  // //             // - Matrices de transformación (World, View, Projection): Vertex Shader.
  // //             // - Datos de cámara, luz, material: Pixel Shader (para cálculos de iluminación).

  // //             if (matrixType == MATRIX_TYPE_VERTEX.data() || matrixType == MATRIX_TYPE_MIXED.data()) {
  // //                 // Estos buffers contienen matrices de transformación que suelen usarse en el Vertex Shader.
  // //                 context->VSSetConstantBuffers(nSlot, 1, &pBuffer);
  // //             }

  // //             if (matrixType == MATRIX_TYPE_PIXEL.data() || matrixType == MATRIX_TYPE_MIXED.data()) {
  // //                 // Estos buffers contienen datos que son cruciales para los cálculos de iluminación
  // //                 // y propiedades de superficie, que se realizan en el Pixel Shader.
  // //                 context->PSSetConstantBuffers(nSlot, 1, &pBuffer);
  // //             }
  // //             // Nota: Si un buffer como "CameraData" también se necesitara en el Vertex Shader (ej. para billboarding),
  // //             // podrías añadir otra línea: context->VSSetConstantBuffers(nSlot, 1, &pBuffer);
  // //         }       
  //      /*
  //      case PipelineOperationType::Mesh_Render_SetInputLayout: {
  //          if (m_inputAssemblyStage != nullptr) {
  //              auto layoutData = operation->GetOperationParam<PipelineLayoutData>();
  //              if (layoutData && layoutData->data) {
  //                  m_inputAssemblyStage->SetInputLayout(layoutData->data);
  //              }
  //          }
  //          break;
  //      }

  //      case PipelineOperationType::Mesh_Render_SetVertexBuffer: {
  //          if (m_inputAssemblyStage) {
  //              auto vertexData = operation->GetOperationParam<PipelineSetVertexBufferData>();
  //              if (vertexData && vertexData->vertexBuffer) {
  //                  Microsoft::WRL::ComPtr<ID3D11Buffer> pBuffer = vertexData->vertexBuffer;
  //                  m_inputAssemblyStage->SetVertexBuffers(
  //                      vertexData->startSlot,
  //                      vertexData->numBuffers,
  //                      pBuffer.Get(),
  //                      &vertexData->stride,
  //                      &vertexData->offset
  //                  );
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Mesh_Render_SetIndexBuffer: {
  //          if (m_inputAssemblyStage) {
  //              auto indexData = operation->GetOperationParam<PipelineSetIndexBufferData>();
  //              if (indexData && indexData->indexBuffer) {
  //                  m_inputAssemblyStage->SetIndexBuffer(
  //                      indexData->indexBuffer,
  //                      indexData->format,
  //                      indexData->offset
  //                  );
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Mesh_Render_SetPrimitiveToplogy: {
  //          if (m_inputAssemblyStage) {
  //              auto topoData = operation->GetOperationParam<PipelinePrimitiveTopologyData>();
  //              if (topoData) {
  //                  m_inputAssemblyStage->SetPrimitiveTopology(topoData->data);
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Mesh_Render_SetVertexShader:
  //      case PipelineOperationType::Device_Init_SetVertexShader: {
  //          if (m_vertexShaderStage) {
  //              auto shaderData = operation->GetOperationParam<PipelineVertexShaderData>();
  //              if (shaderData && shaderData->data) {
  //                  m_vertexShaderStage->SetShader(shaderData->data);
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Mesh_Render_SetPixelShader:
  //      case PipelineOperationType::Device_Init_SetPixelShader: {
  //          if (m_pixelShaderStage) {
  //              auto shaderData = operation->GetOperationParam<PipelinePixelShaderData>();
  //              if (shaderData && shaderData->data) {
  //                  m_pixelShaderStage->SetShader(shaderData->data);
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Mesh_Render_SetTexture: {
  //          if (m_pixelShaderStage) {
  //              auto textureData = operation->GetOperationParam<PipelineTextureData>();
  //              if (textureData && textureData->data.size()) {
  //                  std::vector<ID3D11ShaderResourceView*> textures = {};
  //                  for (auto texturePair : textureData->data) {
  //                      textures.push_back(texturePair.second);
  //                  }
  //                  m_pixelShaderStage->SetShaderResources(textureData->startSlot, textureData->numTextures, textures.data());
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Mesh_Render_SetSampler: {
  //          if (m_pixelShaderStage) {
  //              auto samplerData = operation->GetOperationParam<PipelineSamplerSateData>();
  //              if (samplerData && samplerData->data) {
  //                  Microsoft::WRL::ComPtr<ID3D11SamplerState> pSampler = samplerData->data;
  //                  m_pixelShaderStage->SetSamplers(samplerData->startSlot, samplerData->numSamplers, &pSampler);
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Device_SetRasterizedState:
  //      case PipelineOperationType::Device_Init_RasterizedState: {
  //          if (m_rasterizerStage) {
  //              auto rasterData = operation->GetOperationParam<PipelineRasteriezeData>();
  //              if (rasterData && rasterData->state) {
  //                  m_rasterizerStage->SetState(rasterData->state);
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Device_SetViewport: {
  //          if (m_rasterizerStage) {
  //              auto viewportData = operation->GetOperationParam<PipelineViewPortData>();
  //              if (viewportData) {
  //                  m_rasterizerStage->SetViewport(viewportData->desc);
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Device_ClearRenderTargetView: {
  //          if (m_outputMergerStage) {
  //              auto rtvData = operation->GetOperationParam<PipelineRenderTargetViewData>();
  //              if (rtvData && rtvData->data) {
  //                  const FLOAT* clearColor = new FLOAT[4]{ rtvData->clearColor.x, rtvData->clearColor.y, rtvData->clearColor.z, rtvData->clearColor.w };
  //                  m_outputMergerStage->ClearRenderTargetView(rtvData->data, clearColor);
  //                  delete[] clearColor;
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Device_ClearDepthStencilView: {
  //          if (m_outputMergerStage) {
  //              auto stencilData = operation->GetOperationParam<PipelineDeepStencilData>();
  //              if (stencilData && stencilData->data) {
  //                  m_outputMergerStage->ClearDepthºlView(
  //                      stencilData->data, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Device_SetDepthStencilState: {
  //          break;
  //      }
  //      case PipelineOperationType::Device_EnabledBledingState:
  //      case PipelineOperationType::Device_DisabledBledingState:
  //      case PipelineOperationType::Device_Init_BledingState: {
  //          break;
  //      }
  //      case PipelineOperationType::Device_PresentSwapChain: {
  //          if (m_outputMergerStage) {
  //              auto swapChainData = operation->GetOperationParam<PipelinePresebtSwapChain>();
  //              if (swapChainData) {
  //                  m_outputMergerStage->Present(swapChainData->data, swapChainData->syncInterval, swapChainData->flags);
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Device_drawIndexed: {
  //          if (m_context) {
  //              auto drawData = operation->GetOperationParam<PipelineDrawIndexedData>();
  //              if (drawData) {
  //                  m_context->DrawIndexed(drawData->numIndexes, 0, 0);
  //              }
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Device_ResetContextState: {
  //          if (m_context) {
  //              m_context->ClearState();
  //          }
  //          break;
  //      }
  //      case PipelineOperationType::Mesh_Render_BindConstantsBuffers:
  //      case PipelineOperationType::Mesh_Init_SamplerState:
  //      case PipelineOperationType::Mesh_Init_InputLayout:
  //      case PipelineOperationType::Mesh_Init_MatrixBuffer:
  //      case PipelineOperationType::Mesh_Render_RestoreState:
  //      case PipelineOperationType::Mesh_Render_SaveState:
  //      case PipelineOperationType::Device_draw:
  //      case PipelineOperationType::Unknown:
  //      case PipelineOperationType::Count:
  //          break;
  //      }*/
  //  }

    void RenderPipelineExecutor::ExecuteOperation(PipelineOperationType operationType, FrameStateService* frameState, PipelineOperBase* parameter) {
        auto it = m_operationsMap.find(operationType);
        if (it != m_operationsMap.end()) {
            it->second.second(operationType, frameState, parameter);
        }
    }
}
