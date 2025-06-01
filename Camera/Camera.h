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

    // Nuevos m�todos para el movimiento
    void Move(float x, float y, float z);

    // Nuevos m�todos para la rotaci�n
    void Rotate(float pitchOffset, float yawOffset, float rollOffset);

    // Par�metros de la proyecci�n
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
    void SetLookAt(float x, float y, float z);
    void SetLookAt(const XMFLOAT3& target);

    XMFLOAT3 position;
    XMFLOAT3 rotation;


private:
    float m_fieldOfView;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;
};