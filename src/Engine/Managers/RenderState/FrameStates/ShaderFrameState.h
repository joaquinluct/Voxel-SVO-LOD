#pragma once

#include <DirectXMath.h>
#include <string>
#include <map>
#include "FrameStateBase.h"
#include <Defines/Matrix/MatrixDefinition.h>

class ShaderFrameState : public FrameStateBase {
public:

	std::map<int, std::pair<std::string, MatrixDefinition::AnyMatrixBuffer>> GetMatrices() const { return matrices; }
	void SetMatrices(const std::map<int, std::pair<std::string, MatrixDefinition::AnyMatrixBuffer>>& mats) { matrices = mats; }
	void ClearMatrices() { matrices.clear(); }
	void AddMatrix(int slot, const std::string& name, const MatrixDefinition::AnyMatrixBuffer& matrix) { matrices[slot] = std::make_pair(name, matrix); }
	void SetShaderName(const std::string& name) { shaderName = name; }
	std::string GetShaderName() const { return shaderName; }
	
private:
	std::map<int, std::pair<std::string, MatrixDefinition::AnyMatrixBuffer>> matrices;
	std::string shaderName;
};



