#include "AxisXYZ.h"
#include <REGISTER_SERVICE_MACRO.h>
#include <ManagerLocator/ManagerLocator.h>
#include <ShaderManager.h>
#include <../Resources/resource.h>

REGISTER_SERVICE_TYPE(AxisXYZ, "AxisXYZ")

AxisXYZ::AxisXYZ() : m_material(nullptr), m_deviceManager(nullptr), m_worldMatrixManager(nullptr) {}

AxisXYZ::~AxisXYZ() {
    Shutdown();
}

HRESULT AxisXYZ::Init() {

	std::shared_ptr<ShaderManager> m_shaderManager = ManagerLocator::GetManager<ShaderManager>();
	m_deviceManager = ManagerLocator::GetManager<DeviceManager>();

    m_material = new Material(SHADER_BASE);
    m_material->Init(m_deviceManager->GetDevice());

    if (!m_deviceManager) {
        return E_FAIL; // DeviceManager not found
    }
    ID3D11Device* device = m_deviceManager->GetDevice();
    if (!device) {
        return E_FAIL; // Device not found
    }

	m_worldMatrixManager = ManagerLocator::GetManager<WorldMatrixManager>();
    if (!m_worldMatrixManager) {
        return E_FAIL; // WorldMatrixManager not found
    }

	m_cameraManager = ManagerLocator::GetManager<CameraManager>();
    if (!m_cameraManager) {
        return E_FAIL; // CameraManager not found
	}
    
    // Definir los ejes y sus colores
    std::vector<Line*> m_lines;
	std::vector<UIBox*> boxes;
    
	float size = 1000.0f; // Tamaño de los ejes
    float originX = 0.0f;
    float originY = 0.0f;
    float originZ = 0.0f;

    // Definir los puntos y colores para los ejes
    XMFLOAT3 origin = { originX, originY, originZ };
    XMFLOAT3 xAxisEnd = { size, originY, originZ };
    XMFLOAT3 yAxisEnd = { originX, size, originZ };
    XMFLOAT3 zAxisEnd = { originX, originY, size };
    XMFLOAT4 red = { size, 0.0f, 0.0f, size };
    XMFLOAT4 green = { 0.0f, size, 0.0f, size };
    XMFLOAT4 blue = { 0.0f, 0.0f, size, size };

    XMFLOAT3 originBoxX = { 10.0f, .0f, .0f };
    XMFLOAT3 originBoxY = { .0f, 10.0f, .0f };
    XMFLOAT3 originBoxZ = { .0f, .0f, 10.0f };

    // Crear objetos Line
    m_lines.push_back(new Line(m_material, origin, xAxisEnd, red));
    m_lines.push_back(new Line(m_material, origin, yAxisEnd, green));
    m_lines.push_back(new Line(m_material, origin, zAxisEnd, blue));

    // Inicializar las líneas
    for (Line* line : m_lines) {
        if (FAILED(line->Init(device))) {
            return E_FAIL; // Handle error appropriately
        }
    }

    boxes.push_back(new UIBox(m_material, originBoxX, 2.2f, 2.2f, 2.2f, red));
    boxes.push_back(new UIBox(m_material, originBoxY, 2.2f, 2.2f, 2.2f, green));
    boxes.push_back(new UIBox(m_material, originBoxZ, 2.2f, 2.2f, 2.2f, blue));

    for (UIBox* box : boxes) {
        box->Init(device);
    }

    m_deviceManager->InitRasterizedState();
    
    return S_OK;
}

void AxisXYZ::Render() {
    // Renderizar cada línea
	ID3D11DeviceContext* context = m_deviceManager->GetContext();

    DirectX::XMMATRIX worldIdentity = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX viewIdentity = m_cameraManager->GetCurrentViewMatrix();
    DirectX::XMMATRIX projIdentity = m_cameraManager->GetCurrentProjectionMatrix();

    // Aquí le pasamos al WorldMatrixManager las matrices que necesita para este dibujado.
    // Él se encargará de actualizar su buffer interno.
    m_worldMatrixManager->SetGlobalMatrices(worldIdentity, viewIdentity, projIdentity);

    for (Line* line : m_lines) {
        line->Render(context);
    }
    for (UIBox* box: boxes) {
        box->Render(context);
    }
}

void AxisXYZ::Shutdown() {
    // Liberar y eliminar cada línea
    for (Line* line : m_lines) {
        line->Release();
        delete line;
    }
    m_lines.clear();
}