#pragma once
#define NOMINMAX
#include <d3d11.h>
#include <DirectXMath.h>
#include <Windows.h>
#include <vector>
#include <unordered_map>
#include <optional>
#include <memory>
#include <IChunk.h>
#include <ICamera.h>
#include <IDefine/IVertex.h>
#include <Defines/TerrainChunk.h>
#include <Defines/VertexDefinition.h>
#include <Game/Systems/Terrain/ProceduralService.h>
#include <DeviceManager.h>

//class MeshAsset;

class Chunk : public IChunk {
private:
	const float m_terrainHeight = 250.0f; // Multipicador para a altura do terreno
	bool m_dirty = true;
	std::shared_ptr<ProceduralEngineBase> m_proceduralEngine;
	TerrainChunk::ChunkID m_id;
	DirectX::XMFLOAT3 m_position;
	float m_chunkSize;
	int m_currentLOD = -1;

	std::vector<std::shared_ptr<IVertex>> m_vertices;
	std::vector<UINT> m_indexes;

	std::shared_ptr<DeviceManager> m_deviceManager;

	Util::BoundingBox m_boundingBox;

	//std::shared_ptr<MeshAsset> m_mesh;

	std::shared_ptr<Chunk> m_neighbors[4];

	int m_northLOD = -1;
	int m_southLOD = -1;
	int m_eastLOD = -1;
	int m_westLOD = -1;

	std::optional<XMFLOAT3> InterpolateVertex(float globalX, float globalZ, int gridSize, const TerrainChunk::NeighborDirection& dir, const DirectX::XMFLOAT4& debugColor = {});
	void GenerateVertices(int gridSize, int lodLevel, std::vector<std::shared_ptr<IVertex>>& outVertices);
	void GenerateIndices(int gridSize, int lodLevel, std::vector<UINT>& outIndices);
	void CalculateNormals(std::vector<std::shared_ptr<IVertex>>& processedVertices);
	std::shared_ptr<IVertex> FindVertexByPosition(const DirectX::XMFLOAT3& position, float tolerance) const;

	std::vector<std::tuple<UINT, UINT, UINT>> GetTrianglesTouchingBorder(int direction, float epsilon) const;
	std::vector<std::shared_ptr<IVertex>> GetBorderVerticesFromTriangles(int direction, float epsilon) const;
public:
	Chunk()
		: m_position(), m_chunkSize(0.0f), m_id(), m_proceduralEngine(nullptr)
	{
	}
	Chunk(const DirectX::XMFLOAT3 position, const float size)
		: m_position(position), m_chunkSize(size), m_id(), m_proceduralEngine(nullptr)
	{
	}
	Chunk(TerrainChunk::ChunkID id)
		: m_position(), m_chunkSize(), m_id(id), m_currentLOD(-1), m_proceduralEngine(nullptr)
	{
	}
	virtual ~Chunk() = default;

	// IChunk interface
	HRESULT Init() override;	
	void Render() override {}
	void Update() override {}
	void Shutdown() override;
	// IMesh interface
	std::vector<std::shared_ptr<IVertex>> GetVertices() const override {
		return m_vertices;
	}
	std::vector<DirectX::XMFLOAT3> GetVerticesVectors() const override;

	std::vector<UINT> GetIndexes() const override {
		return m_indexes;
	}
	UINT GetIndexCount() const override {
		return static_cast<UINT>(m_indexes.size());
	}
	UINT GetVertexCount() const override {
		return static_cast<UINT>(m_vertices.size());
	}


	/*void SetMesh(std::shared_ptr<MeshAsset> mesh) override {
		m_mesh = mesh;
	}*/
	/*std::shared_ptr<MeshAsset> GetMesh() override {
		return m_mesh;
	}*/

	DirectX::XMFLOAT3 GetPosition() const override {
		return m_position;
	}

	void SetProceduralEngine(std::shared_ptr<ProceduralEngineBase> proceduralEngine) {
		m_proceduralEngine = proceduralEngine;
	}

	void SetChunkSize(float size) {
		m_chunkSize = size;
	}
	int GetCurrentLOD() const override {
		return m_currentLOD;
	}
	void UpdateLOD(int lodLevel) override;
	void GenerateLODMesh(int lodLevel);
	void SetCurrentLOD(int lodLevel) override {
		if (!m_dirty) {
			m_dirty = (lodLevel != m_currentLOD);
		}
		m_currentLOD = lodLevel;
	}

	void SetDirty(bool dirty) override {
		m_dirty = dirty;
	}
	bool IsDirty() const override {
		return m_dirty;
	}

	float GetDistanceToCamera(std::shared_ptr<ICamera> camera) const;


	TerrainChunk::ChunkID GetID() const override { return m_id; };

	IChunk* GetChunk(const TerrainChunk::ChunkID& id) const;

	const Util::BoundingBox& GetAABB() const {
		return m_boundingBox;
	}

	std::vector<std::shared_ptr<IVertex>> GetBorderVertices(TerrainChunk::NeighborDirection dir, int gridSize) const;

	void SetNeighborLODs(int north, int south, int east, int west) {
		m_northLOD = north;
		m_southLOD = south;
		m_eastLOD = east;
		m_westLOD = west;
	}

	void SetNeighbor(TerrainChunk::NeighborDirection direction, std::shared_ptr<Chunk> neighbor) {
		int dir = static_cast<int>(direction);
		if (dir >= 0 && dir < 4) {
			m_neighbors[dir] = neighbor;
		}
	}
	void SetNeighbors(const std::shared_ptr<Chunk> neighbors[4]) {
		for (int i = 0; i < 4; ++i) {
			m_neighbors[i] = neighbors[i];
		}
	}
};


using ChunkMap = std::unordered_map<TerrainChunk::ChunkID, std::shared_ptr<Chunk>, TerrainChunk::ChunkHasher>;