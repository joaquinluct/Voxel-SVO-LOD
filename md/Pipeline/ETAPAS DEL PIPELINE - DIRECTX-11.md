# 🎯 Pipeline gráfico de DirectX 11 – Etapas, operaciones y detalles

Este documento enumera las etapas del pipeline gráfico programable en DirectX 11, con las operaciones clave que se pueden realizar en cada una, incluyendo:

- Recursos afectados
- Parámetros utilizados
- Tipo de operación (Create, Read, Update, Delete)

---

## 🧩 Etapa: IA – Input Assembler

| Operación               | Recursos afectados         | Parámetros principales                         | CRUD |
|------------------------|----------------------------|------------------------------------------------|------|
| `IASetInputLayout`     | Input Layout               | `ID3D11InputLayout*`                           | R    |
| `IASetVertexBuffers`   | Vertex Buffer              | `ID3D11Buffer*`, `stride`, `offset`            | R    |
| `IASetIndexBuffer`     | Index Buffer               | `ID3D11Buffer*`, `format`, `offset`            | R    |
| `IASetPrimitiveTopology` | Topología de primitivas  | `D3D11_PRIMITIVE_TOPOLOGY`                     | R    |

---

## 🧩 Etapa: VS – Vertex Shader

| Operación               | Recursos afectados         | Parámetros principales                         | CRUD |
|------------------------|----------------------------|------------------------------------------------|------|
| `VSSetShader`          | Vertex Shader              | `ID3D11VertexShader*`                          | R    |
| `VSSetConstantBuffers` | Constant Buffers           | `ID3D11Buffer*[]`, `startSlot`, `numBuffers`   | R    |
| `VSSetShaderResources` | Texturas / SRVs            | `ID3D11ShaderResourceView*[]`                  | R    |
| `VSSetSamplers`        | Sampler States             | `ID3D11SamplerState*[]`                        | R    |

---

## 🧩 Etapa: HS – Hull Shader (Teselación)

| Operación               | Recursos afectados         | Parámetros principales                         | CRUD |
|------------------------|----------------------------|------------------------------------------------|------|
| `HSSetShader`          | Hull Shader                | `ID3D11HullShader*`                            | R    |
| `HSSetConstantBuffers` | Constant Buffers           | `ID3D11Buffer*[]`                              | R    |
| `HSSetShaderResources` | Texturas / SRVs            | `ID3D11ShaderResourceView*[]`                  | R    |
| `HSSetSamplers`        | Sampler States             | `ID3D11SamplerState*[]`                        | R    |

---

## 🧩 Etapa: DS – Domain Shader (Teselación)

| Operación               | Recursos afectados         | Parámetros principales                         | CRUD |
|------------------------|----------------------------|------------------------------------------------|------|
| `DSSetShader`          | Domain Shader              | `ID3D11DomainShader*`                          | R    |
| `DSSetConstantBuffers` | Constant Buffers           | `ID3D11Buffer*[]`                              | R    |
| `DSSetShaderResources` | Texturas / SRVs            | `ID3D11ShaderResourceView*[]`                  | R    |
| `DSSetSamplers`        | Sampler States             | `ID3D11SamplerState*[]`                        | R    |

---

## 🧩 Etapa: GS – Geometry Shader

| Operación               | Recursos afectados         | Parámetros principales                         | CRUD |
|------------------------|----------------------------|------------------------------------------------|------|
| `GSSetShader`          | Geometry Shader            | `ID3D11GeometryShader*`                        | R    |
| `GSSetConstantBuffers` | Constant Buffers           | `ID3D11Buffer*[]`                              | R    |
| `GSSetShaderResources` | Texturas / SRVs            | `ID3D11ShaderResourceView*[]`                  | R    |
| `GSSetSamplers`        | Sampler States             | `ID3D11SamplerState*[]`                        | R    |

---

## 🧩 Etapa: RS – Rasterizer Stage

| Operación               | Recursos afectados         | Parámetros principales                         | CRUD |
|------------------------|----------------------------|------------------------------------------------|------|
| `RSSetState`           | Rasterizer State           | `ID3D11RasterizerState*`                       | R    |
| `RSSetViewports`       | Viewport                   | `D3D11_VIEWPORT[]`, `numViewports`             | R    |
| `RSSetScissorRects`    | Scissor Rects              | `D3D11_RECT[]`, `numRects`                     | R    |

