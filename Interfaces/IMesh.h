#pragma once

#include <DirectXMath.h>
#include <IDefine/IVertex.h>
#include <vector>
#include <Windows.h>


class IMesh
{
public:
    virtual ~IMesh() = default;
    //virtual void Draw() = 0;

    virtual std::vector<IVertex> GetVertices() = 0;
    virtual std::vector<DirectX::XMFLOAT3> GetVerticesVectors() const { return {}; };
    virtual std::vector<UINT> GetIndexes() const = 0;
    virtual UINT GetIndexCount() const = 0;
    virtual UINT GetVertexCount() const = 0;
};
