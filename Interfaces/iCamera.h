#pragma once
#include <directxmath.h>

using namespace DirectX;

class iCamera {
public:
    virtual void SetPosition(float x, float y, float z) = 0;
    virtual void SetRotation(float pitch, float yaw, float roll) = 0;
    virtual XMMATRIX GetViewMatrix() const = 0;
	virtual XMFLOAT3 GetPosition() const = 0;
};

