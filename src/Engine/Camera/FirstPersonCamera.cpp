#include "FirstPersonCamera.h"
#include <Services/Mouse.h>
#include <limits>           // Para std::numeric_limits
#include <windows.h>        // Para OutputDebugStringA (solo para mensajes de depuración)
#include <REGISTER_SERVICE_MACRO.h>
#include <ManagerLocator/ManagerLocator.h>
#include <ServiceLocator/ServiceLocator.h>
#include <algorithm>        // Para std::clamp

REGISTER_SERVICE_TYPE(FirstPersonCamera, "FirstPersonCamera")

// Redefinir las constantes de DirectXMath si no están disponibles globalmente
// O asegúrate de que se incluyan donde sea que se definan (e.g., <DirectXMath.h> en tu entorno)
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

// Asegurarse de que CAMERA_SPEED y CAMERA_SPEEDY estén definidos
// Podrían estar en un archivo de constantes compartidas, o definidos aquí si son específicos de FPC.
// Por el momento, los mantengo aquí para que compile, pero considera su ubicación.
#ifndef CAMERA_SPEED
#define CAMERA_SPEED 15.0f
#endif
#ifndef CAMERA_SPEEDY
#define CAMERA_SPEEDY 50.0f
#endif

using namespace DirectX;

FirstPersonCamera::FirstPersonCamera() :
    m_position(2.0f, 2.0f, 2.0f),
    m_rotation(0.0f, 0.0f, 0.0f),
    m_fieldOfView(XM_PIDIV4),
    m_aspectRatio(1.0f),
    m_nearPlane(0.1f),
    m_farPlane(7000.0f),
    m_moveSpeed(CAMERA_SPEED),
    m_rotationSpeed(XMConvertToRadians(0.1f)), // Sensibilidad ajustada para deltas de movimiento de ratón
    m_viewDirty(true),
    m_projectionDirty(true),
    m_projectionMatrixCache(XMMatrixIdentity()),
    m_viewMatrixCache(XMMatrixIdentity())
{
}

HRESULT FirstPersonCamera::Init() {
    // Obtener referencias a los servicios necesarios
    m_keyboardManager = ManagerLocator::GetKeyboardManager();
    m_mouseService = ServiceLocator::GetService<Mouse>();
    
    // Configurar posición inicial
    SetPosition(40.0f, 40.0f, 40.0f);
    SetLookAt(0.0f, 0.0f, 0.0f); // Mirar al origen por defecto
    
    return S_OK;
}

void FirstPersonCamera::SetPosition(float x, float y, float z) {
    m_position = XMFLOAT3(x, y, z);
    m_viewDirty = true; // La vista cambia si la posición cambia
}

void FirstPersonCamera::SetRotation(float pitch, float yaw, float roll) {
    m_rotation = XMFLOAT3(pitch, yaw, roll);
    m_viewDirty = true;
}

void FirstPersonCamera::SetLookAt(float x, float y, float z) {
    SetLookAt(XMFLOAT3(x, y, z));
}

void FirstPersonCamera::SetLookAt(const XMFLOAT3& target) {
    XMFLOAT3 dir = {
        target.x - m_position.x,
        target.y - m_position.y,
        target.z - m_position.z
    };
    XMVECTOR dirVec = XMVector3Normalize(XMLoadFloat3(&dir));

    // Calculate yaw (rotation around Y-axis)
    // atan2f(x, z) gives angle in range -PI to PI
    float yaw = atan2f(XMVectorGetX(dirVec), XMVectorGetZ(dirVec));

    // Calculate pitch (rotation around X-axis)
    // Project the direction vector onto the XZ plane to get its length
    float flatLength = sqrtf(XMVectorGetX(dirVec) * XMVectorGetX(dirVec) + XMVectorGetZ(dirVec) * XMVectorGetZ(dirVec));
    float pitch = atan2f(XMVectorGetY(dirVec), flatLength);

    SetRotation(pitch, yaw, 0.0f); // Roll is usually 0 for a "look at" function
}

float FirstPersonCamera::GetFieldOfView() const {
    return m_fieldOfView;
}

float FirstPersonCamera::GetAspectRatio() const {
    return m_aspectRatio;
}

float FirstPersonCamera::GetNearPlane() const {
    return m_nearPlane;
}

float FirstPersonCamera::GetFarPlane() const {
    return m_farPlane;
}

DirectX::XMFLOAT3 FirstPersonCamera::GetPosition() const {
    return m_position;
}

XMMATRIX FirstPersonCamera::GetInternalRotationMatrix() const {
    // Las propiedades de rotación ya están en radianes
    return XMMatrixRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);
}

