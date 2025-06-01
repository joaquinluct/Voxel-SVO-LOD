#include <iostream>
#include "World.h"

#undef min;

World::World(DeviceManager* deviceManager, Camera* camera)
	: m_chunkRenderer( new ChunkRenderer(deviceManager, nullptr, nullptr)), m_camera(camera), m_cameraLastPosition(0, 0, 0),
	m_octreeRenderer(new OctreeRenderer(deviceManager, nullptr)),
    m_octree(new Octree(XMFLOAT3(0, 0, 0), 1024.0f, 5)) { // Inicializa el Octree con un tama�o y profundidad
}

World::~World() {
    delete m_chunkRenderer;
    delete m_octree;
}

HRESULT World::Init(ID3D11Device* device, Material* chunkMaterial) {
    m_chunkRenderer->SetChunkMaterial(chunkMaterial);
    HRESULT hr = m_chunkRenderer->Init(device);
    if (FAILED(hr)) return hr;

    LoadVisibleChunks(XMFLOAT3(0, 0, 0));
    return S_OK;
}

void World::Update(float deltaTime, Camera* camera) {
    std::lock_guard<std::mutex> lock(worldMutex);

    XMFLOAT3 cameraChunkPos = GetCameraChunkPosition(camera);

    if (!isOnSameChunk(cameraChunkPos, m_cameraLastPosition)) {
        LoadVisibleChunks(cameraChunkPos);
        UnloadFarChunks(cameraChunkPos);
        m_cameraLastPosition = cameraChunkPos;
    }
}

void World::Render(ID3D11DeviceContext* context, Camera* camera) {
    std::lock_guard<std::mutex> lock(worldMutex);

    // Extraer los planos del frustum
    /*XMFLOAT4 frustumPlanes[6];
    m_camera->ExtractFrustumPlanes(frustumPlanes);*/

    // Usar el Octree para obtener los chunks visibles
    /*std::unordered_map<ChunkKey, Chunk*> visibleChunks;
    m_octree->QueryFrustum(frustumPlanes, visibleChunks);*/

    //m_octreeRenderer->render();

    //m_chunkRenderer->Render(camera, context, viewMatrix, projectionMatrix);
}

DirectX::XMFLOAT3 World::GetCameraChunkPosition(Camera* camera) const {
    DirectX::XMFLOAT3 cameraPos = camera->GetPosition();
    return DirectX::XMFLOAT3(
        floor(cameraPos.x / Chunk::CHUNK_SIZE_X),
        floor(cameraPos.y / Chunk::CHUNK_SIZE_Y),
        floor(cameraPos.z / Chunk::CHUNK_SIZE_Z)
    );
}

int World::GetNumVisibleChunks()
{
    XMFLOAT4 planes[6]{};
    m_camera->ExtractFrustumPlanes(planes);
    return m_octree->CountVisibleChunks(planes);
}

//void World::LoadVisibleChunks(XMFLOAT3 cameraChunkPos) {
//    int minX = static_cast<int>(floor(cameraChunkPos.x - m_chunkLoadRadius));
//    int maxX = static_cast<int>(ceil(cameraChunkPos.x + m_chunkLoadRadius));
//    int minY = static_cast<int>(floor(cameraChunkPos.y - m_chunkLoadRadius));
//    int maxY = static_cast<int>(ceil(cameraChunkPos.y + m_chunkLoadRadius));
//    int minZ = static_cast<int>(floor(cameraChunkPos.z - m_chunkLoadRadius));
//    int maxZ = static_cast<int>(ceil(cameraChunkPos.z + m_chunkLoadRadius));
//
//    for (int x = minX; x <= maxX; ++x) {
//        for (int y = minY; y <= maxY; ++y) {
//            for (int z = minZ; z <= maxZ; ++z) {
//                float distance = sqrt(
//                    pow((x - cameraChunkPos.x), 2) +
//                    pow((y - cameraChunkPos.y), 2) +
//                    pow((z - cameraChunkPos.z), 2)
//
//                );
//
//                if (distance <= m_chunkLoadRadius) {
//                    ChunkKey key(x, y, z);
//                    if (!m_octree->IsChunkInNode(key)) {
//                        Chunk* newChunk = new Chunk(x, y, z);
//                        m_octree->Insert(newChunk);
//                    }
//                }
//            }
//        }
//    }
//}

