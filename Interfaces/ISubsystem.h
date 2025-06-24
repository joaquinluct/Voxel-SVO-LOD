// ISubsystem.h
#pragma once
#include <d3d11.h> // Para HRESULT

class ISubsystem {
public:
    virtual ~ISubsystem() = default;
    virtual HRESULT Init() = 0; // Se encargará de obtener sus dependencias del SL
    virtual void Update(float deltaTime) = 0;
    virtual void Render() = 0;
    virtual void Release() = 0;
    // Puedes añadir un método para el orden de actualización/renderizado si lo necesitas
    // virtual int GetUpdateOrder() const { return 0; }
};