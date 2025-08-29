#pragma once

#include <vector>
#include <memory>

class IVertex;

class IMesh
{
	public:
	virtual ~IMesh() = default;
	//virtual void Draw() = 0;

	virtual std::vector<std::shared_ptr<IVertex>> GetVertices() const = 0;
	virtual std::vector<DirectX::XMFLOAT3> GetVerticesVectors() const { return {}; };
	virtual std::vector<UINT> GetIndexes() const = 0;
	virtual UINT GetIndexCount() const = 0;
	virtual UINT GetVertexCount() const = 0;
};
