#include "Camera.h"

Camera::Camera() : position(2.0f, 2.0f, 2.0f), rotation(0.0f, 0.0f, 0.0f),
m_fieldOfView(XM_PIDIV4), m_aspectRatio(1.0f), m_nearPlane(0.1f), m_farPlane(1000.0f) {}

void Camera::SetPosition(float x, float y, float z) {
    position = XMFLOAT3(x, y, z);
}

void Camera::SetRotation(float pitch, float yaw, float roll) {
    rotation = XMFLOAT3(pitch, yaw, roll);
}

XMMATRIX Camera::GetRotationMatrix() const {
    // Calcular la direcci�n de la c�mara usando la rotaci�n
    float pitch = GetPitch(); // Ya est� en radianes
    float yaw = GetYaw();;   // Ya est� en radianes
    float roll = GetRoll();  // Ya est� en radianes

    // Crear una matriz de rotaci�n a partir de los �ngulos
    XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	return rotationMatrix;
}



XMVECTOR Camera::GetLookAt(XMVECTOR eyePos, XMMATRIX rotationMatrix) const{
    // Vector base para la direcci�n hacia adelante (eje Z positivo)
    XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    // Aplicar la rotaci�n al vector "forward"
    XMVECTOR lookAt = XMVector3TransformCoord(forward, rotationMatrix);

    // Calcular el punto al que mira la c�mara
    lookAt = XMVectorAdd(eyePos, lookAt);

    return lookAt;
}

XMMATRIX Camera::GetViewMatrix() const {
    // Cargar posici�n de la c�mara
    XMVECTOR eyePos = XMLoadFloat3(&position);

    // Crear una matriz de rotaci�n a partir de los �ngulos
    XMMATRIX rotationMatrix = GetRotationMatrix();

    // Calcular el punto al que mira la c�mara
    XMVECTOR lookAt = GetLookAt(eyePos, rotationMatrix);

    // Vector "arriba" (eje Y positivo)
    XMVECTOR upDir = XMVector3TransformCoord(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotationMatrix);

    // Crear y devolver la matriz de vista
    return XMMatrixLookAtLH(eyePos, lookAt, upDir);
}

XMFLOAT3 Camera::GetLookAtPosition() {    
	XMVECTOR lookAt = GetLookAt(XMLoadFloat3(&position), GetRotationMatrix());
    XMFLOAT3 lookAtPos;
    XMStoreFloat3(&lookAtPos, lookAt);
    return lookAtPos;
}

XMMATRIX Camera::GetProjectionMatrix() const {
    return XMMatrixPerspectiveFovLH(m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane);
}

void Camera::SetProjectionParams(float fieldOfView, float aspectRatio, float nearPlane, float farPlane) {
    m_fieldOfView = fieldOfView;
    m_aspectRatio = aspectRatio;
    m_nearPlane = nearPlane;
    m_farPlane = farPlane;
}

// Implementaci�n de los nuevos m�todos
void Camera::Move(float x, float y, float z) {
    position.x += x;
    position.y += y;
    position.z += z;
}

void Camera::Rotate(float pitchOffset, float yawOffset, float rollOffset) {
    rotation.x += pitchOffset;
    rotation.y += yawOffset;
    rotation.z += rollOffset;

    // Clamp pitch to prevent flipping
    if (rotation.x > XM_PIDIV2)
        rotation.x = XM_PIDIV2;
    if (rotation.x < -XM_PIDIV2)
        rotation.x = -XM_PIDIV2;

    // Wrap yaw to keep it in a reasonable range (optional, but prevents potential issues)
    if (rotation.y > XM_PI)
        rotation.y -= XM_2PI;
    if (rotation.y < -XM_PI)
        rotation.y += XM_2PI;
}

void Camera::ExtractFrustumPlanes(XMFLOAT4 planes[6]) const {
    // Obtener la matriz de vista-proyecci�n directamente desde la c�mara
    XMMATRIX viewProjectionMatrix = GetViewMatrix() * GetProjectionMatrix();

    // Transponer la matriz para facilitar la extracci�n de los planos    
    XMFLOAT4X4 m;
    XMStoreFloat4x4(&m, XMMatrixTranspose(viewProjectionMatrix));

    planes[0].x = m._14 + m._11; planes[0].y = m._24 + m._21; planes[0].z = m._34 + m._31; planes[0].w = m._44 + m._41;
    planes[1].x = m._14 - m._11; planes[1].y = m._24 - m._21; planes[1].z = m._34 - m._31; planes[1].w = m._44 - m._41;
    planes[2].x = m._14 + m._12; planes[2].y = m._24 + m._22; planes[2].z = m._34 + m._32; planes[2].w = m._44 + m._42;
    planes[3].x = m._14 - m._12; planes[3].y = m._24 - m._22; planes[3].z = m._34 - m._32; planes[3].w = m._44 - m._42;
    planes[4].x = m._13; planes[4].y = m._23; planes[4].z = m._33; planes[4].w = m._43;
    planes[5].x = m._14 - m._13; planes[5].y = m._24 - m._23; planes[5].z = m._34 - m._33; planes[5].w = m._44 - m._43;

    for (int i = 0; i < 6; ++i) {
        float length = XMVectorGetX(XMVector3Length(XMLoadFloat4(&planes[i])));
        planes[i].x /= length;
        planes[i].y /= length;
        planes[i].z /= length;
        planes[i].w /= length;
    }
}

void Camera::SetLookAt(float x, float y, float z) {
    SetLookAt(XMFLOAT3(x, y, z));
}

void Camera::SetLookAt(const XMFLOAT3& target) {
    // Calcula la dirección desde la posición actual al objetivo
    XMFLOAT3 dir = {
        target.x - position.x,
        target.y - position.y,
        target.z - position.z
    };
    // Normaliza la dirección
    XMVECTOR dirVec = XMVector3Normalize(XMLoadFloat3(&dir));
    // Calcula los ángulos yaw y pitch
    float yaw = atan2f(dir.x, dir.z);
    float length = sqrtf(dir.x * dir.x + dir.z * dir.z);
    float pitch = atan2f(dir.y, length);
    // Roll normalmente es 0 para cámaras FPS
    SetRotation(pitch, yaw, 0.0f);
}