// WorldMatrixManager.cpp
#include "WorldMatrixManager.h"
#include "REGISTER_MANAGER_MACRO.h"
#include <ManagerLocator/ManagerLocator.h>
// Incluir los managers necesarios (ahora completos)
#include "DeviceManager.h"
#include "CameraManager.h"
#include "RenderTargetManager.h"

REGISTER_MANAGER_TYPE(WorldMatrixManager, "WorldMatrixManager");

WorldMatrixManager::WorldMatrixManager() :
    g_matrixBuffer(nullptr),
    g_matrix({}),
    m_device(nullptr),
    m_cameraManager(nullptr),
    m_renderTargetManager(nullptr)
{
}

WorldMatrixManager::~WorldMatrixManager() {
    // Shutdown() debería manejar la liberación, si este es el final del ciclo de vida
    // Si WorldMatrixManager puede ser liberado sin un Shutdown explícito, su destructor debería llamar a Release()
    // Sin embargo, con IShutdownable, el destructor debería estar limpio.
}

// Implementación de IInitializable
HRESULT WorldMatrixManager::Init() {
    OutputDebugStringA("WorldMatrixManager::Init called.\n");

    // Obtener los managers necesarios del ServiceLocator
    m_device = ManagerLocator::GetManager<DeviceManager>();
    if (!m_device) { OutputDebugStringA("ERROR: WorldMatrixManager failed to get DeviceManager.\n"); return E_FAIL; }

	m_cameraManager = ManagerLocator::GetManager<CameraManager>();
	if (!m_cameraManager) { OutputDebugStringA("ERROR: WorldMatrixManager failed to get CameraManager.\n"); return E_FAIL; }

    // Llamar a la inicialización interna del buffer
    return InitInternal(m_device);
}

// La implementación original de Init con DeviceManager
HRESULT WorldMatrixManager::InitInternal(std::shared_ptr<DeviceManager> device) {
    D3D11_BUFFER_DESC matrixBufferDesc = {};
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC; // DYNAMIC para UpdateSubresource
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // Necesario para DYNAMIC

    HRESULT hr = device->GetDevice()->CreateBuffer(&matrixBufferDesc, nullptr, &g_matrixBuffer);
    if (FAILED(hr)) {
        OutputDebugStringA("Error al crear el buffer de matrices.\n");
        return hr;
    }
    OutputDebugStringA("WorldMatrixManager: Matrix buffer created.\n");
    return S_OK;
}

// Método para actualizar las matrices globales que se guardarán en el buffer
void WorldMatrixManager::SetGlobalMatrices(const DirectX::XMMATRIX& world, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection) {
    g_matrix.worldMatrix = DirectX::XMMatrixTranspose(world);
    g_matrix.viewMatrix = DirectX::XMMatrixTranspose(view);
    g_matrix.projectionMatrix = DirectX::XMMatrixTranspose(projection);
}

// Implementación de IRenderable
// Este método se llamará en la fase Render por MainController
void WorldMatrixManager::Render() {

    if (!m_device) {
        OutputDebugStringA("ERROR: WorldMatrixManager::Render called without DeviceManager.\n");
        return;
    }

    // Actualizar el subrecurso con las matrices actuales (g_matrix)
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT hr = m_device->GetContext()->Map(g_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(hr)) {
        OutputDebugStringA("ERROR: WorldMatrixManager failed to map matrix buffer.\n");
        return;
    }
    memcpy(mappedResource.pData, &g_matrix, sizeof(MatrixBufferType));
    m_device->GetContext()->Unmap(g_matrixBuffer, 0);
}

// Implementación de IShutdownable
void WorldMatrixManager::Shutdown() {
    if (g_matrixBuffer) {
        g_matrixBuffer->Release();
        g_matrixBuffer = nullptr;
        OutputDebugStringA("WorldMatrixManager: Matrix buffer released.\n");
    }
}