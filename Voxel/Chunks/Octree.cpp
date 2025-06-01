#include "Octree.h"
#include <cmath>
#include <unordered_map>

Octree::Octree(const XMFLOAT3& center, float size, int depth)
    : m_center(center), m_size(size), m_depth(depth) {
    for (int i = 0; i < 8; ++i) {
        m_children[i] = nullptr;
    }
    m_chunks.clear();
    m_chunks.reserve(1000);
}

Octree::~Octree() {
    for (int i = 0; i < 8; ++i) {
        m_children[i].reset();
    }    
    m_chunks.clear();
}

void Octree::Insert(Chunk* chunk) {
    if (!IsChunkInNode(chunk)) {
        return; // El chunk no pertenece a este nodo
    }

    ChunkKey key(chunk->GetPosition());

    if (m_depth == 0 || m_size <= 1.0f) {
        m_chunks[key] = chunk; // Agregar el chunk al nodo actual
        return;
    }

    // Si no hay hijos, subdividir el nodo
    if (!m_children[0]) {
        Subdivide();
    }

    // Intentar insertar el chunk en los hijos
    for (int i = 0; i < 8; ++i) {
        if (m_children[i]->IsChunkInNode(chunk)) {
            m_children[i]->Insert(chunk);
            return;
        }
    }

    // Si no cabe en ningún hijo, agregarlo al nodo actual
    m_chunks[key] = chunk;
}

void Octree::Remove(Chunk* chunk) {
    if (!IsChunkInNode(chunk)) {
        return; // El chunk no pertenece a este nodo
    }

    // Intentar eliminar el chunk de este nodo
    ChunkKey key(chunk->GetPosition());
    auto it = m_chunks.find(key);
    if (it != m_chunks.end() && it->second == chunk) {
        m_chunks.erase(it);
        return;
    }

    // Intentar eliminar el chunk de los hijos
    for (int i = 0; i < 8; ++i) {
        if (m_children[i]) {
            m_children[i]->Remove(chunk);
        }
    }
}

void Octree::QueryFrustum(const XMFLOAT4 planes[6], std::unordered_map<ChunkKey, Chunk*>& visibleChunks) {
    if (!IsNodeInFrustum(planes)) {
        return; // Este nodo está completamente fuera del frustum
    }

    // Agregar los chunks de este nodo al mapa de visibles
    for (const auto& pair : m_chunks) {
        Chunk* chunk = pair.second;
        ChunkKey key(
            static_cast<int>(chunk->GetPosition().x / Chunk::CHUNK_SIZE_X),
            static_cast<int>(chunk->GetPosition().y / Chunk::CHUNK_SIZE_Y),
            static_cast<int>(chunk->GetPosition().z / Chunk::CHUNK_SIZE_Z)
        );
        visibleChunks[key] = chunk;
    }

    // Consultar los hijos
    for (int i = 0; i < 8; ++i) {
        if (m_children[i]) {
            m_children[i]->QueryFrustum(planes, visibleChunks);
        }
    }

    numVisibleChunks = static_cast<int>(m_chunks.size());
}

void Octree::QueryChunksOutsideRadius(const XMFLOAT3& center, float radius, std::unordered_map<ChunkKey, Chunk*> chunksToRemove) {
    float halfSize = m_size / 2.0f;

    // Calcular la distancia del nodo al centro
    float distanceSquared = 0.0f;
    for (int i = 0; i < 3; ++i) {
        float delta = 0.0f;
        if (i == 0) delta = std::max(0.0f, std::abs(m_center.x - center.x) - halfSize);
        if (i == 1) delta = std::max(0.0f, std::abs(m_center.y - center.y) - halfSize);
        if (i == 2) delta = std::max(0.0f, std::abs(m_center.z - center.z) - halfSize);
        distanceSquared += delta * delta;
    }

    // Si el nodo está completamente fuera del radio, agregar todos los chunks
    if (distanceSquared > radius * radius) {
        chunksToRemove.insert(m_chunks.begin(), m_chunks.end());
        return;
    }

    // Si no está completamente fuera, consultar los hijos
    for (int i = 0; i < 8; ++i) {
        if (m_children[i]) {
            m_children[i]->QueryChunksOutsideRadius(center, radius, chunksToRemove);
        }
    }
}

//bool Octree::IsChunkInNode(const ChunkKey& key) const {
//    // Convertimos el ChunkKey a una posición en el espacio 3D
//    XMFLOAT3 chunkPos = {
//        static_cast<float>(key.x * Chunk::CHUNK_SIZE_X),
//        static_cast<float>(key.y * Chunk::CHUNK_SIZE_Y),
//        static_cast<float>(key.z * Chunk::CHUNK_SIZE_Z)
//    };
//
//    float halfSize = m_size / 2.0f;
//
//    // Verificamos si la posición del chunk está dentro de los límites del nodo
//    return (chunkPos.x >= m_center.x - halfSize && chunkPos.x < m_center.x + halfSize &&
//        chunkPos.y >= m_center.y - halfSize && chunkPos.y < m_center.y + halfSize &&
//        chunkPos.z >= m_center.z - halfSize && chunkPos.z < m_center.z + halfSize);
//}

bool Octree::IsChunkLoaded(const ChunkKey& key) {
    return m_chunks.find(key) != m_chunks.end();
}

