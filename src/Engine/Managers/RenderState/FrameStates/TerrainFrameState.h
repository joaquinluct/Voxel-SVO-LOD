#pragma once

#include <DirectXMath.h>
#include <memory>
#include <vector>
#include "FrameStateBase.h"

class Chunk;

class TerrainFrameState : public FrameStateBase {
public:
    std::vector<Chunk*> GetChunks() { return chunks; }
	void SetChunks(const std::vector<Chunk*>& newChunks) { chunks = newChunks; }
private:
    std::vector<Chunk*> chunks = {}; // Lista de chunks a renderizar
};

