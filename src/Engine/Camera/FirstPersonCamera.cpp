#include "FirstPersonCamera.h"
#include <limits>           // Para std::numeric_limits
#include <windows.h>        // Para OutputDebugStringA (solo para mensajes de depuración)
#include <REGISTER_SERVICE_MACRO.h>
#include <ManagerLocator/ManagerLocator.h>

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

// Asegurarse de que CAMERA_SEEP y CAMERA_SEEPDY estén definidos
// Podrían estar en un archivo de constantes compartidas, o definidos aquí si son específicos de FPC.
// Por el momento, los mantengo aquí para que compile, pero considera su ubicación.
#ifndef CAMERA_SEEP
#define CAMERA_SEEP 50.0f
#endif
#ifndef CAMERA_SEEPDY
#define CAMERA_SEEPDY 150.0f
#endif

using namespace DirectX;



FirstPersonCamera::FirstPersonCamera() :
    m_position(2.0f, 2.0f, 2.0f),
    m_rotation(0.0f, 0.0f, 0.0f),
    m_fieldOfView(XM_PIDIV4),
    m_aspectRatio(1.0f),
    m_nearPlane(0.1f),
    m_farPlane(3000.0f),
    m_moveSpeed(CAMERA_SEEP),
    m_rotationSpeed(XMConvertToRadians(0.1f)), // Sensibilidad de rotación por defecto (ajústala)
    m_keyboardManager(),
    m_viewDirty(true),
    m_projectionDirty(true),
	m_projectionMatrixCache(XMMatrixIdentity()),
	m_viewMatrixCache(XMMatrixIdentity())
{
    m_keyboardManager = ManagerLocator::GetManager<KeyboardManager>();
}

HRESULT FirstPersonCamera::Init() {
    return S_OK;
}

void FirstPersonCamera::SetPosition(float x, float y, float z) {
    m_position = XMFLOAT3(x, y, z);
    m_viewDirty = true; // La vista cambia si la posición cambia
}

void FirstPersonCamera::SetRotation(float pitch, float yaw, float roll) {
    m_rotation = XMFLOAT3(pitch, yaw, roll);

    // Clamp pitch to prevent flipping (mirar directamente hacia arriba o abajo)
    if (m_rotation.x > XM_PIDIV2)
        m_rotation.x = XM_PIDIV2;
    if (m_rotation.x < -XM_PIDIV2)
        m_rotation.x = -XM_PIDIV2;

    // Wrap yaw to keep it in a reasonable range (0 a 2*PI o -PI a PI)
    if (m_rotation.y > XM_PI)
        m_rotation.y -= XM_2PI;
    if (m_rotation.y < -XM_PI)
        m_rotation.y += XM_2PI;

    // Roll (z) clamping if needed, typically 0 for FPS
    // if (m_rotation.z > XM_PI) m_rotation.z -= XM_2PI;
    // if (m_rotation.z < -XM_PI) m_rotation.z += XM_2PI;

    m_viewDirty = true; // La vista cambia si la rotación cambia
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
    if (m_rotation.x > XM_PIDIV2)
        m_rotation.x = XM_PIDIV2;
    if (m_rotation.x < -XM_PIDIV2)
        m_rotation.x = -XM_PIDIV2;

    // Wrap yaw to keep it in a reasonable range
    if (m_rotation.y > XM_PI)
        m_rotation.y -= XM_2PI;
    if (m_rotation.y < -XM_PI)
        m_rotation.y += XM_2PI;

    m_viewDirty = true;
}