XMVECTOR FirstPersonCamera::GetInternalLookAt(XMVECTOR eyePos, XMMATRIX rotationMatrix) const {
    // Vector base para la dirección hacia adelante (eje Z positivo)
    XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    // Aplicar la rotación al vector "forward"
    XMVECTOR lookAt = XMVector3TransformCoord(forward, rotationMatrix);

    // Calcular el punto al que mira la cámara
    lookAt = XMVectorAdd(eyePos, lookAt);

    return lookAt;
}

void FirstPersonCamera::RecalculateViewMatrix() const {
    XMVECTOR eyePos = XMLoadFloat3(&m_position);
    XMMATRIX rotationMatrix = GetInternalRotationMatrix();
    XMVECTOR lookAt = GetInternalLookAt(eyePos, rotationMatrix);

    // Vector "arriba" (eje Y positivo), transformado por la rotación de la cámara
    XMVECTOR upDir = XMVector3TransformCoord(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotationMatrix);

    m_viewMatrixCache = XMMatrixLookAtLH(eyePos, lookAt, upDir);
    m_viewDirty = false;
}

XMMATRIX FirstPersonCamera::GetViewMatrix() const {
    if (m_viewDirty) {
        RecalculateViewMatrix();
    }
    return m_viewMatrixCache;
}

void FirstPersonCamera::RecalculateProjectionMatrix() const {
    m_projectionMatrixCache = XMMatrixPerspectiveFovLH(m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane);
    m_projectionDirty = false;
}

XMMATRIX FirstPersonCamera::GetProjectionMatrix() const {
    if (m_projectionDirty) {
        RecalculateProjectionMatrix();
    }
    return m_projectionMatrixCache;
}

void FirstPersonCamera::SetProjectionParams(float fieldOfViewRadians, float aspectRatio, float nearPlane, float farPlane) {
    m_fieldOfView = fieldOfViewRadians;
    m_aspectRatio = aspectRatio;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
    m_projectionDirty = true; // La matriz de proyección cambia
}

// Implementación de los métodos de movimiento y rotación
void FirstPersonCamera::Move(float x, float y, float z) {
    m_position.x += x;
    m_position.y += y;
    m_position.z += z;
    m_viewDirty = true;
}

void FirstPersonCamera::Rotate(float pitchOffset, float yawOffset, float rollOffset) {
    m_rotation.x += pitchOffset;
    m_rotation.y += yawOffset;
    m_rotation.z += rollOffset;

    // Clamp pitch to prevent flipping
    if (m_rotation.x > XM_PIDIV2 - 0.01f)
        m_rotation.x = XM_PIDIV2 - 0.01f;
    if (m_rotation.x < -XM_PIDIV2 + 0.01f)
        m_rotation.x = -XM_PIDIV2 + 0.01f;

    // Wrap yaw to keep it in a reasonable range
    if (m_rotation.y > XM_PI)
        m_rotation.y -= XM_2PI;
    if (m_rotation.y < -XM_PI)
        m_rotation.y += XM_2PI;

    m_viewDirty = true;
}

// Vectores de dirección para movimiento
DirectX::XMVECTOR FirstPersonCamera::GetForwardVector() const {
    XMMATRIX rotationMatrix = GetInternalRotationMatrix();
    XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    return XMVector3TransformCoord(forward, rotationMatrix);
}

DirectX::XMVECTOR FirstPersonCamera::GetRightVector() const {
    XMMATRIX rotationMatrix = GetInternalRotationMatrix();
    XMVECTOR right = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
    return XMVector3TransformCoord(right, rotationMatrix);
}

DirectX::XMVECTOR FirstPersonCamera::GetUpVector() const {
    XMMATRIX rotationMatrix = GetInternalRotationMatrix();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    return XMVector3TransformCoord(up, rotationMatrix);
}

