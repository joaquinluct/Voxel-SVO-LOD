#pragma once
// ¡IMPORTANTE! Define NOMINMAX antes de cualquier include de Windows o DirectX.
#define NOMINMAX 
#include <windows.h>
#include <IService.h>
#include <vector>

//-----------------------------------------------------------------------------
// Clase Keyboard
//-----------------------------------------------------------------------------
class Keyboard : public IService
{
public:
    Keyboard();
    ~Keyboard() override;

    HRESULT Init() override;
	void Shutdown() override;
    void Render() override;
    void Update(float deltaTime) override; // Aquí manejamos el movimiento de la cámara

    const std::string& GetServiceName() const override {
        static const std::string name = "Keyboard"; // Esta cadena se crea una sola vez y vive durante toda la ejecución del programa.
        return name;
    }
    static const std::string& GetStaticServiceName()
    {
        static const std::string name = "Keyboard"; // Esta cadena se crea una sola vez y vive durante toda la ejecución del programa.
        return name;
    }

    bool IsKeyDown(unsigned char key) const;
    bool IsKeyUp(unsigned char key) const;
    bool IsKeyPressed(unsigned char key) const;
    bool IsCtrlPressed() const;
    bool IsKeyReleased(unsigned char key) const;

private:
    unsigned char m_keys[256];
    unsigned char m_previousKeys[256];
};
