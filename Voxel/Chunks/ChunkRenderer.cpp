// ChunkRenderer.cpp
#include "ChunkRenderer.h"
#include <iostream>
#include <algorithm> // Para std::remove_if si lo usas en alguna funci�n

ChunkRenderer::ChunkRenderer(DeviceManager* deviceManager, ChunkManager* chunkManager, Material* chunkMaterial)
    : m_chunkManager(chunkManager), m_marchingCubes(chunkManager), m_chunkMaterial(chunkMaterial), m_deviceManager(deviceManager), m_octree(XMFLOAT3(0,0,0),0,0) {
    m_chunkManager = new ChunkManager();
}

ChunkRenderer::~ChunkRenderer() {
    Release();
}

HRESULT ChunkRenderer::Init(ID3D11Device* device) {
    return S_OK;
}

static void InitializeChunk(Chunk& chunk) {
    if (chunk.IsInitialized()) {
		return; // Si ya est� inicializado, no hacer nada
    }

     // Inicializar el suelo si el chunk est� en la capa del suelo
    if (chunk.GetPosition().y == -1) {
    /*if ((chunk.GetPosition().y == 2 && chunk.GetPosition().z == 1 && chunk.GetPosition().x == 2) ||
        (chunk.GetPosition().y == 2 && chunk.GetPosition().z == 2 && chunk.GetPosition().x == 1) ||
        (chunk.GetPosition().y == 2 && chunk.GetPosition().z == 1 && chunk.GetPosition().x == 1) ||
        (chunk.GetPosition().y == 2 && chunk.GetPosition().z == 2 && chunk.GetPosition().x == 2)         
        ) {*/
         /*for (int x = 1; x < Chunk::CHUNK_SIZE_X; ++x) {
            for (int y = 1; y < Chunk::CHUNK_SIZE_Y; ++y) {
                for (int z = 1; z < Chunk::CHUNK_SIZE_Z; ++z) {
                    chunk.SetVoxel(x, y, z, true);
                }
             }
         }*/
         for (int x = 0; x < 16; ++x) {
             for (int y = 0; y < 4; ++y) {
                 for (int z = 0; z < 16; ++z) {
                     chunk.SetVoxel(x, y, z, true);
                 }
             }
         }
     }

    // Inicializar el cubo de depuraci�n si el chunk est� en la posici�n (3, 3, 3)
    if (chunk.GetPosition().y == 3 && chunk.GetPosition().x == 3 && chunk.GetPosition().z == 3) {
        for (int x = 0; x <= 15; ++x) {
            for (int z = 0; z <= 15; ++z) {
                chunk.SetVoxel(x, 10, z, true);
                chunk.SetVoxel(x, 11, z, true);
                chunk.SetVoxel(x, 12, z, true);
                chunk.SetVoxel(x, 13, z, true);
                chunk.SetVoxel(x, 14, z, true);
                chunk.SetVoxel(x, 15, z, true);
            }
        }
    }
    chunk.SetIsInitialized(true);
}

// Obtener los vecinos del chunk actual
std::vector<VoxelMesh*> ChunkRenderer::GetChunkNeighbors(const ChunkKey& chunkKey) {
    std::vector<VoxelMesh*> neighbors;

    // Definir los desplazamientos para los 6 vecinos ortogonales
    const int offsets[6][3] = {
        {1, 0, 0}, {-1, 0, 0}, // Vecinos en X
        {0, 1, 0}, {0, -1, 0}, // Vecinos en Y
        {0, 0, 1}, {0, 0, -1}  // Vecinos en Z
    };

    for (const auto& offset : offsets) {
        ChunkKey neighborKey(chunkKey.x + offset[0], chunkKey.y + offset[1], chunkKey.z + offset[2]);

        // Buscar el vecino en m_chunkMeshes
        auto it = m_chunkMeshes.find(neighborKey);
        if (it != m_chunkMeshes.end()) {
            neighbors.push_back(it->second); // Agregar el vecino si existe
        }
    }

    return neighbors;
}

