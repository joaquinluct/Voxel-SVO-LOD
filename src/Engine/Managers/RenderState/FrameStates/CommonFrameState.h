#pragma once

#include "FrameStateBase.h"
#include <chrono>
#include <Defines/EngineDefinition.h>
#include <DirectXMath.h>

class CommonFrameState : public FrameStateBase {
public:
    float GetDeltaTime() const { return engineContext->deltaTime; }
    float GetHeight() const { return engineContext->height; }
    float GetWidth() const { return engineContext->width; }
    DirectX::XMMATRIX GetWorldMatrix() const { return worldMatrix; }
    std::chrono::system_clock::time_point GetSimulatedTime() { return simulatedTime; }

    void SetWorldMatrix(const DirectX::XMMATRIX& matrix) { worldMatrix = matrix; }
    void SetEngineContext(EngineContext* context) { engineContext = context; }
    void SetSimulatedTime(std::chrono::system_clock::time_point simulatedTimed) { this->simulatedTime = simulatedTimed; }

private:
    std::chrono::system_clock::time_point simulatedTime;
    EngineContext* engineContext;
    DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixIdentity();

};

