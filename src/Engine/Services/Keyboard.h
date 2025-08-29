#pragma once
#define NOMINMAX
#include <windows.h>
#include <IService.h>
#include <array>
#include <mutex>

class Keyboard : public IService
{
public:
    Keyboard();
    ~Keyboard() override;

    HRESULT Init() override;
    void Shutdown() override;
    void Render() override; // Se mantiene, pero se deja vacía
    void Update(float deltaTime) override; // Se mantiene, pero con una nueva lógica

    const std::string& GetServiceName() const override;
    static const std::string& GetStaticServiceName();

    // Métodos públicos para la lógica del juego
    bool IsKeyDown(unsigned char key) const;
    bool IsKeyUp(unsigned char key) const;
    bool IsKeyPressed(unsigned char key) const; // Pulsación en este frame
    bool IsKeyReleased(unsigned char key) const; // Liberación en este frame

    // NUEVO: Método para que WndProc pueda actualizar el estado de las teclas
    void SetKey(unsigned char key, bool isDown);

private:
    std::array<bool, 256> m_keys;
    std::array<bool, 256> m_previousKeys;
    mutable std::mutex m_mutex;
};