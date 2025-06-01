#pragma once

#include <windows.h>
#include "../Interfaces/iRenderizable.h"
#include "../Camera/Camera.h" // Necesario para mover la cámara
#include <vector>

//-----------------------------------------------------------------------------
// Clase Keyboard
//-----------------------------------------------------------------------------
class Keyboard : public iRenderizable
{
public:
    Keyboard(Camera* camera);
    ~Keyboard();

    HRESULT Init() override;
    void Render() override; // Aquí manejamos el movimiento de la cámara
    void Release() override;

    void Update();
    bool IsKeyDown(unsigned char key) const;
	bool IsCtrlPressed() const;

private:
    Camera* m_camera;
    unsigned char m_keys[256];
    float m_cameraSpeed;
};