bool Octree::IsChunkInNode(const ChunkKey& key) const {
    // Verificar si el ChunkKey está dentro de los límites del nodo
    float halfSize = m_size / 2.0f;
    XMFLOAT3 chunkPos = {
        static_cast<float>(key.x * Chunk::CHUNK_SIZE_X),
        static_cast<float>(key.y * Chunk::CHUNK_SIZE_Y),
        static_cast<float>(key.z * Chunk::CHUNK_SIZE_Z)
    };

    if (!(chunkPos.x >= m_center.x - halfSize && chunkPos.x < m_center.x + halfSize &&
        chunkPos.y >= m_center.y - halfSize && chunkPos.y < m_center.y + halfSize &&
        chunkPos.z >= m_center.z - halfSize && chunkPos.z < m_center.z + halfSize)) {
        return false;
    }

    // Verificar si el ChunkKey existe en el mapa m_chunks
    // return m_chunks.find(key) != m_chunks.end();
    return true;
}

bool Octree::IsChunkInNode(const Chunk* chunk) const {
    return IsChunkInNode(ChunkKey(
        static_cast<int>(chunk->GetPosition().x),
        static_cast<int>(chunk->GetPosition().y),
        static_cast<int>(chunk->GetPosition().z)
    ));
}

bool Octree::IsNodeInFrustum(const XMFLOAT4 planes[6]) const {
    float halfSize = m_size / 2.0f;

    // Crear los 8 vértices del cubo que representa este nodo
    XMFLOAT3 corners[8] = {
        {m_center.x - halfSize, m_center.y - halfSize, m_center.z - halfSize},
        {m_center.x + halfSize, m_center.y - halfSize, m_center.z - halfSize},
        {m_center.x - halfSize, m_center.y + halfSize, m_center.z - halfSize},
        {m_center.x + halfSize, m_center.y + halfSize, m_center.z - halfSize},
        {m_center.x - halfSize, m_center.y - halfSize, m_center.z + halfSize},
        {m_center.x + halfSize, m_center.y - halfSize, m_center.z + halfSize},
        {m_center.x - halfSize, m_center.y + halfSize, m_center.z + halfSize},
        {m_center.x + halfSize, m_center.y + halfSize, m_center.z + halfSize},
    };

    // Probar cada vértice contra los planos del frustum
    for (int i = 0; i < 6; ++i) {
        int out = 0;
        for (int j = 0; j < 8; ++j) {
            float distance = planes[i].x * corners[j].x +
                planes[i].y * corners[j].y +
                planes[i].z * corners[j].z +
                planes[i].w;
            if (distance < 0) {
                ++out;
            }
        }
        if (out == 8) {
            return false; // Todos los vértices están fuera de este plano
        }
    }

    return true; // Parcial o completamente dentro del frustum
}

void Octree::Subdivide() {
    float quarterSize = m_size / 4.0f;
    float halfSize = m_size / 2.0f;

    for (int i = 0; i < 8; ++i) {
        XMFLOAT3 offset = {
            (i & 1 ? quarterSize : -quarterSize),
            (i & 2 ? quarterSize : -quarterSize),
            (i & 4 ? quarterSize : -quarterSize),
        };
        m_children[i] = std::make_unique<Octree>(XMFLOAT3(m_center.x + offset.x, m_center.y + offset.y, m_center.z + offset.z), halfSize, m_depth - 1);
    }
}

int Octree::CountChunks() const
{
    // Contar los chunks en el nodo actual
    int count = static_cast<int>(m_chunks.size());

    // Recorrer los hijos y sumar sus chunks
    for (int i = 0; i < 8; ++i) {
        if (m_children[i]) {
            count += m_children[i]->CountChunks();
        }
    }

    return count;
}

int Octree::CountVisibleChunks(const XMFLOAT4 planes[6]) const
{
    // Si el nodo no está en el frustum, no hay chunks visibles en este nodo ni en sus hijos
    if (!IsNodeInFrustum(planes)) {
        return 0;
    }

    // Contar los chunks visibles en el nodo actual
    int count = static_cast<int>(m_chunks.size());

    // Recorrer los hijos y sumar los chunks visibles
    for (int i = 0; i < 8; ++i) {
        if (m_children[i]) {
            count += m_children[i]->CountVisibleChunks(planes);
        }
    }

    return count;
}

std::vector<Chunk*> Octree::GetChunks(const XMFLOAT4 planes[6]) const
{
    std::vector<Chunk*> visibleChunks;

    // Si el nodo no está en el frustum, no hay chunks visibles en este nodo ni en sus hijos  
    if (!IsNodeInFrustum(planes)) {
        return visibleChunks;
    }

    // Agregar los chunks visibles en el nodo actual  
    for (const auto& pair : m_chunks) {
        visibleChunks.push_back(pair.second);
    }

    // Recorrer los hijos y agregar los chunks visibles  
    for (int i = 0; i < 8; ++i) {
        if (m_children[i]) {
            std::vector<Chunk*> childVisibleChunks = m_children[i]->GetChunks(planes);
            visibleChunks.insert(visibleChunks.end(), childVisibleChunks.begin(), childVisibleChunks.end());
        }
    }

    return visibleChunks;
 
    
}
