#include "WorldMatrixManager.h"

WorldMatrixManager::WorldMatrixManager()
{
	g_matrixBuffer = nullptr;
	g_matrix = {};
}

WorldMatrixManager::~WorldMatrixManager()
{
}

HRESULT WorldMatrixManager::Init(DeviceManager* g_device)
{
    D3D11_BUFFER_DESC matrixBufferDesc = {};
    matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = 0;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    HRESULT hr = g_device->GetDevice()->CreateBuffer(&matrixBufferDesc, nullptr, &g_matrixBuffer);
    if (FAILED(hr)) {
        OutputDebugString(L"Error al crear el buffer de matrices.\n");
        return hr;
    }
	return S_OK;
}

HRESULT WorldMatrixManager::Init()
{
	// Aquí puedes inicializar el buffer de matrices si es necesario
	return S_OK;
}

HRESULT WorldMatrixManager::Render()
{
	// Aquí puedes implementar el renderizado de matrices si es necesario
	return S_OK;
}

HRESULT WorldMatrixManager::Render(DeviceManager* g_device, RenderTargetManager* g_renderTarget, Camera* g_camera, const XMMATRIX& worldMatrix = XMMatrixIdentity()) const
{
	// Obtener las matrices del mundo, vista y proyección
	float screenWidth = g_renderTarget->GetViewport().Width;
	float screenHeight = g_renderTarget->GetViewport().Height;	
	XMMATRIX viewMatrix = g_camera->GetViewMatrix(); // Aquí obtenemos la vista
	XMMATRIX projectionMatrix = XMMatrixPerspectiveFovLH(XM_PI / 4.0f, screenWidth / screenHeight, 0.1f, 1000.0f);
	//XMMATRIX projectionMatrix = XMMatrixOrthographicLH(screenWidth, screenHeight, 0.1f, 10000.0f);

	// Enviar matrices al shader
	// Crear y actualizar la estructura de matrices antes del renderizado
	MatrixBufferType matrices{};
	matrices.worldMatrix = XMMatrixTranspose(worldMatrix); // Ahora se usa la variable local worldMatrix
	matrices.viewMatrix = XMMatrixTranspose(viewMatrix);  // Se usa la variable local viewMatrix (por consistencia)
	matrices.projectionMatrix = XMMatrixTranspose(projectionMatrix); // Se usa la variable local projectionMatrix (por consistencia)
	//matrices.projectionMatrix = projectionMatrix;

	// Copiar matrices al buffer constante
	g_device->GetContext()->UpdateSubresource(g_matrixBuffer, 0, NULL, &matrices, 0, 0);

	// Vincular buffer con el shader
	g_device->GetContext()->VSSetConstantBuffers(0, 1, &g_matrixBuffer);
	return S_OK;
}

HRESULT WorldMatrixManager::SetVertexShaderConstantBuffer(ID3D11DeviceContext* context, UINT slot, ID3D11Buffer* buffer)
{
	context->VSSetConstantBuffers(slot, 1, &buffer);
	return S_OK;
}

HRESULT WorldMatrixManager::Release()
{
	if (g_matrixBuffer) {
		g_matrixBuffer->Release();
		g_matrixBuffer = nullptr;
	}
	return S_OK;
}