// Actualización basada en la entrada
void FirstPersonCamera::Update(float deltaTime) {
    // 1. Actualizar rotación basada en el ratón
    float deltaX = static_cast<float>(m_mouseService->GetDeltaX());
    float deltaY = static_cast<float>(m_mouseService->GetDeltaY());

    // Aplicar rotación con los deltas del ratón
    Rotate(
        deltaY * m_rotationSpeed, // Negativo porque el eje Y de pantalla va hacia abajo
        deltaX * m_rotationSpeed,
        0.0f                      // Sin cambio en roll
    );

    m_mouseService->SetCenter();

    // 2. Actualizar posición basada en el teclado
    XMVECTOR moveDir = XMVectorZero();

    // Movimiento adelante/atrás y lateral
    if (m_keyboardManager->IsKeyDown(KeyMoves::Forward))
        moveDir = XMVectorAdd(moveDir, GetForwardVector());
    if (m_keyboardManager->IsKeyDown(KeyMoves::Backward))
        moveDir = XMVectorAdd(moveDir, XMVectorScale(GetForwardVector(), -1.0f));
    if (m_keyboardManager->IsKeyDown(KeyMoves::Left))
        moveDir = XMVectorAdd(moveDir, XMVectorScale(GetRightVector(), -1.0f));
    if (m_keyboardManager->IsKeyDown(KeyMoves::Right))
        moveDir = XMVectorAdd(moveDir, GetRightVector());

    // Normalizar y aplicar velocidad
    if (!XMVector3Equal(moveDir, XMVectorZero())) {
        moveDir = XMVector3Normalize(moveDir);
        moveDir = XMVectorScale(moveDir, m_moveSpeed * deltaTime);
        
        // Actualizar posición
        XMVECTOR position = XMLoadFloat3(&m_position);
        position = XMVectorAdd(position, moveDir);
        XMStoreFloat3(&m_position, position);
        
        m_viewDirty = true;
    }
}

void FirstPersonCamera::UpdateViewMatrix() {
    if (m_viewDirty) {
        RecalculateViewMatrix();
    }
}

void FirstPersonCamera::ExtractFrustumPlanes(XMFLOAT4 planes[6]) const {
    // Obtener la matriz de vista-proyección directamente de la cámara
    XMMATRIX viewProjectionMatrix = GetViewMatrix() * GetProjectionMatrix();

    // Transponer la matriz para facilitar la extracción de los planos
    XMFLOAT4X4 m;
    XMStoreFloat4x4(&m, XMMatrixTranspose(viewProjectionMatrix));

    // Extracción de los planos (ya normalizados en el código original)
    planes[0].x = m._14 + m._11; planes[0].y = m._24 + m._21; planes[0].z = m._34 + m._31; planes[0].w = m._44 + m._41; // Left
    planes[1].x = m._14 - m._11; planes[1].y = m._24 - m._21; planes[1].z = m._34 - m._31; planes[1].w = m._44 - m._41; // Right
    planes[2].x = m._14 + m._12; planes[2].y = m._24 + m._22; planes[2].z = m._34 + m._32; planes[2].w = m._44 + m._42; // Bottom
    planes[3].x = m._14 - m._12; planes[3].y = m._24 - m._22; planes[3].z = m._34 - m._32; planes[3].w = m._44 - m._42; // Top
    planes[4].x = m._13;         planes[4].y = m._23;         planes[4].z = m._33;         planes[4].w = m._43;         // Near
    planes[5].x = m._14 - m._13; planes[5].y = m._24 - m._23; planes[5].z = m._34 - m._33; planes[5].w = m._44 - m._43; // Far

    // Normalizar los planos
    for (int i = 0; i < 6; ++i) {
        float length = XMVectorGetX(XMVector3Length(XMLoadFloat4(&planes[i])));
        if (length != 0.0f) { // Evitar división por cero
            planes[i].x /= length;
            planes[i].y /= length;
            planes[i].z /= length;
            planes[i].w /= length;
        }
    }
}

Util::Triangle* FirstPersonCamera::GetTriangleLookingAt(const std::vector<Util::Triangle>& triangles) const {
    XMFLOAT3 cameraPosition = GetPosition();

    XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    XMMATRIX rotationMatrix = GetInternalRotationMatrix(); // Usa el método interno de la clase
    forward = XMVector3TransformCoord(forward, rotationMatrix);
    XMFLOAT3 cameraDirection;
    XMStoreFloat3(&cameraDirection, XMVector3Normalize(forward));

    Util::RayTracing rayTracer; // Asumiendo que Util::RayTracing es una clase válida
    float closestDistance = std::numeric_limits<float>::max();
    const Util::Triangle* closestTriangle = nullptr;

    for (const auto& triangle : triangles) { // Usar const auto& para eficiencia
        // Trace contra un solo triángulo, asegúrate que Trace puede tomar una lista de un solo elemento
        // o ajusta la firma de RayTracing::Trace.
        float distance = rayTracer.Trace(cameraPosition, cameraDirection, 1000.0f, { triangle });
        if (distance > 0.0f && distance < closestDistance) {
            closestDistance = distance;
            closestTriangle = &triangle;
        }
    }

    return const_cast<Util::Triangle*>(closestTriangle);
}