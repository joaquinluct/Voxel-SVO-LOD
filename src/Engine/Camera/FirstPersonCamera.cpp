#include "FirstPersonCamera.h"
#include <debugapi.h>
#include <Defines/CameraDefinition.h>
#include <Game/Systems/World.h>
#include <limits>           // Para std::numeric_limits
#include <ManagerLocator/ManagerLocator.h>
#include <REGISTER_SERVICE_MACRO.h>
#include <ServiceLocator/ServiceLocator.h>
#include <Services/Mouse.h>
#include <Text/Text.h>
#include <windows.h>        // Para OutputDebugStringA (solo para mensajes de depuración)

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
#define CAMERA_SPEED 40.0f
#endif
#ifndef CAMERA_SPEEDY
#define CAMERA_SPEEDY 100.0f
#endif

using namespace DirectX;

FirstPersonCamera::FirstPersonCamera() :
    m_position(4000.0f, 2.0f, 2.0f),
    m_lastPosition(4000.0f, 2.0f, 2.0f),
    m_rotation(0.0f, 0.0f, 0.0f),
    m_fieldOfView(XM_PIDIV4),
    m_aspectRatio(1.0f),
    m_nearPlane(0.1f),
    m_farPlane(10000.0f),
    m_moveSpeed(CAMERA_SPEED),
    //m_rotationSpeed(XMConvertToRadians(0.01f)), // Sensibilidad ajustada para deltas de movimiento de ratón
    m_rotationSpeed(XMConvertToRadians(0.02f)), // Sensibilidad ajustada para deltas de movimiento de ratón
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
    SetPosition(400.0f, 400.0f, 50.0f);
    SetLookAt(0.0f, 0.0f, 0.0f); // Mirar al origen por defecto

    return S_OK;
}

