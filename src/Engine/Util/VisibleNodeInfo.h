#pragma once
#include <DirectXMath.h> // Asegúrate de incluir la librería DirectXMath para XMFLOAT3
#include "../SVO/SVOBase/SVO_Node.h" // Asegúrate de incluir la definición de SVO_Node
#include "../SVO/MarchingCubes/MarchingCubesMesh.h" // Asegúrate de incluir la definición de MarchingCubesMesh

struct VisibleNodeInfo {
    SVO_Node* node;
    DirectX::XMFLOAT3 origin;
    float size;
    MarchingCubesMesh* m_marchingCubesMesh;

    // Operador de igualdad
    bool operator==(const VisibleNodeInfo& other) const {
        return node == other.node &&
            origin.x == other.origin.x &&
            origin.y == other.origin.y &&
            origin.z == other.origin.z &&
            size == other.size;
    }

    // Operador de desigualdad
    bool operator!=(const VisibleNodeInfo& other) const {
        return !(*this == other); // Reutiliza el operador ==
    }
};

struct NeightborsDensitiesNormal {
	const float stepSize = 0.5f; // Tamaño del paso para calcular las posiciones vecinas
    float xMin;
    float xMax;
    float yMin;
    float yMax;
    float zMin;
    float zMax;
    DirectX::XMFLOAT3 posXMax;
    DirectX::XMFLOAT3 posXMin;
    DirectX::XMFLOAT3 posYMax;
    DirectX::XMFLOAT3 posYMin;
    DirectX::XMFLOAT3 posZMax;
    DirectX::XMFLOAT3 posZMin;

    NeightborsDensitiesNormal(DirectX::XMFLOAT3 pos) {
        posXMax = GetXMaxPos(pos);
        posXMin = GetXMinPos(pos);
        posYMax = GetYMaxPos(pos);
        posYMin = GetYMinPos(pos);
        posZMax = GetZMaxPos(pos);
        posZMin = GetZMinPos(pos);
    };

    DirectX::XMFLOAT3 GetXMaxPos(DirectX::XMFLOAT3 pos) {
        return DirectX::XMFLOAT3{ pos.x + stepSize, pos.y, pos.z };
    }
    DirectX::XMFLOAT3 GetXMinPos(DirectX::XMFLOAT3 pos) {
        return DirectX::XMFLOAT3{ pos.x - stepSize, pos.y, pos.z };
    }
    DirectX::XMFLOAT3 GetYMaxPos(DirectX::XMFLOAT3 pos) {
        return DirectX::XMFLOAT3{ pos.x, pos.y + stepSize, pos.z };
    }
    DirectX::XMFLOAT3 GetYMinPos(DirectX::XMFLOAT3 pos) {
        return DirectX::XMFLOAT3{ pos.x, pos.y - stepSize, pos.z };
    }
    DirectX::XMFLOAT3 GetZMaxPos(DirectX::XMFLOAT3 pos) {
        return DirectX::XMFLOAT3{ pos.x, pos.y, pos.z + stepSize };
    }
    DirectX::XMFLOAT3 GetZMinPos(DirectX::XMFLOAT3 pos) {
        return DirectX::XMFLOAT3{ pos.x, pos.y, pos.z - stepSize };
    }

    DirectX::XMFLOAT3 GetNormal() {
        DirectX::XMFLOAT3 normal = DirectX::XMFLOAT3{
            xMax - xMin,
            yMax - yMin,
            zMax - zMin
        };
        XMVECTOR xmNormal = XMVector3Normalize(XMLoadFloat3(&normal));
        XMStoreFloat3(&normal, xmNormal);

        return normal;
    };

    
};