// Render ahora toma ID3D11DeviceContext*, viewMatrix y projectionMatrix
void ChunkRenderer::Render(Camera* camera, ID3D11DeviceContext* context, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix) {

    if (!m_chunkManager->HasChunks()) {
        return;
    }

    XMMATRIX viewProjectionMatrix = viewMatrix * projectionMatrix; // Combina para el frustum culling
    XMFLOAT4 frustumPlanes[6];
    camera->ExtractFrustumPlanes(frustumPlanes);

    std::unordered_map<ChunkKey, Chunk*> currentChunks = m_chunkManager->GetChunks();
    m_octree.QueryFrustum(frustumPlanes, currentChunks);

    std::unordered_map<ChunkKey, Chunk*> frustrumChunks{};

    for (const auto& chunkPair : currentChunks) {
        ChunkKey chunkKey = chunkPair.first;
        Chunk* chunk = chunkPair.second;

        XMFLOAT3 chunkPosition = chunk->GetPosition();

        // Frustum Culling
        if (IsChunkInFrustum(chunkPosition, static_cast<float>(Chunk::CHUNK_SIZE_X), frustumPlanes)) {
            frustrumChunks[chunkKey] = chunk; // Agregar al mapa de chunks visibles    
            InitializeChunk(*chunk);
            if (!chunk->IsEmpty() && chunk->IsBorderChunk()) {
                chunk->MarkMeshForUpdate();
            }
            //InitializeChunk(*chunk);
            //chunk->MarkMeshForUpdate();
            /*auto it = m_chunkMeshes.find(chunkKey);            
            if (it == m_chunkMeshes.end()) {
                InitializeChunk(*chunk);
			}*/

			/*std::vector<VoxelMesh*> neighbors = GetChunkNeighbors(chunkKey);
            for (const auto& neighbor : neighbors) {
				ChunkKey neighborKey = neighbor->GetChunkKey();
				auto chunkIt = frustrumChunks.find(neighborKey);
                if (chunkIt != frustrumChunks.end()) {
                    Chunk* chunk = chunkPair.second;
                    chunk->MarkMeshForUpdate();
				}
			}*/

        }

        // Inicializar el chunk si no est� inicializado
            //chunk->SetIsInitialized(true); // Marca como inicializado para evitar re-inicializaci�n
		
    }

    m_chunkMeshesOnFrustrum = frustrumChunks;

    for (const auto& chunkPair : frustrumChunks) {
        ChunkKey chunkKey = chunkPair.first;
        Chunk* chunk = chunkPair.second;

        /*XMFLOAT3 chunkPosition = chunk->GetPosition();*/

        //// Frustum Culling
        //if (!IsChunkInFrustum(chunkPosition, static_cast<float>(Chunk::CHUNK_SIZE_X), frustumPlanes)) {
        //    continue;
        //}

		bool addMesh = false;

        VoxelMesh* mesh = nullptr;
        auto it = m_chunkMeshes.find(chunkKey);
        if (it != m_chunkMeshes.end()) {
            mesh = it->second;
        }
        else {
            mesh = new VoxelMesh(m_deviceManager, m_chunkMaterial);
			mesh->SetChunkKey(chunkKey);
            
			addMesh = true;
        }

        /*if (chunk->IsEmpty() && chunk->isVirtualChunk()) {
            chunk->MarkMeshUpdated();
        }*/


        if (!chunk->IsEmpty() && chunk->NeedsMeshUpdate()) {
            MarchingCubesMesh mcMesh = m_marchingCubes.GenerateMesh(*chunk);

            ID3D11Device* device = nullptr;
            if (m_chunkManager) {
                device = m_chunkManager->GetDevice();
            }

            if (!device) {
                std::cerr << "Error: ID3D11Device is null in ChunkRenderer::Render for chunk: "
                    << chunkKey.x << ", " << chunkKey.y << ", " << chunkKey.z << std::endl;
                continue;
            }

            HRESULT hr = mesh->Init(device, mcMesh); // Pasa la MarchingCubesMesh completa
            if (FAILED(hr)) {
                /*std::cerr << "Error updating mesh for chunk " << chunkKey.x << ", " << chunkKey.y << ", " << chunkKey.z << std::endl;*/
                continue;
            }
            chunk->MarkMeshUpdated();
        }

        // Renderizar el VoxelMesh pasando las matrices separadas
        if (mesh && mesh->isValidMesh()) {
			// Si el mesh es válido, añadirlo al mapa de chunkMeshes
            if (addMesh) {
                m_chunkMeshes[chunkKey] = mesh;
            }
            // Calcular la matriz de mundo para este chunk
            XMFLOAT3 chunkPosition = chunk->GetPosition();
            XMMATRIX chunkWorldMatrix = XMMatrixTranslation(chunkPosition.x * Chunk::CHUNK_SIZE_X, chunkPosition.y * Chunk::CHUNK_SIZE_Y, chunkPosition.z * Chunk::CHUNK_SIZE_Z);

			// Renderizar el mesh del chunk
            mesh->Render(context, chunkWorldMatrix, viewMatrix, projectionMatrix);
        }
    }
}

