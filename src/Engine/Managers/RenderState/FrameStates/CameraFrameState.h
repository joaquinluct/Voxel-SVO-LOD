#pragma once

#include <DirectXMath.h>
#include "FrameStateBase.h"

class CameraFrameState : public FrameStateBase {
public:	
    DirectX::XMMATRIX GetViewMatrix() const { return viewMatrix; }
	DirectX::XMMATRIX GetProjectionMatrix() const { return projectionMatrix; }
	DirectX::XMFLOAT3 GetCameraPosition() const { return cameraPosition; }
	void SetCameraPosition(const DirectX::XMFLOAT3& position) { cameraPosition = position; }
	void SetViewMatrix(const DirectX::XMMATRIX& matrix) { viewMatrix = matrix; }
	void SetProjectionMatrix(const DirectX::XMMATRIX& matrix) { projectionMatrix = matrix; }
private:
    DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX projectionMatrix = DirectX::XMMatrixIdentity();
	DirectX::XMFLOAT3 cameraPosition = { 0.0f, 0.0f, 0.0f };
};