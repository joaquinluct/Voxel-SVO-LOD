#include "MainController.h"

MainController::MainController()
{
    g_hwnd = nullptr;
    g_device = new DeviceManager();
    g_renderTarget = new RenderTargetManager();
    g_camera = new Camera();
    g_shaderManager = new ShaderManager();
    g_material = nullptr;
    g_triangulo = nullptr;
	g_matrixBuffer = nullptr;
	g_axis = nullptr;
}

HRESULT MainController::Init(HWND hwnd)
{
    g_hwnd = hwnd;

    HRESULT result = g_device->Init(g_hwnd);
    if (FAILED(result)) return result;

    result = g_renderTarget->Init(g_device);
    if (FAILED(result)) return result;

    /*g_camera->SetPosition(0.0f, 2.0f, -5.0f);
    g_camera->SetRotation(0.0f, 0.0f, 0.0f);*/

    // Cargar shaders
    result = g_shaderManager->LoadShader(g_device->GetDevice(),
        L"SimpleLineShader",
        L"Resources/Shaders/Basic/VertexShader.hlsl",
        L"Resources/Shaders/Basic/PixelShader.hlsl", nullptr, 0);
        /*L"Resources/Shaders/BasicVS.hlsl",
        L"Resources/Shaders/BasicPS.hlsl");*/

    if (FAILED(result)) {
        OutputDebugStringA("Error en la carga de shaders.\n");
        return result;
    }

    // Crear el constant buffer para las matrices
    D3D11_BUFFER_DESC matrixBufferDesc = {};
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    result = g_device->GetDevice()->CreateBuffer(&matrixBufferDesc, nullptr, &g_matrixBuffer);
    if (FAILED(result)) {
        OutputDebugStringA("Error al crear el constant buffer para las matrices.\n");
        return result;
    }


    // Crear material
    g_material = new Material(g_shaderManager, L"SimpleLineShader");

    // Crear triángulo con el material
    g_triangulo = new Triangulo(g_material);
    g_triangulo->Init(g_device->GetDevice());

	/*g_axis = new Axis();
	g_axis->Init(g_device->GetDevice());*/

    return S_OK;
}

void MainController::Cleanup()
{
    if (g_axis) {
        g_axis->Release();
        delete g_axis;
        g_axis = nullptr;
    }

    if (g_triangulo) {
        g_triangulo->Release();
        delete g_triangulo;
        g_triangulo = nullptr;
    }

    if (g_material) {
        delete g_material;
        g_material = nullptr;
    }

    if (g_shaderManager) {
        // Liberar shaders, input layouts y blobs
        for (auto& pair : g_shaderManager->vertexShaders) {
            if (pair.second) pair.second->Release();
        }
        for (auto& pair : g_shaderManager->pixelShaders) {
            if (pair.second) pair.second->Release();
        }
        for (auto& pair : g_shaderManager->inputLayouts) {
            if (pair.second) pair.second->Release();
        }
        for (auto& pair : g_shaderManager->vertexShaderBlobs) {
            if (pair.second) pair.second->Release();
        }
        for (auto& pair : g_shaderManager->pixelShaderBlobs) {
            if (pair.second) pair.second->Release();
        }

        delete g_shaderManager;
        g_shaderManager = nullptr;
    }

    if (g_camera) {
        delete g_camera;
        g_camera = nullptr;
    }

    if (g_renderTarget) {
        g_renderTarget->Release();
        delete g_renderTarget;
        g_renderTarget = nullptr;
    }

    if (g_device) {
        g_device->Release();
        delete g_device;
        g_device = nullptr;
    }
}

struct SimpleVertex {
    float x, y, z, w;  // Posición en espacio homogéneo
    float r, g, b, a;  // Color RGBA
};

SimpleVertex cubeVertices[] = {
    // Frente
    { -1, -1,  1, 1,  1, 0, 0, 1 },  // Rojo
    {  1, -1,  1, 1,  0, 1, 0, 1 },  // Verde
    {  1,  1,  1, 1,  0, 0, 1, 1 },  // Azul
    { -1,  1,  1, 1,  1, 1, 0, 1 },  // Amarillo

    // Atrás
    { -1, -1, -1, 1,  1, 0, 1, 1 },  // Magenta
    {  1, -1, -1, 1,  0, 1, 1, 1 },  // Cyan
    {  1,  1, -1, 1,  0.5, 0.5, 0.5, 1 },  // Gris
    { -1,  1, -1, 1,  1, 1, 1, 1 }   // Blanco
};

struct MatrixBufferType {
    DirectX::XMMATRIX worldViewProjection;
};

#include "MainController.h"
#include <DirectXMath.h> // Para operaciones con matrices
using namespace DirectX;

