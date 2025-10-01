#pragma once

#include "MatrixDefinitionBase.h" // Incluimos la base para MatrixParams
#include <Defines/Enums/Matrix.h>
//#include <DirectXMath.h>
#include <string>
#include <memory>
#include <map>

using namespace MatrixDefinitionBase;

namespace TimeMatrices
{
    struct TimeData
    {
        float time;

        void SetTime(float t) { time = t; }

        void SetMatrixData(std::map<std::string, std::shared_ptr<IMatrixParams>>& params) {
            std::shared_ptr<MatrixParams> baseParams = GetMatrixParams<MatrixParams>(params["BaseParams"]);
            this->time = baseParams->timeData.time;
        }

        UINT Size() {
            return sizeof(TimeData);
        }

        MatrixBufferTypeEnum BufferType() {
            return MatrixBufferTypeEnum::Dynamic;
        }

        std::string MatrixType() {
            return MATRIX_TYPE_VERTEX.data();
        }
        UINT Slot() {
            return 8;
        }
    };
} // namespace TimeMatrices
