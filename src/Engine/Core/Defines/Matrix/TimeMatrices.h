#pragma once

#include <DirectXMath.h>
#include "MatrixDefinitionBase.h" // Incluimos la base para MatrixParams

using namespace MatrixDefinitionBase;

namespace TimeMatrices
{
	struct TimeData
	{
		float time;
		DirectX::XMFLOAT3 padding; // Relleno para alinear a 16 bytes

		void SetMatrixData(std::map<std::string, std::shared_ptr<IMatrixParams>>& params) {
			std::shared_ptr<MatrixParams> baseParams = GetMatrixParams<MatrixParams>(params["BaseParams"]);
			this->time = baseParams->timeData.time;
		}

		UINT Size() {
			return sizeof(TimeData);
		}

		std::string MatrixType() {
			return MATRIX_TYPE_VERTEX.data();
		}
	};
} // namespace TimeMatrices