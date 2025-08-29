#pragma once

#include <DirectXMath.h>
#include <memory>
#include <vector>
#include "FrameStateBase.h"

class Chunk;

class TerrainFrameState : public FrameStateBase {
public:
    std::vector<std::shared_ptr<Chunk>> GetChunks() { return chunks; }
	void SetChunks(const std::vector<std::shared_ptr<Chunk>>& newChunks) { chunks = newChunks; }
private:
    std::vector<std::shared_ptr<Chunk>> chunks = {}; // Lista de chunks a renderizar
};

