// VoxelMesh.h
#pragma once
#define NOMINMAX 
#include <d3d11.h>
#include <vector>
#include <directxmath.h>

#include "Material/Material.h"        // Asegúrate de que esta ruta sea correcta para tu Material.h
#include "../MarchingCubes/MarchingCubes.h" // Incluye la definición de MarchingCubesVertex y MarchingCubesMesh
#include "Utils.h" // Incluye MarchingCubesVertex (si no lo tienes ya en MarchingCubes.h)

using namespace DirectX;

class VoxelMesh {
public:
    VoxelMesh(DeviceManager* deviceManager, Material* material);
    ~VoxelMesh();

    // Inicializa los buffers de la GPU con la malla generada
    HRESULT Init(ID3D11Device* device, const MarchingCubesMesh& mcMesh);

    void Release();

    // Renderiza la malla usando el contexto del dispositivo y las matrices World, View, Projection
    void Render(ID3D11DeviceContext* context, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix);

    Material* GetMaterial() { return m_material; };
    bool isValidMesh();

    std::vector<Util::Triangle> GetTriangles(std::vector<unsigned int> indices);

	/*void SetChunkKey(const ChunkKey& chunkKey) { m_chunkKey = chunkKey; }
    ChunkKey GetChunkKey() const { return m_chunkKey; }*/
    
private:
    ID3D11Buffer* m_vertexBuffer;
    ID3D11Buffer* m_indexBuffer; // ¡Nuevo! Puntero al buffer de índices en la GPU
    int m_vertexCount;
    int m_indexCount;   // ¡Nuevo! Número de índices en el buffer
	//ChunkKey m_chunkKey; // Clave del chunk asociado a esta malla

    Material* m_material;
	DeviceManager* m_deviceManager;
};