//void World::LoadVisibleChunks(XMFLOAT3 cameraChunkPos) {
//    int minX = static_cast<int>(floor(cameraChunkPos.x - m_chunkLoadRadius));
//    int maxX = static_cast<int>(ceil(cameraChunkPos.x + m_chunkLoadRadius));
//    int minY = static_cast<int>(floor(cameraChunkPos.y - m_chunkLoadRadius));
//    int maxY = static_cast<int>(ceil(cameraChunkPos.y + m_chunkLoadRadius));
//    int minZ = static_cast<int>(floor(cameraChunkPos.z - m_chunkLoadRadius));
//    int maxZ = static_cast<int>(ceil(cameraChunkPos.z + m_chunkLoadRadius));
//
//    for (int x = minX; x <= maxX; ++x) {
//        for (int y = minY; y <= maxY; ++y) {
//            for (int z = minZ; z <= maxZ; ++z) {
//                ChunkKey key(x, y, z);
//                if (m_octree->IsChunkInNode(key) && !m_octree->isChunkLoaded(key)) {
//                    Chunk* newChunk = new Chunk(x, y, z);
//                    m_octree->Insert(newChunk);
//
//                    // Cargar vecinos inmediatos
//                    for (int dx = -1; dx <= 1; ++dx) {
//                        for (int dy = -1; dy <= 1; ++dy) {
//                            for (int dz = -1; dz <= 1; ++dz) {
//                                if (dx == 0 && dy == 0 && dz == 0) continue;
//                                ChunkKey neighborKey(x + dx, y + dy, z + dz);
//                                if (!m_octree->IsChunkInNode(neighborKey)) {
//                                    Chunk* neighborChunk = new Chunk(x + dx, y + dy, z + dz);
//                                    m_octree->Insert(neighborChunk);
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//}

bool World::IsChunkInFrustum(const ChunkKey& key, const XMFLOAT4 planes[6]) const {
    XMFLOAT3 chunkPos = {
        static_cast<float>(key.x * Chunk::CHUNK_SIZE_X),
        static_cast<float>(key.y * Chunk::CHUNK_SIZE_Y),
        static_cast<float>(key.z * Chunk::CHUNK_SIZE_Z)
    };

    float halfSize = Chunk::CHUNK_SIZE_X / 2.0f;

    // Verificar cada plano del frustum
    for (int i = 0; i < 6; ++i) {
        float distance = planes[i].x * chunkPos.x +
            planes[i].y * chunkPos.y +
            planes[i].z * chunkPos.z +
            planes[i].w;

        if (distance < -halfSize) {
            return false; // El chunk est� completamente fuera del frustum
        }
    }

    return true; // El chunk est� parcial o completamente dentro del frustum
}

#include <algorithm> // Aseg�rate de incluir esta cabecera para usar std::min

