#pragma once
#include <vector>
#include <unordered_map>
#include "../../Util/Utils.h"

// Estructura para contener la malla generada (v�rtices �nicos + �ndices)
struct MarchingCubesMesh {
    std::vector<MarchingCubesVertex> vertices;
    std::vector<unsigned int> indices;
    void addMesh(const MarchingCubesMesh& other) {
        size_t vertexOffset = vertices.size();
        vertices.insert(vertices.end(), other.vertices.begin(), other.vertices.end());
        for (unsigned int originalOtherIndex : other.indices) {
            indices.push_back(originalOtherIndex + static_cast<unsigned int>(vertexOffset));
        }
    }
    void removeDuplicateVertices() {
        std::vector<MarchingCubesVertex> newVertices;
        std::vector<unsigned int> newIndices;
        std::unordered_map<MarchingCubesVertex, unsigned int> uniqueVertexMap;
        unsigned int nextIndex = 0;

        // Recorrer cada índice de la malla actual
        for (unsigned int originalIndex : indices) {
            // Obtener el vértice correspondiente de la lista actual de vértices
            const MarchingCubesVertex& currentVertex = vertices[originalIndex];

            // Intentar insertar el vértice en el mapa de vértices únicos
            // 'it' será un iterador al elemento en el mapa.
            // 'inserted' será true si el vértice se insertó (es nuevo), false si ya existía.
            auto [it, inserted] = uniqueVertexMap.insert({ currentVertex, nextIndex });

            if (inserted) {
                // Si el vértice es nuevo, lo añadimos a nuestra lista de vértices únicos
                newVertices.push_back(currentVertex);
                // Incrementamos el contador para el próximo índice nuevo disponible
                nextIndex++;
            }
            // Añadimos el índice mapeado (el índice del vértice en newVertices)
            // al nuevo array de índices. Si el vértice ya existía, 'it->second'
            // contendrá el índice original de ese vértice en newVertices.
            newIndices.push_back(it->second);
        }

        // Reemplazar los arrays de vértices e índices de la malla con los nuevos
        vertices = std::move(newVertices); // Usa std::move para eficiencia
        indices = std::move(newIndices);   // Usa std::move para eficiencia
    }
};
