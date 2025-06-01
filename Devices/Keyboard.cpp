#include "Keyboard.h"

Keyboard::Keyboard(Camera* camera) : m_camera(camera), m_cameraSpeed(.7f)
{
    for (int i = 0; i < 256; ++i)
    {
        m_keys[i] = 0;
    }
}

Keyboard::~Keyboard()
{
}

HRESULT Keyboard::Init()
{
    // No hay nada específico que inicializar para el teclado
    return S_OK;
}


void Keyboard::Render()
{
    Update(); // Actualizar el estado del teclado

    if (m_camera)
    {
        // Determinar la velocidad de la cámara
        float cameraSpeed = IsCtrlPressed() ? m_cameraSpeed * 2.5f : m_cameraSpeed;

        // Movimiento hacia adelante (W o Flecha Arriba)
        if (IsKeyDown('W') || IsKeyDown(VK_UP))
        {
            XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
            XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_camera->GetPitch(), m_camera->GetYaw(), m_camera->GetRoll());
            forward = XMVector3TransformCoord(forward, rotationMatrix);
            XMFLOAT3 fwd;
            XMStoreFloat3(&fwd, forward);
            m_camera->Move(fwd.x * cameraSpeed, fwd.y * cameraSpeed, fwd.z * cameraSpeed);
        }

        // Movimiento hacia atrás (S o Flecha Abajo)
        if (IsKeyDown('S') || IsKeyDown(VK_DOWN))
        {
            XMVECTOR forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
            XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_camera->GetPitch(), m_camera->GetYaw(), m_camera->GetRoll());
            forward = XMVector3TransformCoord(forward, rotationMatrix);
            XMFLOAT3 fwd;
            XMStoreFloat3(&fwd, forward);
            m_camera->Move(-fwd.x * cameraSpeed, -fwd.y * cameraSpeed, -fwd.z * cameraSpeed);
        }

        // Movimiento hacia la izquierda (A o Flecha Izquierda)
        if (IsKeyDown('A') || IsKeyDown(VK_LEFT))
        {
            XMVECTOR strafe = XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f);
            XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_camera->GetPitch(), m_camera->GetYaw(), m_camera->GetRoll());
            strafe = XMVector3TransformCoord(strafe, rotationMatrix);
            XMFLOAT3 s;
            XMStoreFloat3(&s, strafe);
            m_camera->Move(s.x * cameraSpeed, s.y * cameraSpeed, s.z * cameraSpeed);
        }

        // Movimiento hacia la derecha (D o Flecha Derecha)
        if (IsKeyDown('D') || IsKeyDown(VK_RIGHT))
        {
            XMVECTOR strafe = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
            XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_camera->GetPitch(), m_camera->GetYaw(), m_camera->GetRoll());
            strafe = XMVector3TransformCoord(strafe, rotationMatrix);
            XMFLOAT3 s;
            XMStoreFloat3(&s, strafe);
            m_camera->Move(s.x * cameraSpeed, s.y * cameraSpeed, s.z * cameraSpeed);
        }
    }
}
void Keyboard::Release()
{
    // No hay nada específico que liberar para el teclado
}

void Keyboard::Update()
{
    BOOL resul = GetKeyboardState(m_keys);
}

bool Keyboard::IsKeyDown(unsigned char key) const
{
    return (m_keys[key] & 0x80) != 0;
}
bool Keyboard::IsCtrlPressed() const
{
	return (GetKeyState(VK_CONTROL) & 0x8000) != 0;
}