void World::LoadVisibleChunks(XMFLOAT3 cameraChunkPos) {
    float farPlane = m_camera->GetFarPlane();
    float loadRadius = std::min(farPlane / static_cast<float>(Chunk::CHUNK_SIZE_X), 12.0f); // L�mite m�ximo de 32 chunks

    int minX = static_cast<int>(floor(cameraChunkPos.x - loadRadius));
    int maxX = static_cast<int>(ceil(cameraChunkPos.x + loadRadius));
    int minY = static_cast<int>(floor(cameraChunkPos.y - loadRadius));
    int maxY = static_cast<int>(ceil(cameraChunkPos.y + loadRadius));
    int minZ = static_cast<int>(floor(cameraChunkPos.z - loadRadius));
    int maxZ = static_cast<int>(ceil(cameraChunkPos.z + loadRadius));

    XMFLOAT4 frustumPlanes[6];
    m_camera->ExtractFrustumPlanes(frustumPlanes);

    for (int i = 0; i < 6; ++i) {
        const XMFLOAT4& plane = frustumPlanes[i];

        // Verificar NaN
        //if (std::isnan(plane.x) || std::isnan(plane.y) || std::isnan(plane.z) || std::isnan(plane.w)) {
        //    std::cerr << "Plano " << i << " contiene NaN: "
        //        << plane.x << ", " << plane.y << ", " << plane.z << ", " << plane.w << std::endl;
        //    return; // Salir del renderizado
        //}

        //// Verificar infinito
        //if (!std::isfinite(plane.x) || !std::isfinite(plane.y) || !std::isfinite(plane.z) || !std::isfinite(plane.w)) {
        //    std::cerr << "Plano " << i << " contiene infinito: "
        //        << plane.x << ", " << plane.y << ", " << plane.z << ", " << plane.w << std::endl;
        //    return; // Salir del renderizado
        //}

        //// Verificar si todos los componentes son cero
        //if (plane.x == 0.0f && plane.y == 0.0f && plane.z == 0.0f && plane.w == 0.0f) {
        //    std::cerr << "Plano " << i << " tiene todos los componentes en 0." << std::endl;
        //    return; // Salir del renderizado
        //}
    }

    static int chunksLoadedThisFrame = 0;
    const int MAX_CHUNKS_PER_FRAME = 12000; // L�mite de chunks por fotograma

    for (int x = minX; x <= maxX && chunksLoadedThisFrame < MAX_CHUNKS_PER_FRAME; ++x) {
        for (int y = minY; y <= maxY && chunksLoadedThisFrame < MAX_CHUNKS_PER_FRAME; ++y) {
            for (int z = minZ; z <= maxZ && chunksLoadedThisFrame < MAX_CHUNKS_PER_FRAME; ++z) {
                ChunkKey key(x, y, z);
                if (!m_octree->IsChunkLoaded(key) && IsChunkInFrustum(key, frustumPlanes)) {
                    Chunk* newChunk = new Chunk(x, y, z);
                    m_octree->Insert(newChunk);
                    chunksLoadedThisFrame++;
                }
            }
        }
    }
}

void World::UnloadFarChunks(XMFLOAT3 cameraChunkPos) {
    float farPlane = m_camera->GetFarPlane();
    float unloadRadius = farPlane / Chunk::CHUNK_SIZE_X + 2;

    std::unordered_map<ChunkKey, Chunk*> chunksToRemove;
    m_octree->QueryChunksOutsideRadius(cameraChunkPos, unloadRadius, chunksToRemove);

    for (const auto& pair : chunksToRemove) {
        Chunk* chunk = pair.second;

        // Si el chunk es virtual, simplemente elim�nalo
        if (chunk->isVirtualChunk()) {
            m_octree->Remove(chunk);
            delete chunk;
            continue;
        }

        // Si el chunk es real, verifica si sus vecinos lo necesitan
        bool hasLoadedNeighbors = false;
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dz = -1; dz <= 1; ++dz) {
                    if (dx == 0 && dy == 0 && dz == 0) continue;
                    ChunkKey neighborKey(chunk->GetPosition().x + dx, chunk->GetPosition().y + dy, chunk->GetPosition().z + dz);
                    if (m_octree->IsChunkLoaded(neighborKey)) {
                        hasLoadedNeighbors = true;
                        break;
                    }
                }
            }
        }

        // Si no tiene vecinos cargados, eliminar el chunk
        if (!hasLoadedNeighbors) {
            m_octree->Remove(chunk);
            delete chunk;
        }
    }
}

bool World::isOnSameChunk(XMFLOAT3 pos1, XMFLOAT3 pos2) const {
    return (static_cast<int>(pos1.x) == static_cast<int>(pos2.x) &&
        static_cast<int>(pos1.y) == static_cast<int>(pos2.y) &&
        static_cast<int>(pos1.z) == static_cast<int>(pos2.z));
}