---

## 🧩 Etapa: PS – Pixel Shader

| Operación               | Recursos afectados         | Parámetros principales                         | CRUD |
|------------------------|----------------------------|------------------------------------------------|------|
| `PSSetShader`          | Pixel Shader               | `ID3D11PixelShader*`                           | R    |
| `PSSetConstantBuffers` | Constant Buffers           | `ID3D11Buffer*[]`                              | R    |
| `PSSetShaderResources` | Texturas / SRVs            | `ID3D11ShaderResourceView*[]`                  | R    |
| `PSSetSamplers`        | Sampler States             | `ID3D11SamplerState*[]`                        | R    |

---

## 🧩 Etapa: OM – Output Merger

| Operación               | Recursos afectados         | Parámetros principales                         | CRUD |
|------------------------|----------------------------|------------------------------------------------|------|
| `OMSetRenderTargets`   | RTVs / DSVs                | `ID3D11RenderTargetView*[]`, `ID3D11DepthStencilView*` | R |
| `OMSetBlendState`      | Blend State                | `ID3D11BlendState*`, `blendFactor`, `sampleMask` | R |
| `OMSetDepthStencilState` | Depth-Stencil State      | `ID3D11DepthStencilState*`, `stencilRef`       | R |

---

## 🧩 Etapa: CS – Compute Shader

| Operación               | Recursos afectados         | Parámetros principales                         | CRUD |
|------------------------|----------------------------|------------------------------------------------|------|
| `CSSetShader`          | Compute Shader             | `ID3D11ComputeShader*`                         | R    |
| `CSSetConstantBuffers` | Constant Buffers           | `ID3D11Buffer*[]`                              | R    |
| `CSSetShaderResources` | Texturas / SRVs            | `ID3D11ShaderResourceView*[]`                  | R    |
| `CSSetSamplers`        | Sampler States             | `ID3D11SamplerState*[]`                        | R    |
| `Dispatch`             | GPU Threads                | `x, y, z` thread groups                        | C    |
| `DispatchIndirect`     | GPU Threads (indirect)     | `ID3D11Buffer*` with dispatch args             | C    |

---

## 🧠 Operaciones adicionales clave

| Operación                      | Recursos afectados         | Parámetros principales                         | CRUD |
|-------------------------------|----------------------------|------------------------------------------------|------|
| `CreateBuffer`                | Buffers                    | `D3D11_BUFFER_DESC`, `D3D11_SUBRESOURCE_DATA`   | C    |
| `CreateTexture2D`             | Texturas                   | `D3D11_TEXTURE2D_DESC`, `initialData`          | C    |
| `CreateShaderResourceView`    | SRVs                       | `ID3D11Resource*`, `D3D11_SHADER_RESOURCE_VIEW_DESC` | C |
| `CreateRenderTargetView`      | RTVs                       | `ID3D11Resource*`, `D3D11_RENDER_TARGET_VIEW_DESC` | C |
| `CreateDepthStencilView`      | DSVs                       | `ID3D11Resource*`, `D3D11_DEPTH_STENCIL_VIEW_DESC` | C |
| `ClearRenderTargetView`       | RTV                        | `ID3D11RenderTargetView*`, `ColorRGBA[4]`      | U    |
| `ClearDepthStencilView`       | DSV                        | `ID3D11DepthStencilView*`, `flags`, `depth`, `stencil` | U |
| `Present`                     | Swap Chain                 | `syncInterval`, `flags`                        | R    |
| `Draw`, `DrawIndexed`         | Pipeline                   | `vertexCount`, `startVertex`, etc.             | R    |

---

## 📌 Notas

- CRUD se interpreta como:  
  - **C**: Create  
  - **R**: Read / Bind  
  - **U**: Update / Clear / Dispatch  
  - **D**: Delete (implícito al liberar recursos)

- Las operaciones `Set*` son generalmente **binds** (Read), no crean ni destruyen recursos.

- Las operaciones `Create*` deben ser liberadas manualmente o mediante `ComPtr`.