void MainController::Render()
{
    g_renderTarget->SetRenderTargets();
    g_renderTarget->SetViewport();

    // Limpia la pantalla
    FLOAT clearColor[4] = { 0.2f, 0.2f, 0.2f, 1.0f }; // Gris oscuro
    g_renderTarget->Clear(clearColor);

    ID3D11DeviceContext* context = g_device->GetContext();
    ID3D11Device* device = g_device->GetDevice();

    //context->RSSetState(nullptr); // Estado de rasterizador predeterminado
    //context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF); // Estado de mezcla predeterminado (no blending)
    //context->OMSetDepthStencilState(nullptr, 0); // Estado de profundidad/stencil predeterminado (sin pruebas)

    // 1. Definir matrices de transformación
    XMMATRIX worldMatrix = XMMatrixIdentity();
    XMMATRIX viewMatrix = XMMatrixLookAtLH(
        XMVectorSet(0.0f, 2.0f, -5.0f, 1.0f),
        XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
        XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
    );
    XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(XM_PIDIV4, 1280.0f / 720.0f, 0.1f, 100.0f);
    XMMATRIX worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;

    // 2. Pasar la matriz al constant buffer
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = context->Map(g_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) return;

    MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
    dataPtr->worldMatrix = XMMatrixTranspose(worldMatrix);
    dataPtr->viewMatrix = XMMatrixTranspose(viewMatrix);
    dataPtr->projectionMatrix = XMMatrixTranspose(projectionMatrix);

    context->Unmap(g_matrixBuffer, 0);
    context->VSSetConstantBuffers(0, 1, &g_matrixBuffer);

    // 3. Crear el buffer de vértices
    UINT numVertices = ARRAYSIZE(cubeVertices);
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.ByteWidth = sizeof(SimpleVertex) * numVertices;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = cubeVertices;
    ID3D11Buffer* vertexBuffer;
    HRESULT hr = device->CreateBuffer(&bd, &InitData, &vertexBuffer);
    if (FAILED(hr)) return;

    // 4. Definir el input layout
    D3D11_INPUT_ELEMENT_DESC layoutDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };
    UINT numElements = ARRAYSIZE(layoutDesc);
    ID3D11InputLayout* inputLayout;
    hr = device->CreateInputLayout(layoutDesc, numElements, g_shaderManager->GetVertexShaderBytecode(L"SimpleLineShader")->GetBufferPointer(), g_shaderManager->GetVertexShaderBytecode(L"SimpleLineShader")->GetBufferSize(), &inputLayout);
    if (FAILED(hr)) return;

    // 5. Configurar la topología y buffers
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context->IASetInputLayout(inputLayout);

    // 6. Activar los shaders
	ID3D11VertexShader* vs = g_shaderManager->GetVertexShader(L"SimpleLineShader");
	ID3D11PixelShader* ps = g_shaderManager->GetPixelShader(L"SimpleLineShader");
    context->VSSetShader(vs, nullptr, 0);
    context->PSSetShader(ps, nullptr, 0);

    // 7. Dibujar el cubo
    context->Draw(numVertices, 0);

    // 8. Limpiar recursos locales
    vertexBuffer->Release();
    inputLayout->Release();

    g_device->GetSwapChain()->Present(1, 0);
}



//void ZZZZRender()
//{
//    g_renderTarget->SetRenderTargets();
//    g_renderTarget->SetViewport();
//
//    // Limpia la pantalla
//    FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
//    g_renderTarget->Clear(clearColor);
//
//    ID3D11DeviceContext* context = g_device->GetContext();
//
//    // Activar los shaders
//    ID3D11VertexShader* vs = g_shaderManager->GetVertexShader(L"BasicShader");
//    ID3D11PixelShader* ps = g_shaderManager->GetPixelShader(L"BasicShader");
//    ID3D11InputLayout* layout = g_shaderManager->GetInputLayout(L"BasicShader"); // 
//
//    context->VSSetShader(vs, nullptr, 0);
//    context->PSSetShader(ps, nullptr, 0);
//    context->IASetInputLayout(layout); // Establecer el input layout
//
//    // Obtener y configurar las matrices (como antes)
//    XMMATRIX worldMatrix = XMMatrixIdentity();
//    XMMATRIX viewMatrix = g_camera->GetViewMatrix();
//    XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(XM_PI / 4.0f, 1.0f, 0.1f, 100.0f);
//
//    worldMatrix = XMMatrixTranspose(worldMatrix);
//    viewMatrix = XMMatrixTranspose(viewMatrix);
//    projectionMatrix = XMMatrixTranspose(projectionMatrix);
//
//    D3D11_MAPPED_SUBRESOURCE mappedResource;
//    HRESULT result = context->Map(g_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
//    if (FAILED(result)) {
//        OutputDebugStringA("Error al mapear el constant buffer.\n");
//        return;
//    }
//
//    MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
//    dataPtr->worldMatrix = worldMatrix;
//    dataPtr->viewMatrix = viewMatrix;
//    dataPtr->projectionMatrix = projectionMatrix;
//
//    context->Unmap(g_matrixBuffer, 0);
//    context->VSSetConstantBuffers(0, 1, &g_matrixBuffer);
//
//    // Renderizar el triángulo
//    g_triangulo->Render(context);
//
//    // Renderizar los ejes (ahora pasamos el input layout)
//    //g_axis->Render(context, layout);
//
//    g_device->GetSwapChain()->Present(1, 0);
//}