HRESULT FirstPersonCamera::PostInit() {
    m_world = ServiceLocator::GetService<World>();
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

DirectX::XMFLOAT3 FirstPersonCamera::GetLookAt() const {
    // 1. Obtener la posición de la cámara
    DirectX::XMVECTOR eyePos = DirectX::XMLoadFloat3(&m_position);

    // 2. Obtener el vector de dirección "hacia adelante"
    // Este método ya maneja la rotación.
    DirectX::XMVECTOR forwardVector = GetForwardVector();

    // 3. Escalar el vector de dirección a una distancia razonable
    DirectX::XMVECTOR lookAtVector = DirectX::XMVectorScale(forwardVector, m_farPlane);

    // 4. Sumar el vector de dirección a la posición de la cámara
    DirectX::XMVECTOR lookAtPoint = DirectX::XMVectorAdd(eyePos, lookAtVector);

    // 5. Almacenar el resultado y devolverlo
    DirectX::XMFLOAT3 result;
    DirectX::XMStoreFloat3(&result, lookAtPoint);

    return result;
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

XMMATRIX FirstPersonCamera::GetViewMatrix(bool onUpdate) const {
    if (m_viewDirty) { //} && !onUpdate) {
        RecalculateViewMatrix();
    }
    return m_viewMatrixCache;
}

void FirstPersonCamera::RecalculateProjectionMatrix() const {
    m_projectionMatrixCache = XMMatrixPerspectiveFovLH(m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane);
    m_projectionDirty = false;
}

XMMATRIX FirstPersonCamera::GetProjectionMatrix(bool onUpdate) const {
    if (m_projectionDirty) { //&& !onUpdate) {
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

// --------------------------------------------------------------
// MOVE
// --------------------------------------------------------------
void FirstPersonCamera::Move(float x, float y, float z) {
    m_position.x += x;
    m_position.y += y;
    m_position.z += z;
    m_viewDirty = true;
}

// --------------------------------------------------------------
// ROTATE
// --------------------------------------------------------------
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

// --------------------------------------------------------------
// GET FORWARD, RIGHT, UP VECTORS
// --------------------------------------------------------------
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


// --------------------------------------------------------------
// UPDATE HEIGHT
// --------------------------------------------------------------
const float ALTURA_PERSONAJE = 10.0f;
bool FirstPersonCamera::UpdateHeight(float deltaTime, XMVECTOR moveDir) {

    m_verticalVelocity += GRAVITY * deltaTime;

    // 4. Actualizar la posición Y de la cámara
    float newY = m_position.y + m_verticalVelocity * deltaTime;
    //m_position.y += m_verticalVelocity * deltaTime;

    if (m_world && m_world->HasHeight()) {

        // Obtener la altura del terreno en la nueva posición (x, z) de la cámara
        float terrainHeight = m_world->GetTerrain()->GetTerrainHeight(m_position.x, m_position.z);

        /*if (newY > terrainHeight) {
            m_lastHeight = terrainHeight;
            m_position.y = newY;
            return true;
        }*/

        float diff = terrainHeight - m_lastHeight;

        m_lastHeight = terrainHeight;

        /*if (diff > .2f) {
            m_position = m_lastPosition;
            return false;
        }*/

        // DEBUG
        if (diff != 0.0f) {
            m_heightDifference = diff;
        }

        // Verificar la colisión y ajustar la posición
        if (m_position.y < (terrainHeight + ALTURA_PERSONAJE)) {
            // Corregir la posición de la cámara para que esté en la altura del terreno
            m_position.y = terrainHeight + ALTURA_PERSONAJE;

            // Reiniciar la velocidad vertical para detener la caída
            m_verticalVelocity = 0.0f;
        }
    }
    return true;
}

// --------------------------------------------------------------
// UPDATE
// --------------------------------------------------------------
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
    if (m_keyboardManager->IsKeyDown(KeyMoves::Sprint))
        m_moveSpeed = CAMERA_SPEEDY; // Aumentar velocidad al sprintar
    else
        m_moveSpeed = CAMERA_SPEED; // Velocidad normal

    // Normalizar y aplicar velocidad
    if (!XMVector3Equal(moveDir, XMVectorZero())) {
        float vel = m_moveSpeed * deltaTime;
        moveDir = XMVector3Normalize(moveDir);
        moveDir = XMVectorScale(moveDir, vel);

        OutputDebugStringA(("Vel. " + ParseFloat(vel) + " Pos. Inicial de la cámara" + DirectXUtils::ToString(m_position) + " en FirstPersonCamera\n").c_str());

        // Actualizar posición
        XMVECTOR position = XMLoadFloat3(&m_position);
        position = XMVectorAdd(position, moveDir);
        XMStoreFloat3(&m_position, position);
        //OutputDebugStringA(("Pos. cámara" + DirectXUtils::ToString(m_position) + " en FirstPersonCamera\n").c_str());
        m_viewDirty = true;
    }

    // 5. Actualizar la altura
    //if (!UpdateHeight(deltaTime, moveDir)) {
    //    m_viewDirty = true;
    //    //	//return;
    //}

    m_lastPosition = m_position; // Guardar la última posición para comparaciones futuras
}

void FirstPersonCamera::UpdateViewMatrix() {
    if (m_viewDirty) {
        RecalculateViewMatrix();
    }
}
//const float HORIZONTAL_OVERSCAN_MARGIN = 0.01f;
const float HORIZONTAL_OVERSCAN_MARGIN = 0.0f;

void FirstPersonCamera::ExtractFrustumPlanes(std::vector<CameraDefinition::FrustumPlane>& frustumPlanes) const {
    // 1. Obtener las matrices de vista y proyección
    XMMATRIX viewMatrix = m_viewMatrixCache;
    XMMATRIX projectionMatrix = m_projectionMatrixCache;

    // 2. Combinar las matrices para obtener la matriz de vista-proyección
    XMMATRIX viewProjectionMatrix = XMMatrixTranspose(XMMatrixMultiply(viewMatrix, projectionMatrix));

    // 3. Obtener las filas de la matriz combinada
    XMVECTOR row0 = viewProjectionMatrix.r[0];
    XMVECTOR row1 = viewProjectionMatrix.r[1];
    XMVECTOR row2 = viewProjectionMatrix.r[2];
    XMVECTOR row3 = viewProjectionMatrix.r[3];

    frustumPlanes.clear();
    frustumPlanes.resize(6);

    // Plano Izquierdo (row3 + row0)
    //XMVECTOR leftPlane = row3 + row0;
    XMVECTOR leftPlane = row3 - row0;
    leftPlane = XMVector4Normalize(leftPlane);
    frustumPlanes[0].coefficients.x = XMVectorGetX(leftPlane);
    frustumPlanes[0].coefficients.y = XMVectorGetY(leftPlane);
    frustumPlanes[0].coefficients.z = XMVectorGetZ(leftPlane);
    frustumPlanes[0].coefficients.w = XMVectorGetW(leftPlane) + HORIZONTAL_OVERSCAN_MARGIN;

    // Plano Derecho (row3 - row0)
    //XMVECTOR rightPlane = row3 - row0;
    XMVECTOR rightPlane = row3 + row0;
    rightPlane = XMVector4Normalize(rightPlane);
    frustumPlanes[1].coefficients.x = XMVectorGetX(rightPlane);
    frustumPlanes[1].coefficients.y = XMVectorGetY(rightPlane);
    frustumPlanes[1].coefficients.z = XMVectorGetZ(rightPlane);
    frustumPlanes[1].coefficients.w = XMVectorGetW(rightPlane) + HORIZONTAL_OVERSCAN_MARGIN;

    // Plano Inferior (row3 + row1)
    //XMVECTOR bottomPlane = row3 + row1;
    XMVECTOR bottomPlane = row3 - row1;
    bottomPlane = XMVector4Normalize(bottomPlane);
    frustumPlanes[2].coefficients.x = XMVectorGetX(bottomPlane);
    frustumPlanes[2].coefficients.y = XMVectorGetY(bottomPlane);
    frustumPlanes[2].coefficients.z = XMVectorGetZ(bottomPlane);
    frustumPlanes[2].coefficients.w = XMVectorGetW(bottomPlane);

    // Plano Superior (row3 - row1)
    //XMVECTOR topPlane = row3 - row1;
    XMVECTOR topPlane = row3 + row1;
    topPlane = XMVector4Normalize(topPlane);
    frustumPlanes[3].coefficients.x = XMVectorGetX(topPlane);
    frustumPlanes[3].coefficients.y = XMVectorGetY(topPlane);
    frustumPlanes[3].coefficients.z = XMVectorGetZ(topPlane);
    frustumPlanes[3].coefficients.w = XMVectorGetW(topPlane);

    // Plano Cercano (row2)
    XMVECTOR nearPlane = row2;
    nearPlane = XMVector4Normalize(nearPlane);
    frustumPlanes[4].coefficients.x = XMVectorGetX(nearPlane);
    frustumPlanes[4].coefficients.y = XMVectorGetY(nearPlane);
    frustumPlanes[4].coefficients.z = XMVectorGetZ(nearPlane);
    frustumPlanes[4].coefficients.w = XMVectorGetW(nearPlane);

    // Plano Lejano (row3 - row2)
    XMVECTOR farPlane = row3 - row2;
    farPlane = XMVector4Normalize(farPlane);
    frustumPlanes[5].coefficients.x = XMVectorGetX(farPlane);
    frustumPlanes[5].coefficients.y = XMVectorGetY(farPlane);
    frustumPlanes[5].coefficients.z = XMVectorGetZ(farPlane);
    frustumPlanes[5].coefficients.w = XMVectorGetW(farPlane);
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

std::string FirstPersonCamera::GetDebugInfo() const {
    return ParseFloat(m_heightDifference);
}
