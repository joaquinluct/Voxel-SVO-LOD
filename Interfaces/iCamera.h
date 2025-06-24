#pragma once

#include <DirectXMath.h>
#include <vector> // Para el ray tracing
#include <memory> // Para std::shared_ptr
#include "RayTracing/RayTracing.h" // Asumiendo que Triangle es una estructura definida en Util

// Forward declarations para evitar dependencias circulares si KeyboardManager o Util son definidos en otro lado
//class KeyboardManager; // Asumiendo que KeyboardManager es una clase y no un namespace
//namespace Util {
//    struct Triangle;
//    class RayTracing; // Asumiendo que RayTracing es una clase dentro del namespace Util
//}

class ICamera {
public:
    virtual ~ICamera() = default;

    // Métodos para obtener/establecer la posición y rotación de la cámara
    virtual DirectX::XMFLOAT3 GetPosition() const = 0;
    virtual DirectX::XMFLOAT3 GetRotation() const = 0; // Pitch, Yaw, Roll en radianes

    virtual void SetPosition(float x, float y, float z) = 0;
    virtual void SetRotation(float pitch, float yaw, float roll) = 0; // pitch, yaw, roll en radianes

    // Métodos para establecer la cámara mirando a un punto específico
    virtual void SetLookAt(float x, float y, float z) = 0;
    virtual void SetLookAt(const DirectX::XMFLOAT3& target) = 0;

    // Métodos para obtener las matrices de vista y proyección
    virtual DirectX::XMMATRIX GetViewMatrix() const = 0;
    virtual DirectX::XMMATRIX GetProjectionMatrix() const = 0;

    // Métodos para configurar los parámetros de proyección de la cámara
    virtual void SetProjectionParams(float fieldOfViewRadians, float aspectRatio, float nearPlane, float farPlane) = 0;

    // Métodos para obtener los parámetros de proyección (útil para debug o UIs)
    virtual float GetFieldOfView() const = 0;    // En radianes
    virtual float GetAspectRatio() const = 0;
    virtual float GetNearPlane() const = 0;
    virtual float GetFarPlane() const = 0;

    // Método de actualización que maneja la lógica de movimiento/input
    virtual void Update(float deltaTime) = 0;

    // Métodos de utilidad que dependen del estado de la cámara
    virtual void ExtractFrustumPlanes(DirectX::XMFLOAT4 planes[6]) const = 0;

    // Ray tracing / picking. NOTA: Este método acopla la cámara a Util::Triangle y Util::RayTracing.
    // En un sistema muy grande, esto podría ser responsabilidad de un "PickingManager" que usa la cámara,
    // en lugar de la cámara directamente. Pero por ahora, lo mantenemos aquí siguiendo tu diseño original.
    virtual Util::Triangle* GetTriangleLookingAt(const std::vector<Util::Triangle>& triangles) const = 0;
};