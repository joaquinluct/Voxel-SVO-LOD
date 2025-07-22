#pragma once
#include <string>
#include <vector>
#include <Defines/VertexDefinition.h>
#include "IAssetMeshConfigBase.h"

template <typename T>
struct IAssetMeshConfig : public IAssetMeshConfigBase
{
	~IAssetMeshConfig() override = default;
	std::vector<T> m_vertext;
	std::vector<uint16_t> m_index;
	void SetVertex(const std::vector<T>& vertex) { m_vertext = vertex; }
	void SetIndex(const std::vector<uint16_t>& index) { this->m_index = index; }
	std::vector<T> GetVertex() {
		return m_vertext;
	}
	std::vector<uint16_t> GetIndex() {
		return m_index;
	}
	std::string type;
	bool has_normals;
	bool has_indices;
	bool has_tangents;
	bool has_uvs;
	bool has_colors;
	bool has_bones;

	std::string GetType() const override { return type; }
};
