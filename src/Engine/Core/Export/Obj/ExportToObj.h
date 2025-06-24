#pragma once
#include <string>
#include <vector>
#include "Material/includes/VertexDefinition.h"

template<typename TVertex, typename TIndex>
void ExportToOBJ_Dynamic(const std::string& filename,
    const std::vector<TVertex>& vertices,
    const std::vector<TIndex>& indices)
{
    std::ofstream out(filename);
    if (!out) throw std::runtime_error("No se pudo abrir el archivo OBJ");

    out << "# Exportado dinámicamente por Copilot\n\n";

    for (const auto& v : vertices) {
        out << TVertex::ToOBJLine(v) << "\n";
    }

    for (size_t i = 0; i + 2 < indices.size(); i += 3) {
        out << std::format("f {} {} {}\n",
            indices[i] + 1, indices[i + 1] + 1, indices[i + 2] + 1);
    }

    out.close();
}

