#pragma once

#include <Defines/VertexDefinition.h>
#include <string>
#include <vector>
#include <variant>
#include <algorithm>
#include <iostream>
#include <map>
#include <DirectXMath.h>
#include <../Libs/TinyObjLoader/tiny_obj_loader.h>
#include <Util/DirectXUtils.h> // ¡Asegúrate de que esta ruta sea correcta para tu DirectXUtils.h!

namespace ObjVertexDefinition {
    struct Vertex {
        float x, y, z;
        float nx, ny, nz;
        float u, v;

        Vertex(float px, float py, float pz) : x(px), y(py), z(pz), nx(0), ny(0), nz(0), u(0), v(0) {}
        Vertex(float px, float py, float pz, float pnx, float pny, float pnz, float pu, float pv)
            : x(px), y(py), z(pz), nx(pnx), ny(pny), nz(pnz), u(pu), v(pv) {
        }

        bool operator<(const Vertex& other) const {
            if (x != other.x) return x < other.x;
            if (y != other.y) return y < other.y;
            if (z != other.z) return z < other.z;
            if (nx != other.nx) return nx < other.nx;
            if (ny != other.ny) return ny < other.ny;
            if (nz != other.nz) return nz < other.nz;
            if (u != other.u) return u < other.u;
            return v < other.v;
        }
    };
}

class ObjUtil
{
public:
    ~ObjUtil() = default;

    static tinyobj::ObjReader getReader(const std::string& filename) {
        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = "./";

        tinyobj::ObjReader reader;

        if (!reader.ParseFromFile(filename, reader_config)) {
            if (!reader.Error().empty()) {
                std::cerr << "TinyObjReader: " << reader.Error() << std::endl;
            }
            return {};
        }

        if (!reader.Warning().empty()) {
            std::cout << "TinyObjReader: " << reader.Warning() << std::endl;
        }

        return reader;
    }

    static bool LoadObj(const std::string& filename, std::vector<std::shared_ptr<VertexDefinition::VertexVariant>>& outVertices, std::vector<uint16_t>& outIndexes) {
        tinyobj::ObjReader reader = getReader(filename);

        if (!reader.Valid()) {
            return false;
        }

        auto& attrib = reader.GetAttrib();
        auto& shapes = reader.GetShapes();

        std::map<ObjVertexDefinition::Vertex, uint16_t> uniqueVerticesMap;
        std::vector<ObjVertexDefinition::Vertex> processedVertices;

        outIndexes.clear();
        outVertices.clear();

        uint16_t currentVertexIndex = 0;

        for (const auto& shape : shapes) {
            size_t index_offset = 0;
            for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
                size_t fv = size_t(shape.mesh.num_face_vertices[f]);

                if (fv != 3) {
                    std::cerr << "Advertencia: La cara no es un triángulo. No se calcularán tangentes correctamente para esta cara." << std::endl;
                    index_offset += fv;
                    continue;
                }

                for (size_t v_idx_in_face = 0; v_idx_in_face < fv; v_idx_in_face++) {
                    tinyobj::index_t idx = shape.mesh.indices[index_offset + v_idx_in_face];

                    tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                    tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                    tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                    tinyobj::real_t nx = 0.0f, ny = 0.0f, nz = 0.0f;
                    if (idx.normal_index >= 0) {
                        nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
                        ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
                        nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
                    }

                    tinyobj::real_t tx = 0.0f, ty = 0.0f;
                    if (idx.texcoord_index >= 0) {
                        tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
                        ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
                    }

                    ObjVertexDefinition::Vertex currentFaceVertex(vx, vy, vz, nx, ny, nz, tx, ty);

                    auto it = uniqueVerticesMap.find(currentFaceVertex);
                    if (it == uniqueVerticesMap.end()) {
                        uniqueVerticesMap[currentFaceVertex] = currentVertexIndex;
                        outIndexes.push_back(currentVertexIndex);
                        processedVertices.push_back(currentFaceVertex);
                        currentVertexIndex++;
                    }
                    else {
                        outIndexes.push_back(it->second);
                    }
                }
                index_offset += fv;
            }
        }

        if (processedVertices.empty()) {
            std::cerr << "Error: No se cargaron vértices del archivo OBJ." << std::endl;
            return false;
        }

        std::vector<DirectX::XMFLOAT3> tangents(processedVertices.size(), { 0.0f, 0.0f, 0.0f });
        std::vector<DirectX::XMFLOAT3> bitangents(processedVertices.size(), { 0.0f, 0.0f, 0.0f });
        std::vector<int> vertexTangentCounts(processedVertices.size(), 0);

