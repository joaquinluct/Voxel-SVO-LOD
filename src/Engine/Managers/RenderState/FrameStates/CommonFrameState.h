#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <wrl/client.h>
#include "FrameStateBase.h"
#include <Defines/Pipeline.h>

class CommonFrameState : public FrameStateBase {
public:

	float GetDeltaTime() const { return deltaTime; }
	float GetHeight() const { return m_height; }
	float GetWidth() const { return m_width; }
	DirectX::XMMATRIX GetWorldMatrix() const { return worldMatrix; }

	void SetDeltaTime(float dt) { deltaTime = dt; }
	void SetHeight(float height) { m_height = height; }
	void SetWidth(float width) { m_width = width; }
	void SetWorldMatrix(const DirectX::XMMATRIX& matrix) { worldMatrix = matrix; }    

private:
	float deltaTime = 0.0f;
	float m_height = 600.0f;	
	float m_width = 800.0f;
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();
	
};

