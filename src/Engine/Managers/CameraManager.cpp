#include "CameraManager.h"
#include <ManagerLocator/ManagerLocator.h>
#include "REGISTER_MANAGER_MACRO.h"
#include "Camera/FirstPersonCamera.h"
#include "KeyboardManager.h"
#include <windows.h>
#include <ServiceLocator/ServiceLocator.h>

using namespace DirectX;

// Auto-registro del CameraManager
REGISTER_MANAGER_TYPE(CameraManager, "CameraManager")

CameraManager::CameraManager() : m_hwnd(nullptr), m_width(0), m_height(0), m_currentCamera(nullptr), m_cameras(), m_config() {}

CameraManager::~CameraManager() {
    m_cameras.clear(); // Limpiar el mapa de cámaras
    m_currentCamera = nullptr;
}

HRESULT CameraManager::Init(HWND hwnd, int width, int height) {
    OutputDebugStringA("Incializando CameraManager...\n");
    m_hwnd = hwnd;
    m_width = width;
    m_height = height;

    m_config = new CameraManagerConfig();

    HRESULT hr = ServiceLocator::InitializeServices(m_config->camerasTypes);
    if (FAILED(hr)) {
        OutputDebugStringA("MainController Init: Failed to initialize services.\n");
        return hr;
    }

    if (m_config->cameras.size() <= 0) {
        OutputDebugStringA("CameraManager: No hay cámaras en la configuración.\n");
        return E_FAIL;
    }

    int index = 0;
    std::string firstName = "";
    for (auto& camName : m_config->cameras) {
        if (firstName.empty()) {
            firstName = camName;
        }
        if (m_config->camerasTypes.size() <= index) {
            continue; // Evitar acceso fuera de rango
		}
        std::string camType = m_config->camerasTypes[index];
        if (camType == "FirstPersonCamera") {
            std::shared_ptr<FirstPersonCamera> cam = ServiceLocator::GetService<FirstPersonCamera>();
            AddCamera(camName, cam);
        }
        index++;
    }

    if (m_config->initialCamera.empty()) {
        SetCurrentCamera(firstName);
    }
    else {
        SetCurrentCamera(m_config->initialCamera);
    }

    hr = S_OK;
    OutputDebugStringA(("Resultado Init " + std::to_string(hr) + " en CameraManager\n").c_str());
    return hr;
}

void CameraManager::Update(float deltaTime) {
    if (m_currentCamera) {
        m_currentCamera->Update(deltaTime); // Delegar la actualización a la cámara activa
    }
}

void CameraManager::Render() {
    // CameraManager no tiene nada que renderizar directamente
    // Puedes dejarlo vacío o con un OutputDebugStringA si quieres
}

void CameraManager::Shutdown() {
    m_cameras.clear();
    m_currentCamera = nullptr;
    m_keyboardManager = nullptr; // Liberar la referencia al KeyboardManager
    OutputDebugStringA("CameraManager: Shutdown completo.\n");
}

void CameraManager::AddCamera(const std::string& name, std::shared_ptr<ICamera> camera) {
    if (m_cameras.find(name) == m_cameras.end()) { // Usa find para comprobar existencia
        m_cameras[name] = camera;
    }
    else {
        OutputDebugStringA(("CameraManager: Una cámara con el nombre '" + name + "' ya existe.\n").c_str());
    }
}

std::shared_ptr<ICamera> CameraManager::GetCamera(const std::string& name) {
    auto it = m_cameras.find(name);
    if (it != m_cameras.end()) {
        return it->second;
    }
    return nullptr;
}

void CameraManager::SetCurrentCamera(const std::string& name) {
    std::shared_ptr<ICamera> camera = GetCamera(name);
    if (camera) {
        m_currentCamera = camera;
        OutputDebugStringA(("CameraManager: Cámara actual establecida a '" + name + "'.\n").c_str());
    }
    else {
        OutputDebugStringA(("CameraManager: No se encontró la cámara '" + name + "' para establecer como actual.\n").c_str());
    }
}

std::shared_ptr<ICamera> CameraManager::GetCurrentCamera() const {
    return m_currentCamera;
}

// Métodos delegados a la cámara activa
DirectX::XMMATRIX CameraManager::GetCurrentViewMatrix() const {
    if (m_currentCamera) {
        return m_currentCamera->GetViewMatrix();
    }
    return XMMatrixIdentity();
}

DirectX::XMMATRIX CameraManager::GetCurrentProjectionMatrix() const {
    if (m_currentCamera) {
        return m_currentCamera->GetProjectionMatrix();
    }
    return XMMatrixIdentity();
}

const DirectX::XMFLOAT3 CameraManager::GetCurrentCameraPosition() {
    if (m_currentCamera) {
        return m_currentCamera->GetPosition();
    }
    return XMFLOAT3(0.0f, 0.0f, 0.0f);
}

DirectX::XMFLOAT3 CameraManager::GetCurrentCameraRotation() const {
    if (m_currentCamera) {
        return m_currentCamera->GetRotation();
    }
    return XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void CameraManager::ExtractCurrentFrustumPlanes(DirectX::XMFLOAT4 planes[6]) const {
    if (m_currentCamera) {
        m_currentCamera->ExtractFrustumPlanes(planes);
    }
    else {
        // Opcional: inicializar planos a valores inválidos o identidad
        for (int i = 0; i < 6; ++i) {
            planes[i] = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
        }
    }
}

Util::Triangle* CameraManager::GetTriangleLookingAt(const std::vector<Util::Triangle>& triangles) const {
    if (m_currentCamera) {
        return m_currentCamera->GetTriangleLookingAt(triangles);
    }
    return nullptr;
}