#pragma once

#include "ICamera.h"
#include "IService.h"
#include <DirectXMath.h>
#include <memory>         // Para std::shared_ptr
#include <vector>         // Para Util::Triangle
#include <limits>         // Para std::numeric_limits
#include "RayTracing/RayTracing.h"   
#include <KeyboardManager.h>

// ... (tus #defines y forward declarations existentes) ...

class FirstPersonCamera : public ICamera, public IService {
public:
    FirstPersonCamera();
    ~FirstPersonCamera() override = default;

    HRESULT Init() override;
    void Render() override {};
    void Update(float deltaTime) override;
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
    DirectX::XMMATRIX GetViewMatrix() const override;
    DirectX::XMMATRIX GetProjectionMatrix() const override;
    DirectX::XMFLOAT3 GetPosition() const override;
    DirectX::XMFLOAT3 GetRotation() const override { return m_rotation; };

    void SetPosition(float x, float y, float z) override; // Añadir override
    void SetRotation(float pitch, float yaw, float roll) override; // Añadir override
    void SetLookAt(float x, float y, float z) override; // Añadir override
    void SetLookAt(const DirectX::XMFLOAT3& target) override; // Añadir override
    void SetProjectionParams(float fieldOfViewRadians, float aspectRatio, float nearPlane, float farPlane) override; // Añadir override

    // --- ¡AÑADIR ESTAS DECLARACIONES! ---
    float GetFieldOfView() const override;
    float GetAspectRatio() const override;
    float GetNearPlane() const override;
    float GetFarPlane() const override;
    // --- FIN DE AÑADIR ---

    void ExtractFrustumPlanes(DirectX::XMFLOAT4 planes[6]) const override;
    Util::Triangle* GetTriangleLookingAt(const std::vector<Util::Triangle>& triangles) const override; // Añadir override

    void SetMoveSpeed(float speed) { m_moveSpeed = speed; };
    void SetRotationSpeed(float speed) { m_rotationSpeed = speed; };

protected:
    DirectX::XMFLOAT3 m_position;
    DirectX::XMFLOAT3 m_rotation;

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

    std::shared_ptr<KeyboardManager> m_keyboardManager;

    DirectX::XMMATRIX GetInternalRotationMatrix() const;
    DirectX::XMVECTOR GetInternalLookAt(DirectX::XMVECTOR eyePos, DirectX::XMMATRIX rotationMatrix) const;
    void RecalculateViewMatrix() const;
    void RecalculateProjectionMatrix() const;
    void Move(float x, float y, float z);
    void Rotate(float pitchOffset, float yawOffset, float rollOffset);
};