void FirstPersonCamera::Update(float deltaTime) {
    if (!m_keyboardManager) {
        return; // No hay input si no tenemos KeyboardManager
    }

    // Movimiento de la cámara (Horizontal y Vertical si permitimos volar)
    float currentMoveSpeed = m_moveSpeed * deltaTime; // Aplicar deltaTime

    // Calcular la dirección hacia adelante (ignorando el pitch para movimiento horizontal tipo FPS)
    XMMATRIX yawRotationMatrix = XMMatrixRotationY(m_rotation.y); // Solo yaw para movimiento en el plano XZ
    XMVECTOR forwardDir = XMVector3TransformCoord(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), yawRotationMatrix);
    XMVECTOR strafeDir = XMVector3TransformCoord(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), yawRotationMatrix);

    XMFLOAT3 fwd, s;
    XMStoreFloat3(&fwd, forwardDir);
    XMStoreFloat3(&s, strafeDir);

    float moveX = 0.0f;
    float moveZ = 0.0f;
    float moveY = 0.0f; // Para movimiento vertical libre

    if (m_keyboardManager->IsKeyDown('W') || m_keyboardManager->IsKeyDown(VK_UP)) {
        moveX += fwd.x * currentMoveSpeed;
        moveZ += fwd.z * currentMoveSpeed;
    }
    if (m_keyboardManager->IsKeyDown('S') || m_keyboardManager->IsKeyDown(VK_DOWN)) {
        moveX -= fwd.x * currentMoveSpeed;
        moveZ -= fwd.z * currentMoveSpeed;
    }
    if (m_keyboardManager->IsKeyDown('A') || m_keyboardManager->IsKeyDown(VK_LEFT)) {
        moveX -= s.x * currentMoveSpeed;
        moveZ -= s.z * currentMoveSpeed;
    }
    if (m_keyboardManager->IsKeyDown('D') || m_keyboardManager->IsKeyDown(VK_RIGHT)) {
        moveX += s.x * currentMoveSpeed;
        moveZ += s.z * currentMoveSpeed;
    }
    // Movimiento vertical libre (ejemplo con Q/E o espacio/control)
    if (m_keyboardManager->IsKeyDown(VK_SPACE)) { // Subir
        moveY += currentMoveSpeed;
    }
    if (m_keyboardManager->IsKeyDown(VK_CONTROL)) { // Bajar
        moveY -= currentMoveSpeed;
    }

    Move(moveX, moveY, moveZ);


    if (m_keyboardManager->IsKeyDown(VK_F8)) {
		SetPosition(200.0f, 200.0f, 200.0f); // Resetear posición a un punto fijo
		SetLookAt(0.0f, 0.0f, 0.0f); // Mirar al origen
		m_farPlane = 6000.0f; // Ajustar el plano lejano para una vista más cercana
        RecalculateProjectionMatrix();
    }

    // Modificar velocidad de la cámara (shift/control)
    if (m_keyboardManager->IsKeyDown(VK_SHIFT)) { // Usamos SHIFT para acelerar
        m_moveSpeed = CAMERA_SEEPDY;
    }
    else { // Si no, velocidad normal
        m_moveSpeed = CAMERA_SEEP;
    }
    // Nota: El control para `VK_CONTROL` para la velocidad `CAMERA_SEEPDY` estaba duplicado y ahora se usa para bajar.
    // Esto es solo un ejemplo, ajusta las teclas a tu gusto.


    // Rotación de la cámara usando el ratón (si MouseManager está disponible)
    // Esto asume que KeyboardManager también maneja el input del ratón o que hay un MouseManager separado.
    // Si tu KeyboardManager tiene GetMouseDeltaX/Y, úsalos.
    // Si no, deberás obtener un MouseManager del ServiceLocator o pasar uno a la FPC.

    // Comentado para evitar errores de compilación si no tienes estos métodos en KeyboardManager.
    // Si los tienes, descomenta y ajusta la sensibilidad:
    // float pitchOffset = m_keyboardManager->GetMouseDeltaY() * m_rotationSpeed * deltaTime;
    // float yawOffset = m_keyboardManager->GetMouseDeltaX() * m_rotationSpeed * deltaTime;
    // Rotate(pitchOffset, yawOffset, 0.0f);
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
        // Asumo que tu Util::RayTracing::Trace tiene una sobrecarga para un solo triángulo o un vector de triángulos.
        float distance = rayTracer.Trace(cameraPosition, cameraDirection, 1000.0f, { triangle });
        if (distance > 0.0f && distance < closestDistance) {
            closestDistance = distance;
            closestTriangle = &triangle;
        }
    }

    return const_cast<Util::Triangle*>(closestTriangle);
}