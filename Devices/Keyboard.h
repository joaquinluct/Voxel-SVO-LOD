#pragma once

#include <windows.h>
#include "../Interfaces/iRenderizable.h"
#include <vector>

//-----------------------------------------------------------------------------
// Clase Keyboard
//-----------------------------------------------------------------------------
class Keyboard : public iRenderizable
{
public:
    Keyboard();
    ~Keyboard();

    HRESULT Init() override;
    void Render() override; // Aquí manejamos el movimiento de la cámara
    void Release() override;

    void Update();
    bool IsKeyDown(unsigned char key) const;
    bool IsKeyUp(unsigned char key) const;
    bool IsKeyPressed(unsigned char key) const;
	bool IsCtrlPressed() const;
    bool IsKeyReleased(unsigned char key) const;

private:
    unsigned char m_keys[256];
    unsigned char m_previousKeys[256];
};

