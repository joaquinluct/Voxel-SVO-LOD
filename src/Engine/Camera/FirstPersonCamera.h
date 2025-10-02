#pragma once
#define NOMINMAX 

#include <d3d11.h>
#include <DirectXMath.h>
//#include <windows.h>
#include <KeyboardManager.h>
#include <ICamera.h>
#include <IService.h>
#include <memory>         // Para std::shared_ptr
#include <vector>         // Para Util::Triangle
#include <limits>         // Para std::numeric_limits
#include <Config/Base/Managers/CameraManagerConfig.h>
#include <Defines/CameraDefinition.h>
#include <RayTracing/RayTracing.h>

class Mouse; // Declaración anticipada para evitar incluir el archivo completo aquí
class World; // Declaración anticipada para evitar incluir el archivo completo aquí

// Redefinir las constantes de DirectXMath si no están disponibles globalmente
#ifndef XM_PIDIV4
#define XM_PIDIV4 0.785398163f // Pi / 4
#endif
#ifndef XM_PIDIV2
#define XM_PIDIV2 1.570796327f // Pi / 2
#endif
#ifndef XM_PI
#define XM_PI 3.1415926535f    // Pi
#endif
#ifndef XM_2PI
#define XM_2PI 6.283185307f    // 2 * Pi
#endif


//// Asegurarse de que CAMERA_SPEED esté definido
//#ifndef CAMERA_SPEED
//#define CAMERA_SPEED 70.0f
//#endif

class FirstPersonCamera : public ICamera, public IService {
public:
    FirstPersonCamera();
    ~FirstPersonCamera() override = default;

    HRESULT Init() override;
    HRESULT PostInit();
    void Render() override {};
    void Update(float deltaTime) override;

    bool UpdateHeight(float deltaTime);
    //void AdjustToTerrain();

    void Shutdown() override {};
    const std::string& GetServiceName() const override {
        static const std::string name = "FirstPersonCamera"; // Esta cadena se crea una sola vez y vive durante toda la ejecución del programa.
        return name;
    }
    static const std::string& GetStaticServiceName()
    {
        static const std::string name = "FirstPersonCamera"; // Esta cadena se crea una sola vez y vive durante toda la ejecución del programa.
        return name;
    }

    // Métodos de ICamera (implementaciones virtuales)    
    DirectX::XMMATRIX GetViewMatrix(bool onUpdate = false) const override;
    DirectX::XMMATRIX GetProjectionMatrix(bool onUpdate = false) const override;
    DirectX::XMFLOAT3 GetPosition() const override;
    DirectX::XMFLOAT3 GetRotation() const override { return m_rotation; };

    void SetPosition(float x, float y, float z) override;
    void SetRotation(float pitch, float yaw, float roll) override;
    void SetLookAt(float x, float y, float z) override;
    void SetLookAt(const DirectX::XMFLOAT3& target) override;
    DirectX::XMFLOAT3 GetLookAt() const;
    void SetProjectionParams(float fieldOfViewRadians, float aspectRatio, float nearPlane, float farPlane) override;

    float GetFieldOfView() const override;
    float GetAspectRatio() const override;
    float GetNearPlane() const override;
    float GetFarPlane() const override;

    void ExtractFrustumPlanes(std::vector<CameraDefinition::FrustumPlane>& frustumPlanes) const override;
    Util::Triangle* GetTriangleLookingAt(const std::vector<Util::Triangle>& triangles) const override;

    // Configuración
    void SetMoveSpeed(float speed) { m_moveSpeed = speed; };
    void SetRotationSpeed(float speed) { m_rotationSpeed = speed; };

    // Métodos para obtener vectores de dirección para movimiento
    DirectX::XMVECTOR GetForwardVector() const;
    DirectX::XMVECTOR GetRightVector() const;
    DirectX::XMVECTOR GetUpVector() const;

    // Debug
    std::string GetDebugInfo() const;

protected:
    float m_lastTerrainHeight = -100000000.0f; // Altura del terreno en el frame anterior
    float m_verticalVelocity = 0.0f;
    const float GRAVITY = -9.8f; // Aceleración de la gravedad en m/s^2
    float m_lastHeight = 0.0f; // Última altura del terreno

    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT3 m_lastPosition;
    DirectX::XMFLOAT3 m_rotation;  // x = pitch, y = yaw, z = roll

    float m_fieldOfView;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;

    float m_moveSpeed;
    float m_rotationSpeed;

    mutable DirectX::XMMATRIX m_viewMatrixCache;
    mutable DirectX::XMMATRIX m_projectionMatrixCache;
    mutable bool m_viewDirty;
    mutable bool m_projectionDirty;

    // Servicios dependientes
    std::shared_ptr<KeyboardManager> m_keyboardManager;
    std::shared_ptr<Mouse> m_mouseService;
    std::shared_ptr<World> m_world;

    // Métodos internos
    DirectX::XMMATRIX GetInternalRotationMatrix() const;
    DirectX::XMVECTOR GetInternalLookAt(DirectX::XMVECTOR eyePos, DirectX::XMMATRIX rotationMatrix) const;
    void RecalculateViewMatrix() const;
    void RecalculateProjectionMatrix() const;
    void Move(float x, float y, float z);
    void Rotate(float pitchOffset, float yawOffset, float rollOffset);
    void UpdateViewMatrix();

    // Debug
    float m_heightDifference = 0.0f; // Diferencia de altura para el ajuste a terreno
};