        for (size_t i = 0; i < outIndexes.size(); i += 3) {
            uint16_t i0 = outIndexes[i + 0];
            uint16_t i1 = outIndexes[i + 1];
            uint16_t i2 = outIndexes[i + 2];

            const ObjVertexDefinition::Vertex& v0 = processedVertices[i0];
            const ObjVertexDefinition::Vertex& v1 = processedVertices[i1];
            const ObjVertexDefinition::Vertex& v2 = processedVertices[i2];

            DirectX::XMVECTOR p0 = DirectX::XMVectorSet(v0.x, v0.y, v0.z, 1.0f);
            DirectX::XMVECTOR p1 = DirectX::XMVectorSet(v1.x, v1.y, v1.z, 1.0f);
            DirectX::XMVECTOR p2 = DirectX::XMVectorSet(v2.x, v2.y, v2.z, 1.0f);

            // Coordenadas de textura como XMVECTOR
            DirectX::XMVECTOR uv0_vec = DirectX::XMVectorSet(v0.u, v0.v, 0.0f, 0.0f);
            DirectX::XMVECTOR uv1_vec = DirectX::XMVectorSet(v1.u, v1.v, 0.0f, 0.0f);
            DirectX::XMVECTOR uv2_vec = DirectX::XMVectorSet(v2.u, v2.v, 0.0f, 0.0f);

            DirectX::XMVECTOR edge1_vec = DirectX::XMVectorSubtract(p1, p0);
            DirectX::XMVECTOR edge2_vec = DirectX::XMVectorSubtract(p2, p0);

            // Obtener las diferencias de UVs como floats
            float uv1x = DirectX::XMVectorGetX(uv1_vec) - DirectX::XMVectorGetX(uv0_vec);
            float uv1y = DirectX::XMVectorGetY(uv1_vec) - DirectX::XMVectorGetY(uv0_vec);
            float uv2x = DirectX::XMVectorGetX(uv2_vec) - DirectX::XMVectorGetX(uv0_vec);
            float uv2y = DirectX::XMVectorGetY(uv2_vec) - DirectX::XMVectorGetY(uv0_vec);

            float det = (uv1x * uv2y - uv2x * uv1y);
            float r = (det == 0.0f) ? 0.0f : (1.0f / det);

            // Aquí convertimos XMVECTOR a XMFLOAT3 para usar DirectXUtils
            DirectX::XMFLOAT3 edge1, edge2;
            DirectX::XMStoreFloat3(&edge1, edge1_vec);
            DirectX::XMStoreFloat3(&edge2, edge2_vec);

            DirectX::XMFLOAT3 T, B;

            // T = r * ( (edge1 * uv2y) - (edge2 * uv1y) )
            T = DirectXUtils::Multiply(
                DirectXUtils::Subtract(
                    DirectXUtils::Multiply(edge1, uv2y),
                    DirectXUtils::Multiply(edge2, uv1y)
                ), r
            );

            // B = r * ( (edge2 * uv1x) - (edge1 * uv2x) )
            B = DirectXUtils::Multiply(
                DirectXUtils::Subtract(
                    DirectXUtils::Multiply(edge2, uv1x),
                    DirectXUtils::Multiply(edge1, uv2x)
                ), r
            );

            // Acumular las tangentes y bitangentes por vértice
            tangents[i0] = DirectXUtils::Add(tangents[i0], T);
            bitangents[i0] = DirectXUtils::Add(bitangents[i0], B);
            vertexTangentCounts[i0]++;

            tangents[i1] = DirectXUtils::Add(tangents[i1], T);
            bitangents[i1] = DirectXUtils::Add(bitangents[i1], B);
            vertexTangentCounts[i1]++;

            tangents[i2] = DirectXUtils::Add(tangents[i2], T);
            bitangents[i2] = DirectXUtils::Add(bitangents[i2], B);
            vertexTangentCounts[i2]++;
        }

        outVertices.reserve(processedVertices.size());
        for (size_t i = 0; i < processedVertices.size(); ++i) {
            const auto& pVert = processedVertices[i];

            DirectX::XMFLOAT3 finalTangent = { 0.0f, 0.0f, 0.0f };
            // DirectX::XMFLOAT3 finalBitangent ya no es necesaria en la estructura final del vértice,
            // pero aún la calculamos internamente para la ortogonalización y para la referencia de handedness si fuera necesario.

            if (vertexTangentCounts[i] > 0) {
                // Calcular promedio y normalizar tangente
                DirectX::XMFLOAT3 T_avg = DirectXUtils::Normalize(DirectXUtils::Divide(tangents[i], (float)vertexTangentCounts[i]));

                // Normal del vértice
                DirectX::XMFLOAT3 N = DirectX::XMFLOAT3(pVert.nx, pVert.ny, pVert.nz);

                // Proyectar T_avg sobre el plano de la normal para asegurar ortogonalidad (Gram-Schmidt)
                // T_avg = T_avg - N * dot(T_avg, N)
                T_avg = DirectXUtils::Subtract(T_avg, DirectXUtils::Multiply(N, DirectXUtils::Dot(T_avg, N)));
                T_avg = DirectXUtils::Normalize(T_avg); // Normalizar de nuevo después de la proyección

                // Opcional: Calcular la bitangente para lahandedness, aunque no la almacenemos en el vértice
                // Esto es útil para verificar que la tangente, bitangente y normal forman un sistema de coordenadas a derechas/izquierdas.
                // DirectX::XMFLOAT3 B_avg = DirectXUtils::Normalize(DirectXUtils::Cross(N, T_avg)); 

                finalTangent = T_avg;
            }

            DirectX::XMFLOAT3 position(pVert.x, pVert.y, pVert.z);
            DirectX::XMFLOAT3 normal(pVert.nx, pVert.ny, pVert.nz);
            DirectX::XMFLOAT2 texCoord(pVert.u, pVert.v);
			DirectX::XMFLOAT4 debugColor(0.0f, 0.0f, 0.0f, 0.0f); // Color negro como valor por desactivado

            // Utiliza la estructura TextureMapVertex
            VertexDefinition::TextureMapVertex vtt = { position, texCoord, normal, finalTangent, debugColor };
            
            outVertices.push_back(std::make_shared<VertexDefinition::VertexVariant>(vtt));
        }

        return true;
    }
};