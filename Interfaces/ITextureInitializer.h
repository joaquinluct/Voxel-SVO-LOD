// ITextureInitializer.h
#pragma once

#include <d3d11.h> // Necesario para ID3D11Device y HRESULT

// Opcional: Adelantar la declaración de Material si la interfaz va a interactuar con él de alguna manera,
// aunque en este caso, la inicialización parece ser para los miembros de Material.
// class Material; 

class ITextureInitializer {
public:
    // Virtual puro para hacer la clase abstracta.
    // Recibe el ID3D11Device necesario para la inicialización
    // y punteros a los miembros de Material que necesita configurar.
    // Opcional: podrías pasar un puntero a Material* si la lógica es más compleja,
    // pero pasar los miembros directamente es más explícito y menos acoplado.
    virtual HRESULT Init(
        std::shared_ptr<ID3D11Device> device,
        ID3D11SamplerState** outSamplerState, // Puntero al SamplerState de Material
        ID3D11Buffer** outMatrixBuffer       // Puntero al MatrixBuffer de Material
    ) = 0;

    // El destructor virtual es crucial para liberar correctamente la memoria
    // cuando se eliminen objetos derivados a través de un puntero a la base.
    virtual ~ITextureInitializer() = default;
};