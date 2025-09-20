#pragma once

#include <DirectXMath.h>            // Para XMMATRIX, XMFLOAT3, etc.
#include <map>                      // Para almacenar múltiples cámaras por nombre
#include <memory>                   // Para std::shared_ptr
#include "ICamera.h"                // La nueva interfaz base para cámaras
#include <string>                   // Para los nombres de las cámaras
#include <ManagerBase.h>
#include <Config/Base/Managers/CameraManagerConfig.h>
#include <Defines/CameraDefinition.h>

// Forward declaration
class KeyboardManager;

namespace Util {
    struct Triangle; // For ray picking
}

class CameraManager  : public ManagerBase {
public:
    CameraManager();
    ~CameraManager();

    HRESULT Init(EngineContext* context) override;
    HRESULT PostInit();
    void Update(float deltaTime) override;
    void Render() override;
    void Shutdown() override;

    const std::string& GetManagerName() const override { 
		static const std::string& name = "CameraManager";
        return name;
    }
    static const std::string& GetStaticManagerName()
    {
        static const std::string name = "CameraManager";
        return name;
    }

    // Métodos para gestionar las cámaras
    void AddCamera(const std::string& name, std::shared_ptr<ICamera> camera);
    std::shared_ptr<ICamera> GetCamera(const std::string& name);
    void SetCurrentCamera(const std::string& name);
    std::shared_ptr<ICamera> GetCurrentCamera() const;

    // Métodos de acceso a la cámara actual (delegados a m_currentCamera)
    DirectX::XMMATRIX GetCurrentViewMatrix(bool onUpdate = false) const;
    DirectX::XMMATRIX GetCurrentProjectionMatrix(bool onUpdate = false) const;
    const DirectX::XMFLOAT3 GetCurrentCameraPosition();
    DirectX::XMFLOAT3 GetCurrentCameraRotation() const; // Nuevo acceso a la rotación
    DirectX::XMFLOAT3 GetCurrentLookAt() const;

    // Métodos de utilidad de la cámara actual (delegados a m_currentCamera)
    void ExtractCurrentFrustumPlanes(std::vector<CameraDefinition::FrustumPlane>& frustumPlanes) const;
    Util::Triangle* GetTriangleLookingAt(const std::vector<Util::Triangle>& triangles) const;

    std::string GetDebugInfo() const;

    // Métodos para cambiar parámetros de la cámara actual (puede ser peligroso si la cámara no es del tipo esperado)
    // Es mejor acceder a la cámara y cambiar sus parámetros directamente si el tipo es conocido.
    // Por ejemplo: std::static_pointer_cast<FirstPersonCamera>(GetCurrentCamera())->m_moveSpeed = newSpeed;

protected:
	CameraManagerConfig* m_config; // Configuración del administrador de cámaras
    std::map<std::string, std::shared_ptr<ICamera>> m_cameras;
    std::shared_ptr<ICamera> m_currentCamera;

    // Dependencias
    std::shared_ptr<KeyboardManager> m_keyboardManager;

    // Información de la ventana (para inicializar cámaras con el aspect ratio correcto)
    HWND* m_hwnd;
    int m_width;
    int m_height;
};