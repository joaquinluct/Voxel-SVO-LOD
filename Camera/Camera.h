#pragma once
#include "..\Interfaces\iCamera.h"
#include <array>

class Camera : public iCamera {
public:
    Camera();
    void SetPosition(float x, float y, float z) override;
    void SetRotation(float pitch, float yaw, float roll) override;
    XMMATRIX GetViewMatrix() const;
    XMMATRIX GetProjectionMatrix() const;
    XMFLOAT3 GetPosition() const override { return position; };

    // Nuevos métodos para el movimiento
    void Move(float x, float y, float z);

    // Nuevos métodos para la rotación
    void Rotate(float pitchOffset, float yawOffset, float rollOffset);

    // Parámetros de la proyección
    void SetProjectionParams(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);

    // Getters para Pitch, Yaw y Roll (necesarios para Mouse y Keyboard)
    float GetPitch() const { return rotation.x; }
    float GetYaw() const { return rotation.y; }
    float GetRoll() const { return rotation.z; }
    float GetFarPlane() const { return m_farPlane;  }
    XMMATRIX GetRotationMatrix() const;
    XMVECTOR GetLookAt(XMVECTOR eyePos, XMMATRIX rotationMatrix) const;
    XMFLOAT3 GetLookAtPosition();
    void ExtractFrustumPlanes(XMFLOAT4 planes[6]) const;
    XMFLOAT3 position;
    XMFLOAT3 rotation;


private:
    float m_fieldOfView;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;
};