void ChunkRenderer::Release() {
    for (auto& meshPair : m_chunkMeshes) {
        if (meshPair.second) {
            meshPair.second->Release();
            delete meshPair.second;
        }
    }
    m_chunkMeshes.clear();
}

void ChunkRenderer::ReleaseChunkMesh(const ChunkKey& key) {
    auto it = m_chunkMeshes.find(key);
    if (it != m_chunkMeshes.end()) {
        if (it->second) {
            it->second->Release();
            delete it->second;
        }
        m_chunkMeshes.erase(it);
    }
}

bool ChunkRenderer::IsChunkInFrustum(const XMFLOAT3& chunkPosition, float chunkSize, const XMFLOAT4 planes[6]) {
    float radius = sqrt(3) * (chunkSize / 2.0f); // Radio de la esfera delimitadora
    XMFLOAT3 center = {
        chunkPosition.x + chunkSize / 2.0f,
        chunkPosition.y + chunkSize / 2.0f,
        chunkPosition.z + chunkSize / 2.0f
    };

    for (int i = 0; i < 6; ++i) {
        float distance = planes[i].x * center.x + planes[i].y * center.y + planes[i].z * center.z + planes[i].w;
        if (distance < -radius) {
            return false; // Completamente fuera del frustum
        }
    }
    return true; // Parcialmente o completamente dentro del frustum
}

//bool ChunkRenderer::IsChunkInFrustum(const XMFLOAT3& chunkPosition, float chunkSize, const XMFLOAT4 planes[6]) {
//    XMFLOAT3 aabb[8] = {
//        {chunkPosition.x - chunkSize / 2, chunkPosition.y - chunkSize / 2, chunkPosition.z - chunkSize / 2},
//        {chunkPosition.x + chunkSize / 2, chunkPosition.y - chunkSize / 2, chunkPosition.z - chunkSize / 2},
//        {chunkPosition.x - chunkSize / 2, chunkPosition.y + chunkSize / 2, chunkPosition.z - chunkSize / 2},
//        {chunkPosition.x + chunkSize / 2, chunkPosition.y + chunkSize / 2, chunkPosition.z - chunkSize / 2},
//        {chunkPosition.x - chunkSize / 2, chunkPosition.y - chunkSize / 2, chunkPosition.z + chunkSize / 2},
//        {chunkPosition.x + chunkSize / 2, chunkPosition.y - chunkSize / 2, chunkPosition.z + chunkSize / 2},
//        {chunkPosition.x - chunkSize / 2, chunkPosition.y + chunkSize / 2, chunkPosition.z + chunkSize / 2},
//        {chunkPosition.x + chunkSize / 2, chunkPosition.y + chunkSize / 2, chunkPosition.z + chunkSize / 2}
//    };
//
//    for (int i = 0; i < 6; ++i) {
//        int out = 0;
//        for (int j = 0; j < 8; ++j) {
//            float dist = planes[i].x * aabb[j].x + planes[i].y * aabb[j].y + planes[i].z * aabb[j].z + planes[i].w;
//            if (dist < 0) {
//                out++;
//            }
//        }
//        if (out == 8) {
//            return false;
//        }
//    }
//    return true;
//}