#pragma once

#include "RayTracing/RayTracing.h" // Asumiendo que Triangle es una estructura definida en Util
#include <Defines/CameraDefinition.h>
#include <Defines/Enums/Camera.h>
#include <DirectXMath.h>
#include <IDebug.h>
#include <vector> // Para el ray tracing
#include <Windows.h>

// Forward declarations para evitar dependencias circulares si KeyboardManager o Util son definidos en otro lado
//class KeyboardManager; // Asumiendo que KeyboardManager es una clase y no un namespace
//namespace Util {
//    struct Triangle;
//    class RayTracing; // Asumiendo que RayTracing es una clase dentro del namespace Util
//}

class ICamera : public IDebug {
protected:
    CameraMode m_currentMode = CameraMode::Gravity; // Inicia con gravedad.
    bool m_switchCamTypePressed = false;
public:
    virtual ~ICamera() = default;

    virtual HRESULT PostInit() = 0;

    // M�todos para obtener/establecer la posici�n y rotaci�n de la c�mara
    virtual DirectX::XMFLOAT3 GetPosition() const = 0;
    virtual DirectX::XMFLOAT3 GetRotation() const = 0; // Pitch, Yaw, Roll en radianes

    virtual void SetPosition(float x, float y, float z) = 0;
    virtual void SetRotation(float pitch, float yaw, float roll) = 0; // pitch, yaw, roll en radianes

    // M�todos para establecer la c�mara mirando a un punto espec�fico
    virtual void SetLookAt(float x, float y, float z) = 0;
    virtual void SetLookAt(const DirectX::XMFLOAT3& target) = 0;

    virtual DirectX::XMFLOAT3 GetLookAt() const = 0; // Agrega esto

    // M�todos para obtener las matrices de vista y proyecci�n
    virtual DirectX::XMMATRIX GetViewMatrix(bool onUpdate) const = 0;
    virtual DirectX::XMMATRIX GetProjectionMatrix(bool onUpdate) const = 0;

    // M�todos para configurar los par�metros de proyecci�n de la c�mara
    virtual void SetProjectionParams(float fieldOfViewRadians, float aspectRatio, float nearPlane, float farPlane) = 0;

    // M�todos para obtener los par�metros de proyecci�n (�til para debug o UIs)
    virtual float GetFieldOfView() const = 0;    // En radianes
    virtual float GetAspectRatio() const = 0;
    virtual float GetNearPlane() const = 0;
    virtual float GetFarPlane() const = 0;

    // M�todo de actualizaci�n que maneja la l�gica de movimiento/input
    virtual void Update(float deltaTime) = 0;

    // M�todos de utilidad que dependen del estado de la c�mara
    virtual void ExtractFrustumPlanes(std::vector<CameraDefinition::FrustumPlane>& frustumPlanes) const = 0;

    // Ray tracing / picking. NOTA: Este m�todo acopla la c�mara a Util::Triangle y Util::RayTracing.
    // En un sistema muy grande, esto podr�a ser responsabilidad de un "PickingManager" que usa la c�mara,
    // en lugar de la c�mara directamente. Pero por ahora, lo mantenemos aqu� siguiendo tu dise�o original.
    virtual Util::Triangle* GetTriangleLookingAt(const std::vector<Util::Triangle>& triangles) const = 0;
};
